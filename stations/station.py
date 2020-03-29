from threading import Thread
import random, time
import paho.mqtt.client as mqtt
import config


# Station class defines simulated sensor entities
# The class is defined as a thread instance 
# Paho module is used for MQTT connection
class Station(Thread):
	
	def __init__( self, name, idx ):
		
		Thread.__init__( self )
		
		self.mqttc = mqtt.Client()
		
		self.idx = idx
		self.name = name
		self.values = {}
		self.running = False
		
	# When thread starts a connection through MQTT broker is performed
	# Then it keeps sending new values to the cloud server 
	def run( self ):
		if( not self.running ):
			self.connect( config.MQTT_IP, config.MQTT_PORT )
			self.running = True
		self.send_values()
		
	# Sets the flag to false and sending process will stop
	def stop( self ):
		self.running = False
	
	# Generates new random values for all the five sensors
	def new_values( self ):
		self.values["temp"] = random.randint( -5000, 5000 ) / 100
		self.values["hum"] = random.randint( 0, 10000 ) / 100
		self.values["w_dir"] = random.randint( 0, 36000 ) / 100
		self.values["w_int"] = random.randint( 0, 10000 ) / 100
		self.values["r_heig"] = random.randint( 0, 5000 ) / 100
		
	# Debug function 
	def print_values( self, msg ):
		print( "Message sent: " + msg )

	# Connection function for MQTT client through paho
	def connect( self, host, port ):
		self.mqttc.connect( host, port )
		
	# Generate new values, create JSON string, publish it and sleep
	def send_values( self ):
		while( self.running ):
			self.new_values()
			msg = self.create_msg()
			self.mqttc.publish( "sensors", msg )
			time.sleep(3)
			
	# Generate proper JSON string to send through MQTT
	def create_msg( self ):
		msg = '{ "id": "' + str(self.idx) + '", '
		msg += '"data": { '
		for key in self.values:
			msg += '"' + key + '": "' + str( self.values[key] ) + '", '
		
		k = msg.rfind(', ')
		msg = msg[:k] + msg[k+1:]	
		
		msg += '} '
		msg += '}'
		return msg
