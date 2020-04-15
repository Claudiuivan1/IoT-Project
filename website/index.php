<!DOCTYPE html>
<html>
    <head>
        <title>Sensors</title>
        <meta charset="utf-8">

        <link rel="stylesheet" type="text/css" href="./css/bootstrap.min.css">
        
        <script src="./js/paho-mqtt-min.js"></script>
        <script src="./js/jquery-3.4.1.min.js"></script>
        <script src="./js/bootstrap.min.js"></script>
    </head>
    <body>
        <div class="container mt-5">
            <h1 class="mb-4">Sensors</h1>

            <!-- Real time sensor values -->
            <div class="row">
                <div class="col">
                    <div class="alert alert-primary" id="sens0" role="alert">
                        <h4 class="alert-heading pt-2">Station n.1</h4>
                            
                        <hr>

                        <div class="row">
                            <div class="col temp"></div>
                        </div>
                        <div class="row">
                            <div class="col hum"></div>
                        </div>
                        <div class="row">
                            <div class="col w_dir">Retriving data...</div>
                        </div>
                        <div class="row">
                            <div class="col w_int"></div>
                        </div>
                        <div class="row">
                            <div class="col r_heig"></div>
                        </div>
                    </div>
                </div>
                <div class="col">
                    <div class="alert alert-primary" id="sens1" role="alert">
                        <h4 class="alert-heading pt-2">Station n.2</h4>
                        
                        <hr>

                        <div class="row">
                            <div class="col temp"></div>
                        </div>
                        <div class="row">
                            <div class="col hum"></div>
                        </div>
                        <div class="row">
                            <div class="col w_dir">Retriving data...</div>
                        </div>
                        <div class="row">
                            <div class="col w_int"></div>
                        </div>
                        <div class="row">
                            <div class="col r_heig"></div>
                        </div>
                    </div>
                </div>
            </div>

            <?php
                // PHP connection to DB
                $link = mysqli_connect("localhost", "claudiu", "12345678", "sensors");

                // Perform SQL queries for both the sensors, retriving past hour data
                $query = "SELECT * FROM sensors WHERE id = 1 ORDER BY time_id DESC LIMIT 4";
                $result1 = $link->query( $query );

                $query = "SELECT * FROM sensors WHERE id = 2 ORDER BY time_id DESC LIMIT 4";
                $result2 = $link->query( $query );
            ?>   

            <!-- Last hour sensor values -->
            <div class="row">
                <div class="col">
                    <div class="alert alert-primary" role="alert">
                        <h4 class="alert-heading pt-2">Last hour</h4>
                            
                        <hr>

                        <table class="table">
                            <thead>
                                <tr>
                                    <th scope="col">Temp</th>
                                    <th scope="col">Hum</th>
                                    <th scope="col">W. Dir</th>
                                    <th scope="col">W. Int</th>
                                    <th scope="col">R. Heig</th>
                                </tr>
                            </thead>
                            <tbody>
                                <!-- Decode messages and print values -->
                                <?php while( $row = $result1->fetch_array(  ) ) {
                                    $data = json_decode( $row["data"] );

                                   echo '<tr>
                                        <td>'. $data->temp. ' °C</td>
                                        <td>'. $data->hum. '%</td>
                                        <td>'. $data->w_dir. '°</td>
                                        <td>'. $data->w_int. ' m/s</td>
                                        <td>'. $data->r_heig. ' mm/h</td>
                                    </tr>';
                                } ?>
                            </tbody>
                        </table>
                    </div>
                </div>
                <div class="col">
                    <div class="alert alert-primary" role="alert">
                        <h4 class="alert-heading pt-2">Last hour</h4>
                        
                        <hr>

                        <table class="table">
                            <thead>
                                <tr>
                                    <th scope="col">Temp</th>
                                    <th scope="col">Hum</th>
                                    <th scope="col">W. Dir</th>
                                    <th scope="col">W. Int</th>
                                    <th scope="col">R. Heig</th>
                                </tr>
                            </thead>
                            <tbody>
                                <!-- Decode messages and print values -->
                                <?php while( $row = $result2->fetch_array(  ) ) {
                                    $data = json_decode( $row["data"] );

                                   echo '<tr>
                                        <td>'. $data->temp. ' °C</td>
                                        <td>'. $data->hum. '%</td>
                                        <td>'. $data->w_dir. '°</td>
                                        <td>'. $data->w_int. ' m/s</td>
                                        <td>'. $data->r_heig. ' mm/h</td>
                                    </tr>';
                                } ?>
                            </tbody>
                        </table>
                    </div>
                </div>
            </div>

            <?php mysqli_close( $link ); ?>
            
        </div>
        
        <script>
            // MQTT broker
            var HOST = "";
            var PORT = 8083;

            // Array of stations
            var stations = [];

            // Callback handlers for connection up and lost
            function onConnect() {
                // Once a connection has been made, make a subscription
                client.subscribe("sensors");
            }

            function onConnectionLost( responseObject ) {
                if ( responseObject.errorCode !== 0 ) {
                    console.log( "onConnectionLost:" + responseObject.errorMessage );
                }
            }

            // Callback for received message from MQTT broker
            function onMessageArrived( message ) {
                var sens_id;
                var mess = JSON.parse( message.payloadString );

                // Add station to array
                if( stations.indexOf( mess.id ) == -1 )
                    stations.push( mess.id )

                if( mess.id == stations[0] )
                    sens_id = "#sens0";
                else if( mess.id == stations[1] )
                    sens_id = "#sens1";

                // Use JQuery in order to inject data into HTML code
                if( sens_id ) {
                    $( sens_id + ' .temp' ).empty().append( "Temperature: <span class='font-weight-bold'>" + mess.data.temp + "</span> °C" );
                    $( sens_id + ' .hum' ).empty().append( "Humidity: <span class='font-weight-bold'>" + mess.data.hum + "%</span>" );
                    $( sens_id + ' .w_dir' ).empty().append( "Wind direction: <span class='font-weight-bold'>" + mess.data.w_dir + "°</span>" );
                    $( sens_id + ' .w_int' ).empty().append( "Wind intensity: <span class='font-weight-bold'>" + mess.data.w_int + "</span> m/s" );
                    $( sens_id + ' .r_heig' ).empty().append( "Rain height: <span class='font-weight-bold'>" + mess.data.r_heig + "</span> mm/h" );
                }
            }

            // Create a client instance with paho javascript api
            var client = new Paho.MQTT.Client( HOST, Number(PORT), "website" );

            // Set callback handlers
            client.onConnectionLost = onConnectionLost;
            client.onMessageArrived = onMessageArrived;

            // Connect the client to MQTT broker
            client.connect( { onSuccess: onConnect } );
        </script>
    </body>
</html>