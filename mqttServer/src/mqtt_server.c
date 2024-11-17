/*
*   Real-Time Embedded Systems ~ Spring 2022 ~ Final Project
*   --------------------------------------------------------
*   Author: Roozmehr Jalilian
*   Student ID: 97101467
*   --------------------------------------------------------
*   Part of: Server
*   Name: MQTT server
*/

/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <MQTTClient.h>
#include <mysql/mysql.h>

/* User definitions */
#define MYSQL_HOST_NAME "localhost"
#define MYSQL_USER_ID "rjh"
#define MYSQL_PASSWORD "1379"
#define MYSQL_DB "embproj"
#define MQTT_ADDRESS "localhost"
#define MQTT_ID "MQTT_server"
#define MQTT_USERNAME "rjh79"
#define MQTT_PASSWORD "9731"
#define CPU_TEMP_DIR "/sys/class/thermal/thermal_zone0/temp"
#define CPU_LOAD_DIR "/proc/loadavg"

/* Function prototypes */

void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void finishWithError(MYSQL* con);

/* Global variables */
static int request = -2;
static int counter = 0;

/**
 * @brief The application entry point.
 * 
 * @param argc Number of arguments
 * @param argv Arguments
 * @return int 
 */
int main(int argc, char* argv[]) {
    // Greet user
    printf("MQTT server\n-----------\n");

    // Initialize file handling
    FILE* fptr = NULL;
    char* line = NULL;
    char word[100];
    size_t len = 0;
    ssize_t read;

    // Database connection
    MYSQL *con = mysql_init(NULL);
    MYSQL_RES *res;  // MySQL response
    MYSQL_ROW row;  // MySQL row

    if (con == NULL) {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
    }
    if (mysql_real_connect(con, MYSQL_HOST_NAME, MYSQL_USER_ID, MYSQL_PASSWORD, MYSQL_DB, 0, NULL, 0) == NULL) {
        finishWithError(con);
    }

    // MQTT initialization
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

    // Subscribe to client requests
    MQTTClient_subscribe(client, "client/req/#", 0);

    /* Infinite loop */
    while (1) {
        // Send appropriate data based on received request
        switch (request) {
            case -1: // bad request
                printf("WARNING: Last received request was undefined.\n");
                request = -2;
                break;

            case 0:  // cpu temp
                int temp;
                fptr = fopen(CPU_TEMP_DIR, "r");
                read = getline(&line, &len, fptr);
                temp = atoi(line) / 1000;
                snprintf(word, 6, "%d^C", temp);
                publish(client, "sensors/cpu/temp", word);
                fclose(fptr);
                request = -2;
                break;

            case 1:  // cpu load
                char* tmp;
                fptr = fopen(CPU_LOAD_DIR, "r");
                read = getline(&line, &len, fptr);
                tmp = strtok(line, " ");
                publish(client, "sensors/cpu/load", tmp);
                fclose(fptr);
                request = -2;
                break;

            case 2:  // number of faces (last)
                if (mysql_query(con, "SELECT count, time FROM faces WHERE id=(SELECT MAX(id) FROM faces);")) {
                    finishWithError(con);
                }
                res = mysql_use_result(con);
                row = mysql_fetch_row(res);
                if (row != NULL) {
                    strcpy(word, "# of faces: ");
                    strcat(word, row[0]);
                    strcat(word, " @ ");
                    strcat(word, row[1]);
                    publish(client, "sensors/webcam/num_faces/last", word);
                } else {
                    publish(client, "sensors/webcam/num_faces/last", "*NO DATA AVAILABLE*");
                }
                mysql_free_result(res);
                request = -2;
                break;

            case 3:  // mic level (last)
                if (mysql_query(con, "SELECT volume, time FROM audio WHERE id=(SELECT MAX(id) FROM audio);")) {
                    finishWithError(con);
                }
                res = mysql_use_result(con);
                row = mysql_fetch_row(res);
                if (row != NULL) {
                    strcpy(word, "Microphone volume: ");
                    strcat(word, row[0]);
                    strcat(word, "% @ ");
                    strcat(word, row[1]);
                    publish(client, "sensors/mic/volume/last", word);
                } else {
                    publish(client, "sensors/mic/volume/last", "*NO DATA AVAILABLE*");
                }
                mysql_free_result(res);
                request = -2;
                break;
            default:  // nothing
                request = -2;
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
    printf("-> Message '%s' witch topic '%s' and delivery token %d delivered.\n", payload, topic, token);
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    counter++;

    // Check incoming topic name
    if (!strcmp(topicName, "client/req/cpu/temp")) {
        request = 0;
    } else if (!strcmp(topicName, "client/req/cpu/load")) {
        request = 1;
    } else if (!strcmp(topicName, "client/req/webcam/num_faces/last")) {
        request = 2;
    } else if (!strcmp(topicName, "client/req/mic/volume/last")) {
        request = 3;
    } else {
        request = -1;
    }

    // Acknowledge & End
    printf("[%d] Received request '%s' from topic name '%s'.\n", counter, payload, topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/**
 * @brief Finishes program due to a potential MySQL error.
 * 
 * @param con MySQL conneciton
 */
void finishWithError(MYSQL* con) {
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);
}
