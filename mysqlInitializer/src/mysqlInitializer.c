/*
*   Real-Time Embedded Systems ~ Spring 2022 ~ Final Project
*   --------------------------------------------------------
*   Author: Roozmehr Jalilian
*   Student ID: 97101467
*   --------------------------------------------------------
*   Part of: *None*
*   Name: MySQL database initializer
*/

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <string.h>

/* User definitions */
#define MYSQL_HOST_NAME "localhost"
#define MYSQL_USER_ID "rjh"
#define MYSQL_PASSWORD "1379"
#define MYSQL_DB "embproj"
#define STR_CAT(A, B, C) (A B C)

/* Function prototypes */

void finishWithError(MYSQL* con);

/**
 * @brief The application entry point.
 * 
 * @return int 
 */
int main() {
    // Initializations
    MYSQL *con = mysql_init(NULL);
    char input = '.';

    // Greet user & Wait for input
    printf("MySQL database initializer for Real-Time Embedded Systems final project\n");
    printf("-----------------------------------------------------------------------\n");
    printf(STR_CAT("WARNING: Running this program will erase any database with the name '", MYSQL_DB, "'!\n"));
    
    while (input != 'y' && input != 'Y' && input != 'n' && input != 'N') {
        printf("\nContinue running program [y/n]? ");
        scanf(" %c", &input);
        getchar();
    }

    // Perform database initialization if user provided 'y/Y'
    if (input == 'y' || input == 'Y') {
        // MySQL connection (no database)
        if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
        }
        if (mysql_real_connect(con, MYSQL_HOST_NAME, MYSQL_USER_ID, MYSQL_PASSWORD, NULL, 0, NULL, 0) == NULL) {
            finishWithError(con);
        }

        // Delete database
        if (mysql_query(con, STR_CAT("DROP DATABASE IF EXISTS ", MYSQL_DB, " ;"))) {
            finishWithError(con);
        }
        printf("\n-> Deleted database\n");

        // Create database & Set as default
        if (mysql_query(con, STR_CAT("CREATE DATABASE ", MYSQL_DB, " ;"))) {
            finishWithError(con);
        }
        if (mysql_query(con, STR_CAT("USE ", MYSQL_DB, " ;"))) {
            finishWithError(con);
        }
        printf("-> Created database\n");

        // Create tables
        if (mysql_query(con, "CREATE TABLE faces (id INT PRIMARY KEY AUTO_INCREMENT, count INT, time CHAR(29));")) {
            finishWithError(con);
        }
        if (mysql_query(con, "CREATE TABLE audio (id INT PRIMARY KEY AUTO_INCREMENT, volume INT, time CHAR(29));")) {
            finishWithError(con);
        }
        if (mysql_query(con, "CREATE TABLE flags (name CHAR(7) PRIMARY KEY, value BOOL);")) {
            finishWithError(con);
        }
        printf("-> Created tables\n");

        // Set flags
        if (mysql_query(con, "INSERT INTO flags VALUES('CAM_INT', 0);")) {
            finishWithError(con);
        }
        if (mysql_query(con, "INSERT INTO flags VALUES('CAM_BSY', 0);")) {
            finishWithError(con);
        }
        printf("-> Flags have been set\n");
        printf("Operation succussfull. Exiting program...\n");
    }

    return 0;
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