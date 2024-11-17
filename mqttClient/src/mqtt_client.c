/*
*   Real-Time Embedded Systems ~ Spring 2022 ~ Final Project
*   --------------------------------------------------------
*   Author: Roozmehr Jalilian
*   Student ID: 97101467
*   --------------------------------------------------------
*   Part of: Client
*   Name: MQTT client
*/

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>

/* User definitions */
#define MYSQL_HOST_NAME "localhost"
#define MYSQL_USER_ID "rjh"
#define MYSQL_PASSWORD "1379"
#define MYSQL_DB "embproj"
#define MQTT_ADDRESS "localhost"
#define MQTT_ID "MQTT_client"
#define MQTT_USERNAME "rjh79"
#define MQTT_PASSWORD "9731"

/* Function prototypes */

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);

/* Global variables */
char word[100];
static int counter=0;

int main(int argc, char* argv[]) {
    // Initialize MQTT client
    MQTTClient client;
    MQTTClient_create(&client, MQTT_ADDRESS, MQTT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = MQTT_USERNAME;
    conn_opts.password = MQTT_PASSWORD;

    MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("MQTT failed to connect, return code %d\n", rc);
        exit(-1);
    }

    // Greet user
    printf("MQTT client\n-----------\n");
    printf("Enter any of the following numbers to subscribe to its appropriate topic:\n");
    printf("1 -> Current CPU temperature\n");
    printf("2 -> Current CPU load\n");
    printf("3 -> Last number of detected faces + time of detection\n");
    printf("4 -> Last microphone volume level + time of detection\n");
    printf("0 -> Exit program\n");

    // Subscribe to all sensory data
    MQTTClient_subscribe(client, "sensors/#", 0);

    /* Infinite loop */
    int input;
    while (1) {
        // Get user input
        printf("\nEnter the topic you'd wish to subscribe to [0~4]: ");
        scanf("%d", &input);

        // Publish request based on input, and wait for its reply
        switch (input) {
            case 0:
                MQTTClient_disconnect(client, 0);
                MQTTClient_destroy(&client);
                return rc;
                break;
            case 1:
                publish(client, "client/req/cpu/temp", "MQTT client");
                break;
            
            case 2:
                publish(client, "client/req/cpu/load", "MQTT client");
                break;

            case 3:
                publish(client, "client/req/webcam/num_faces/last", "MQTT client");
                break;
            
            case 4:
                publish(client, "client/req/mic/volume/last", "MQTT client");
                break;
            
            default:
                break;
        }
        sleep(1);
    }
 
    MQTTClient_disconnect(client, 1000);
    MQTTClient_destroy(&client);
    return rc;
}

void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    printf("-> Message '%s' with topic '%s' and delivery token %d delivered.\n", payload, topic, token);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    counter++;
    printf("[%d] Received response '%s' from topic '%s'.\n", counter, payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
