#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell.h"
#include "msg.h"
#include "net/emcute.h"
#include "net/ipv6/addr.h"

#include <time.h>
#include "xtimer.h"

#ifndef EMCUTE_ID
#define EMCUTE_ID           ("gertrud")
#endif
#define EMCUTE_PORT         (1883U)
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)

#define NUMOFSUBS           (16U)
#define TOPIC_MAXLEN        (64U)

static char stack[THREAD_STACKSIZE_DEFAULT];
static msg_t queue[8];

float values[5];	// Random values array
char msg[100];		// Payoff buffer

// Random value generator in specific range
float random_float( float a, float b ) {
    float random = ( ( float ) rand() ) / ( float ) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

// Generate proper JSON string to send through MQTT
static char* create_msg( void ) {
		
	strcat( msg, "{ \"id\": \"" );
	strcat( msg, EMCUTE_ID );
	strcat( msg, "\", " );
	strcat( msg, "\"data\": { " );
	
	char str[7];
	strcat( msg, "\"temp\": \"" );
	snprintf(str, sizeof str, "%02.02f", values[0]);
	strcat( msg, str );
	strcat( msg, "\", " );
	
	memset( str, 0, sizeof( str ) );
	
	snprintf(str, sizeof str, "%02.02f", values[1]);
	strcat( msg, "\"hum\": \"" );
	strcat( msg, str );
	strcat( msg, "\", " );
	
	memset( str, 0, sizeof( str ) );
	
	snprintf(str, sizeof str, "%02.02f", values[2]);
	strcat( msg, "\"w_dir\": \"" );
	strcat( msg, str );
	strcat( msg, "\", " );
	
	memset( str, 0, sizeof( str ) );
	
	snprintf(str, sizeof str, "%02.02f", values[3]);
	strcat( msg, "\"w_int\": \"" );
	strcat( msg, str );
	strcat( msg, "\", " );
	
	memset( str, 0, sizeof( str ) );
	
	snprintf(str, sizeof str, "%02.02f", values[4]);
	strcat( msg, "\"r_heig\": \"" );
	strcat( msg, str );
	strcat( msg, "\" } }" );
	
	return msg;
}

// Function which generates new random values for the station
void sens(void) {

	values[0] = random_float(-50.00, 50.00);	// temp
	values[1] = random_float( 0, 100.00 ); 		// hum
	values[2] = random_float( 0, 360.00 ); 		// w_dir
	values[3] = random_float( 0, 100.00 ); 		// w_int
	values[4] = random_float( 0, 50.00 ); 		// r_heig

}

static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    
    
    return NULL;    /* should never be reached */
}

static unsigned get_qos(const char *str)
{
    int qos = atoi(str);
    switch (qos) {
        case 1:     return EMCUTE_QOS_1;
        case 2:     return EMCUTE_QOS_2;
        default:    return EMCUTE_QOS_0;
    }
}

// Connection function
static int cmd_con(int argc, char **argv)
{
    sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT };
    char *topic = NULL;
    char *message = NULL;
    size_t len = 0;

    if (argc < 2) {
        printf("usage: %s <ipv6 addr> [port] [<will topic> <will message>]\n",
                argv[0]);
        return 1;
    }

    /* parse address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, argv[1]) == NULL) {
        printf("error parsing IPv6 address\n");
        return 1;
    }

    if (argc >= 3) {
        gw.port = atoi(argv[2]);
    }
    if (argc >= 5) {
        topic = argv[3];
        message = argv[4];
        len = strlen(message);
    }

    if (emcute_con(&gw, true, topic, message, len, 0) != EMCUTE_OK) {
        printf("error: unable to connect to [%s]:%i\n", argv[1], (int)gw.port);
        return 1;
    }
    printf("Successfully connected to gateway at [%s]:%i\n",
           argv[1], (int)gw.port);

    return 0;
}

// Disconnection function
static int cmd_discon(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = emcute_discon();
    if (res == EMCUTE_NOGW) {
        puts("error: not connected to any broker");
        return 1;
    }
    else if (res != EMCUTE_OK) {
        puts("error: unable to disconnect");
        return 1;
    }
    puts("Disconnect successful");
    return 0;
}

// Start sending data to broker
int cmd_start(int argc, char** argv) {
    
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;

    /* parse QoS level */
    if (argc >= 2) {
        flags |= get_qos(argv[3]);
    }

    printf("pub on topic: 'sensors' and flags 0x%02x\n", (int)flags);

    /* step 1: get topic id */
    t.name = "sensors";
    if (emcute_reg(&t) != EMCUTE_OK) {
        puts("error: unable to obtain topic ID");
        return 1;
    }

	/* step 2: publish data */
    while(1) {
		
		// Generate new random values
		sens();
		
		// Create message
		char* msg = create_msg();
		
		// Publish on topic
		if (emcute_pub( &t, msg, strlen( msg ), flags ) != EMCUTE_OK) {
			printf("error: unable to publish data to topic '%s [%i]'\n",
					t.name, (int)t.id);
			return 1;
		}

		printf("Published %i bytes to topic '%s [%i]'\n",
				(int)strlen( msg ), t.name, t.id);
				
		memset( msg, 0, 100 );
		xtimer_sleep(3);
	}

    return 0;
    
}


static const shell_command_t shell_commands[] = {
    { "con", "connect to MQTT broker", cmd_con },
    { "discon", "disconnect from the current broker", cmd_discon },
    { "start","publish values to broker", cmd_start},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MQTT-SN RIOT station\n");
    puts("Type 'help' to get started.");

    /* the main thread needs a msg queue to be able to run `ping6`*/
    msg_init_queue(queue, ARRAY_SIZE(queue));

    /* start the emcute thread */
    thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0,
                  emcute_thread, NULL, "emcute");

    /* start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
