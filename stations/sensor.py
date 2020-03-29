import station

# Global variables for started threads
THREADS = {}
N_THREADS = 0

# Start new thread handler
def start_fn( cmd ):
	global THREADS
	global N_THREADS
	
	# Verify command correctness
	cmd = cmd.split(" ")
	if( len(cmd) != 2 or cmd[0] != "start" ):
		print( "  Usage: start <station-name>\n" )
		return False
	if( N_THREADS == 2 ):
		print( "  Two stations already running:" )
		i = 0
		for key in THREADS:
			print( "  STATION " + str( i ) + " -> '" + key + "'" )
			i += 1
		return False
	if( cmd[1] in THREADS ):
		print( "  A stations with the same name is already running\n" )
		return False
		
	# Start new station thread, update global variables, print feedback
	thread = station.Station( cmd[1], N_THREADS + 1 )
	THREADS[cmd[1]] = thread
	N_THREADS += 1
	thread.start()
	print( "  Station '" + cmd[1] + "' started" )
	return True
	
	
# Stop thread handler
def stop_fn( cmd ):
	global THREADS
	global N_THREADS
	
	# Verify command correctness
	cmd = cmd.split(" ")
	if( len(cmd) != 2 or cmd[0] != "stop" ):
		print( "  Usage: stop <station-name>\n" )
		return False
	if( N_THREADS == 0 ):
		print( "  There are no running stations.\n" )
		return False
	if( not ( cmd[1] in THREADS ) ):
		print( "  There is no running station with this name.\n" )
		return False
		
	# Stop selected station thread, update global variables, print feedback
	print( "  Stopping sensor..." )
	THREADS[cmd[1]].stop()
	THREADS[cmd[1]].join()
	del THREADS[cmd[1]]
	N_THREADS -= 1
	print( "  Station '" + cmd[1] + "' stopped" )
	return True
	
# Status handler
def print_status():
	global THREADS
	global N_THREADS
	
	if( N_THREADS > 0 ):
		print( "  Running stations:" )
		i = 1
		for key in THREADS:
			print( "  STATION " + str( i ) + " -> '" + key + "'" )
			i += 1
	else:
		print( "  No running stations" )
	

def main():
	
	# Print list of commands
	print( "\n  SENSORS CONSOLE\n\n" + 
			"  Commands list:\n\n" + 
			"  start <station-name> -> Start new station\n" + 
			"  stop <station-name> -> Stop station\n" +
			"  status -> Print current threads status\n" +
			"  exit -> Close shell\n" )
			
	# Initialize handlers array
	CMDS = { "start": start_fn, "stop": stop_fn }
			
	# Start shell
	while( True ):
		cmd = input("$ ")
		cmd_b = cmd.split(" ")
		if( cmd_b[0] in CMDS ):
			CMDS[cmd_b[0]](cmd)
		if( cmd == "status" ):
			print_status()
		if( cmd == "exit" ):
			# When closing shell, stop all the stations
			for i in range( 0, N_THREADS ):
				stop_fn( "stop " + str( next( iter( THREADS ) ) ) )
			break
			

if __name__== "__main__":
	main()
