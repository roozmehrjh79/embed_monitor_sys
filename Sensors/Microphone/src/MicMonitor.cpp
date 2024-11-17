/*
*   Real-Time Embedded Systems ~ Spring 2022 ~ Final Project
*   --------------------------------------------------------
*   Author: Roozmehr Jalilian
*   Student ID: 97101467
*   --------------------------------------------------------
*   Part of: Sensors
*   Name: Microphone monitor
*/

/* Includes */
#include <alsa/asoundlib.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <limits>

using namespace std;

/* User definitions */
#define MYSQL_HOST_NAME "localhost"
#define MYSQL_USER_ID "rjh"
#define MYSQL_PASSWORD "1379"
#define MYSQL_DB "embproj"
#define BITRATE 48000
#define BUFFER_SIZE 256
#define THRESHOLD 40

/* Function prototypes */

float getMicLevel(snd_pcm_t* handle);
void finishWithError(MYSQL* con);

/**
 * @brief The applicaiton entry point.
 * 
 * @return int 
 */
int main(void) {
    // Database connection
    MYSQL *con = mysql_init(NULL);

    if (con == NULL) {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
    }
    if (mysql_real_connect(con, MYSQL_HOST_NAME, MYSQL_USER_ID, MYSQL_PASSWORD, MYSQL_DB, 0, NULL, 0) == NULL) {
        finishWithError(con);
    }

    // Microphone cinfiguration
    snd_pcm_t* waveform;

    // Open and init a waveform
    if (snd_pcm_open(&waveform, "default", SND_PCM_STREAM_CAPTURE, 0) != 0) {
        printf("ERROR: No active audio capture device found.\n");
        return 1;
    }

    // Audio processing
    float mic_lvl;
    int volume;
    while(1) {
        // Get & Convert current sound level (in percentage)
        mic_lvl = getMicLevel(waveform);
        if (mic_lvl < std::numeric_limits<float>::epsilon()) { // error
            printf("WARNING: Audio capture device has been muted.\n");
            mysql_close(con);
            break;
        }
        volume = (int) (mic_lvl * 100);

        // If louder than threshold, write data to MySQL table
        if (volume > THRESHOLD) {
            // Prepare query statement
            ostringstream s;
            s << "INSERT INTO audio VALUES(0, '" << to_string(volume) << "', NOW());";
            string query = s.str();

            // Execute query
            if (mysql_query(con, query.c_str())) {
                finishWithError(con);
            }

            // (Optional) Print audio level in console
            printf("-> Audio level %d%% detected.\n", volume);

            // Delay
            sleep(1);
        }
        
    }

    snd_pcm_close(waveform);
    return 0;
}

/**
 * @brief Fetches microphone sound level.
 * 
 * @return float 
 */
float getMicLevel(snd_pcm_t* handle) {
    float result = 0.0f;

    if (!snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, 2, BITRATE, 1, 0)) {
        // Read current samples
        short buffer[BUFFER_SIZE];
        if (snd_pcm_readi(handle, buffer, 128) == 128) {
            // Find peak value
            float s;
            for (int i = 0; i < BUFFER_SIZE; i++) {
                s = buffer[i] / 32768.0f;
                if (s < 0) {
                    s *= -1;
                }
                if (result < s) {
                    result = s;
                }
            }
        }
    }

    return result;
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
