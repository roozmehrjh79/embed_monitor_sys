/*
*   Real-Time Embedded Systems ~ Spring 2022 ~ Final Project
*   --------------------------------------------------------
*   Author: Roozmehr Jalilian
*   Student ID: 97101467
*   --------------------------------------------------------
*   Part of: Sensors
*   Name: Webcam monitor
*/

/* Includes */
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <cstring>
#include <stdio.h>
#include <ctime>
#include <sstream>
#include <unistd.h>
#include <mysql/mysql.h>
#include <pthread.h>

using namespace cv;
using namespace std;

/* User definitions */
#define MYSQL_HOST_NAME "localhost"
#define MYSQL_USER_ID "rjh"
#define MYSQL_PASSWORD "1379"
#define MYSQL_DB "embproj"
#define HAAR_CASCADE_DIR "/home/roozmehr/Documents/OpenCV/opencv/data/haarcascades/haarcascade_frontalcatface.xml"
#define CAPTURE_SAVE_DIR "/home/roozmehr/Homeworks/RTEmbSys/Project/WebServer/build/bin/capture/webcam_capture.jpg"

/* Function prototypes */

void* countFaces(void* arg);
void* saveImage(void* arg);
void finishWithError(MYSQL* con);

/* Global variables */
MYSQL *con = mysql_init(NULL);  // MySQL connection
MYSQL_RES *res;  // MySQL response
MYSQL_ROW row;  // MySQL row
CascadeClassifier face_cascade;  // Cascade classifier for OpenCV face detection
cv::VideoCapture camera(0);  // Webcam video capture
pthread_mutex_t cam_mutex;  // Mutex for locking the camera
int num_faces_old = 0;
int num_faces_new = 0;

/**
 * @brief The application entry point.
 * 
 * @return int 
 */
int main( )
{
    // Setup threads
    pthread_t threads[2];

    // Setup MySQL connection
    if (con == NULL) {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
    }
    if (mysql_real_connect(con, MYSQL_HOST_NAME, MYSQL_USER_ID, MYSQL_PASSWORD, MYSQL_DB, 0, NULL, 0) == NULL) {
        finishWithError(con);
    }

    // Setting up the webcam
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera.\n" << std::endl;
        return 1;
    }

    // Setting up face cascade
    if( !face_cascade.load(HAAR_CASCADE_DIR) )
    {
        cout << "ERROR: Problem loading face cascade.\n";
        return -1;
    };
	
    /* Main program */
    while (1) {
        // Create threads & Initialize mutex lock
        pthread_mutex_init(&cam_mutex, NULL);
        pthread_create(&threads[0], NULL, countFaces, (void*) NULL);
        pthread_create(&threads[1], NULL, saveImage, (void*) NULL);

        // Join threads & Destroy mutex lock
        for (int i = 0; i < 2; i++) {
            pthread_join(threads[i], NULL);
        }
        pthread_mutex_destroy(&cam_mutex);
    }

    return 0;
}

/**
 * @brief Counts the number of faces in a webcam capture, and writes in database if values are changed.
 * 
 * @param arg Input argument (dummy)
 * @return void* 
 */
void* countFaces(void* arg) {
    // Capture frame
    Mat frame;
    pthread_mutex_lock(&cam_mutex);
    camera >> frame;
    pthread_mutex_unlock(&cam_mutex);

    // Find faces & count them
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);
    std::vector<Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces);
    num_faces_new = faces.size();

    // Check if number of faces has changed
    if (num_faces_old != num_faces_new) {
        // Prepare query statement
        ostringstream s;
        s << "INSERT INTO faces VALUES(0, '" << to_string(num_faces_new) << "', NOW());";
        string query = s.str();

        // Execute query
        if (mysql_query(con, query.c_str())) {
            finishWithError(con);
        }

        // (Optional) Print number of faces found
        printf("-> Number of faces changed from %d to %d.\n", num_faces_old , num_faces_new);

        // Update current face count
        num_faces_old = num_faces_new;
    }

    // Goto sleep
    sleep(1);

    return NULL;
}

/**
 * @brief Saves a frame captured from webcam.
 * 
 * @param arg Input argument (dummy)
 * @return void* 
 */
void* saveImage(void* arg) {
    // Check if we have interrupt
    if (mysql_query(con, "SELECT value FROM flags WHERE name = 'CAM_INT';")) {
        finishWithError(con);
    }
    res = mysql_use_result(con);
    row = mysql_fetch_row(res);
    if (row[0][0] != '1') {
        mysql_free_result(res);
        return NULL;
    }
    mysql_free_result(res);

    // If we have an interrupt, set BUSY flag to 1 until job is done
    if (mysql_query(con, "UPDATE flags SET value = 1 WHERE name = 'CAM_BSY';")) {
        finishWithError(con);
    }

    // Capture frame
    Mat frame;
    pthread_mutex_lock(&cam_mutex);
    camera >> frame;
    pthread_mutex_unlock(&cam_mutex);

    // Save it
    cv::imwrite(CAPTURE_SAVE_DIR, frame);

    // (Optional) Print success of the operation
    cout << "(!)-> Generated webcam capture at '" << CAPTURE_SAVE_DIR << "'." << endl;

    // Reset interrupt & busy flag to 0
    if (mysql_query(con, "UPDATE flags SET value = 0;")) {
        finishWithError(con);
    }

    return NULL;
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
