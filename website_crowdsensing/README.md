# Activity recognition web app

This directory contains all the files needed to build a really simple activity recognition web app.
The content is structured in two web pages: the first one which has to be opened from the a mobile phone and will collect data from its accelerometer, and the second one which will display the informations collected by the smarphone.
Data elaboration works through two parallel binaries: a cloud computation in which the model is computed by the AWS cloud IoT service and an edge computation, which uses a local smarphone computation and sends the resulting state.

### Prerequisites

You will need to upload the **mobile.html** file and **js** and **css** directories on an online hosting, or find a way to access them from your phone. Remember to configure the broker IP in the js script.
Once you've done this, since the broker certificate will be self signed, you will also need to guarantee a security exception to the website. 
In order to do this you can access this address:

```
https://HOSTING_ADDRESS/mobile.html:9883/mqtt
```

Once opened, the browser will prompt you and notice about possible security issues. Accept the risks and go over, from now you won't have to do it anymore.

### Mosquitto bridge and SSL certificate

The Mosquitto broker already installed is no more enough, we'll need to link it with the AWS IoT core service.
The procedure is really well explained [here](https://aws.amazon.com/it/blogs/iot/how-to-bridge-mosquitto-mqtt-broker-to-aws-iot/), just follow all the steps and the bridge will be up.  
You will also need an SSL certificate as now we'll use secure web sockets. This step can be easily done in this way (thanks to [this](https://primalcortex.wordpress.com/2016/03/31/mqtt-mosquitto-broker-with-ssltls-transport-security/) article):

1. Download [this](https://github.com/owntracks/tools/blob/master/TLS/generate-CA.sh) bash script
2. Upload the script on your EC2 virtual machine, in the **/etc/mosquitto/** directory
3. Execute the script and copy the **ca.crt**, **<YOUR_HOST>.crt** and **<YOUR_HOST>.key** files in the **certs** directory
4. Edit Mosquitto configuration **/etc/mosquitto/conf.d/default.conf** file with this part:

```
# MQTT over TLS/SSL
listener 8883
cafile /etc/mosquitto/certs/ca.crt
certfile /etc/mosquitto/certs/<YOUR_HOST>.crt
keyfile /etc/mosquitto/certs/<YOUR_HOST>.key

# WebSockets over TLS/SSL
listener 9883
protocol websockets
cafile /etc/mosquitto/certs/ca.crt
certfile /etc/mosquitto/certs/<YOUR_HOST>.crt
keyfile /etc/mosquitto/certs/<YOUR_HOST>.key
```

Your broker will now be linked to AWS IoT core services and able to receive secure websockets.

### DynamoDB tables and autmoatic rules

At this point you need to configure the DynamoDB tables which will collect the data and the automatic rules for insertion and computation.
You can do this easily:

1. Access the AWS console panel and go to the DynamoDB section
2. Create two tables, with this names: **crowd-edge** and **crowd-cloud**, put "timestamp" in the primary key field
3. Go to the IoT core section and enter in **action execution**, then create a new rule
4. The first rule will have "both_directions" as source and will simply put all the messages in the "crowd-edge" table. Hash key: **timestamp** and Hash value: **${timestamp}**
5. The second one will have "localgateway_to_awsiot" as source and will do two actions: automatic insertion in "crowd-cloud" table and Lambda function execution. Again Hash key: **timestamp** and Hash value: **${timestamp}**
6. Create the lambda rule and link it to the before mentioned one. The python code is this one:

```
from __future__ import print_function
  
import json
import boto3
  
print('Loading function')
  
def lambda_handler(event, context):
  
    # Parse the JSON message 
    eventText = json.dumps(event)
  
    # Print the parsed JSON message to the console. You can view this text in the Monitoring tab in the AWS Lambda console or in the Amazon CloudWatch Logs console.
    print('Received event: ', eventText)
    
    client = boto3.client('iot-data', region_name='us-east-1')
    
    state = ''
    
    if(event["acceleration"] <= 0.6):
        state = 'Stuck'
    elif(event["acceleration"] > 0.6 and event["acceleration"] <= 5):
        state = 'Walking'
    elif(event["acceleration"] > 5):
        state = 'Running'

    # Change topic, qos and payload
    response = client.publish(
        topic='awsiot_to_localgateway',
        qos=0,
        payload=json.dumps({"timestamp": event["timestamp"], "state": state, "acceleration": event["acceleration"]})
    )
  
    print(response)
```

The service is now ready. Do some tests and check if all it's okay.

### Website

Now you can open the website from your pc and data will be displayed. Obviously real time data will appear when phone will open the mobile web page.

## Authors

* **Claudiu Gabriel Ivan** - [LinkedIn](https://www.linkedin.com/in/claudiu-gabriel-ivan-835a33176/).

Project developed as a part of my MSc degree in Engineering in Computer Science
