import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import config

import time
import ttn

# Everytime a message is received from TTN broker, send it to the second
def uplink_callback(msg, client):
	  print("Received uplink from ", msg.dev_id)
	  publish.single( "sensors", 
						msg.payload_fields.string, 
						hostname=config.MQTT_CLOUD_IP, 
						port=config.MQTT_CLOUD_PORT)
	  print(msg.payload_fields.string)

def main():
	
	print( "Transparent bridge starting..." )
	
	# Configuring TTN broker
	app_id = config.MQTT_TTN_USER
	access_key = config.MQTT_TTN_PASS
	
	handler = ttn.HandlerClient(app_id, access_key)
	mqtt_client = handler.data()
	
	# Setting callback
	mqtt_client.set_uplink_callback(uplink_callback)
	# Connect
	mqtt_client.connect()
	
	print( "Connection completed" )
	print( "Waiting messages from broker..." )
	
	# Sleep forever
	while(True):
		time.sleep(60)
	
	mqtt_client.close()

		
			
if __name__== "__main__":
	main()
