# MQTT IoT Service

This repository contains all the scripts and files to build up an IoT service based on AWS, Mosquitto MQTT broker, Paho libraries and MySQL DB.
The system consists in a sensor emulator, a cloud based broker, an intermediate MQTT client for DB and a website showing data. 
The code is written using Python, SQL, HTML, Javascript and PHP.  

## Getting Started

All the scripts, the MySQL DB and the website run locally so you will need a PHP server with mysqli extension enabled and an instance of MySQL installed. 
You will also need an AWS account in order to build up the cloud service.

### Prerequisites

Paho Python and PyMySQL libraries were used, so you will need to install them:

```
$ pip install paho-mqtt
$ pip install PyMySQL
```

More info:  
* [Paho](https://pypi.org/project/paho-mqtt/)  
* [PyMySQL](https://pypi.org/project/PyMySQL/)

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

At this point you are ready to run the scripts. Download the repository and follow this steps:

1. Enter in **dbprocess** folder, open **config** file and set up the credentials (You can also edit the database update frequency. Default is every 15 minutes)
2. Run **dbprocess.py**. Now the DB will be connected to the broker. Keep the script running
3. Return in the main folder and now enter **stations** one. Edit config file again and then run **sensor.py**
4. Shell commands will be printed. You can create a new stations using **start** command. 
5. Once you've created two stations, the system will be up

### Website

As was written in the prerequisites, you should have installed a program or a service which will allow you to run the index.php file.  
Before opening the webpage, edit the index file and go down to the javascript code, then edit **HOST** and **PORT** variables.  
Now you can serve the file and the website will receive data from sensors in real time and retrive last four updates from the database.

## Useful links

* [Video tutorial](https://youtu.be/4VNC8UzBAdM)
* [Hackster blog article](https://www.hackster.io/claudiuivan1/mqtt-broker-for-real-time-data-db28d4)

## Authors

* **Claudiu Gabriel Ivan** - [LinkedIn](https://www.linkedin.com/in/claudiu-gabriel-ivan-835a33176/).

Project developed as a part of my MSc degree in Engineering in Computer Science