# MQTT IoT Service

This repository contains all the scripts and files to build up an IoT service based on AWS, Mosquitto MQTT broker, Paho libraries and MySQL DB and some different types of sensors (Python local, RIOT based local, RIOT based using LoRa and TTN).
The system consists in a sensor emulator, a cloud based broker, an intermediate MQTT client for DB and a website showing data.
There is also a part dedicated to crowdsensing. More info are displayed in the readme file of the **website_crowdsensing** directory.
The code is written using Python, C, SQL, HTML, Javascript and PHP.  

## Getting Started

All the scripts, the MySQL DB and the website run locally so you will need a PHP server with mysqli extension enabled and an instance of MySQL installed. 
You will also need an AWS account in order to build up the cloud service.

### Prerequisites

Paho Python and PyMySQL libraries were used, so you will need to install them. If you choose LoRa - TTN stations you will also need the The Things Network library. Type this commands to have all up:

```
$ pip install paho-mqtt
$ pip install PyMySQL
$ pip install ttn
```

More info:  
* [Paho](https://pypi.org/project/paho-mqtt/)  
* [PyMySQL](https://pypi.org/project/PyMySQL/)
* [TTN](https://pypi.org/project/ttn/)

Website uses PHP functions to interact with the database, so you will need a local server. I recommend you to install Visual Studio Code and its PHP Server plugin.

### MQTT Broker

1. Access the AWS console and create a new EC2 machine. To do this go through **EC2** > **Running Instances** > **Launch Instance**
2. Select the **Ubuntu Server 18.04 LTS** type, choose the correct IAM role and go ahead untill the security group configuration 
3. Here you will need to add two new rules for port **1883** and **8083** with source ip set to 0.0.0.0/0
4. Launch the machine and wait untill it is fully running
5. Now open the terminal and connect to the machine
6. Install Mosquitto MQTT broker service:

```
$ sudo apt-get update  
$ sudo apt-get install mosquitto mosquitto-clients
```

7. Modify the configuration and add the two ports with the different protocols, then restart the service:

```
$ sudo nano /etc/mosquitto/conf.d/default.conf
```

Add this four lines and save the file:

```
listener 1883
protocol mqtt
listener 8083
protocol websockets
```

Then restart:

```
$ sudo systemctl restart mosquitto
```

Now your MQTT broker is up and running. Before leaving take note of the server IP address.

### MySQL Table

Before start running the sensors, you will need to create a new table in your local database. In order to do this you can use the mysql shell.  
Create a new table through the following query:

```
CREATE TABLE sensors ( id INT, data VARCHAR(100), time_id INT AUTO_INCREMENT PRIMARY KEY ) 
```

The database is now ready. Take note of the credentials for the access. 

### Start scripts

At this point the scripts are ready to be run. You can choose between three options:
* Simple Python MQTT stations
* RIOT MQTT-SN stations (you will need a gateway)
* RIOT LoRa - TTN stations (you will need a The Things Network account)

Once you've decided, download the repository and follow this steps:

#### Simple Python MQTT stations

1. Enter in **dbprocess** folder, open **config** file and set up the credentials (You can also edit the database update frequency. Default is every 15 minutes)
2. Run **dbprocess.py**. Now the DB will be connected to the broker. Keep the script running
3. Return in the main folder and now enter **stations** one. Edit config file again and then run **sensor.py**
4. Shell commands will be printed. You can create a new stations using **start** command. 
5. Once you've created two stations, the system will be up

See more on my Hackster article [here](https://www.hackster.io/claudiuivan1/1-iot-mqtt-system-broker-db-and-python-stations-db28d4)

#### RIOT MQTT-SN stations

1. Setup Paho MQTT-SN transparent gateway ([See more here](https://www.eclipse.org/paho/components/mqtt-sn-transparent-gateway/)) followiing the guide provided in the readme file of this [repository](https://github.com/eclipse/paho.mqtt-sn.embedded-c/tree/master/MQTTSNGateway).  
This address and port can be used as an example configuration:
```
fec0:affe::1 1885
```
2. Configure the gateway with an IPv6 address for MQTT-SN connection and the IPv4 address of the cloud MQTT broker.
3. Start the gateway
4. Enter in **dbprocess** folder, open **config** file and set up the credentials (You can also edit the database update frequency. Default is every 15 minutes)
5. Run **dbprocess.py**. Now the DB will be connected to the broker. Keep the script running
6. Enter **riot_station** folder, setup Makefile with currect RIOT path and compile
7. Assign a site-global address using the same prefix of gateway one, with this command:
```
ifconfig 5 add fec0:affe::99
```
to RIOT process and connect to gateway using **start** command 
8. Once you've created two stations with two differents terminal instances, the system will be up

See more on my Hackster article [here](https://www.hackster.io/claudiuivan1/2-iot-mqtt-system-riot-stations-0b53f4)

#### RIOT LoRa - TTN stations

1. Create [IoT-LAB](https://www.iot-lab.info/) and [The Things Network](https://www.thethingsnetwork.org/) accounts and setup them
2. Enter **riot_lora_station/transparent_bridge** folder, edit the **config** file with your TTN and cloud broker infos
3. Run **transparent_bridge.py**. The bridge will be connected to TTn and to your broker, keep it running
4. Enter **riot_lora_station/station** folder, setup Makefile with currect RIOT path and compile
5. Copy the elf file on your IoT-LAB home, using
```
$ scp bin/b-l072z-lrwan1/riot_lora.elf <USERNAME>@saclay.iot-lab.info:riot_lora.elf
```
6. Login in IoT through SSH, create a new experiment and flash the elf file
```
$ ssh <USERNAME>@saclay.iot-lab.info
$ iotlab-auth -u <USERNAME>
$ iotlab-experiment submit -n <NAME-OF-EXPERIMENT> -d 60 -l 2,archi=st-lrwan1:sx1276+site=saclay 
$ iotlab-experiment get -i <EXPERIMENT-ID> -s
$ iotlab-experiment get -i <EXPERIMENT-ID> -r
$ iotlab-node --update riot_lora.elf -l saclay,st-lrwan1,<DEVICE-ID>
```
7. Access the app shell, connect to TTN and start the station
```
$ nc st-lrwan1-<DEVICE-ID> 20000
```
```
> loramac set deveui <DEVICE-EUI>
> loramac set appeui <APP EUI>
> loramac set appkey <APP-KEY>
> loramac set dr 5
> loramac join otaa
> start <STATION-ID>
```
The commands has to be executed one by one. Check the below mentioned article for more detailed informations.

See more on my Hackster article [here](https://www.hackster.io/claudiuivan1/3-iot-mqtt-system-lora-and-ttn-61c4f2)

### Website

As was written in the prerequisites, you should have installed a program or a service which will allow you to run the index.php file.  
Before opening the webpage, edit the index file and go down to the javascript code, then edit **HOST** and **PORT** variables.  
Now you can serve the file and the website will receive data from sensors in real time and retrive last four updates from the database.

### Activity recognition

The MQTT Mosquitto broker can also be used to set up a really simple activity recognition web app service. All the informations about this part, can be found [here](https://github.com/Claudiuivan1/IoT-Project/tree/master/website_crowdsensing).

## Useful links

### Part 1 - Python MQTT stations

* [Video tutorial](https://youtu.be/4VNC8UzBAdM)
* [Hackster blog article](https://www.hackster.io/claudiuivan1/mqtt-broker-for-real-time-data-db28d4)

### Part 2 - RIOT MQTT-SN stations

* [Video tutorial](https://youtu.be/xlfrrRODYE4)
* [Hackster blog article](https://www.hackster.io/claudiuivan1/mqtt-broker-for-real-time-data-part-2-0b53f4)

### Part 3 - RIOT LoRa TTN stations

* [Video tutorial](https://www.youtube.com/watch?v=nv5WgZYVP1k)
* [Hackster blog article](https://www.hackster.io/claudiuivan1/3-iot-mqtt-system-lorawan-and-ttn-61c4f2)

### Part 4 - Activity recognition and cloud analysis

* [Video tutorial](https://youtu.be/uvwUmiBE9js)
* [Hackster blog article](https://www.hackster.io/claudiuivan1/4-iot-mqtt-system-crowdsensing-and-cloud-analysis-323ff6)

## Authors

* **Claudiu Gabriel Ivan** - [LinkedIn](https://www.linkedin.com/in/claudiu-gabriel-ivan-835a33176/).

Project developed as a part of my MSc degree in Engineering in Computer Science
