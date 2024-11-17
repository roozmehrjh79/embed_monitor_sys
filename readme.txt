Real-Time Embedded Systems ~ Spring 2022 ~ Final Project
--------------------------------------------------------
--------------------------------------------------------
Author: Roozmehr Jalilain
Student ID: 97101467
--------------------------------------------------------
--------------------------------------------------------

Directory map:
--------------
** NOTE: All directories are in standard format (i.e., have CMakeLists.txt, /src/ and /build/ directories).

mqttClient/ -> Containts MQTT client files (in standard format)

mqttServer/ -> Containts MQTT server files

mysqlInitializer/ -> Containts MySQL database initializer files

Sensors/Webcam/ -> Containts webcam monitor files

Sensors/Microphone/ -> Containts microphone monitor files

WebServer/ -> Containts HTTP webserver files

'readme.txt' -> This file!

'RTEmbSys_Project_Report.pdf' -> Project report

'servicesInstaller.sh' -> Installs service files to system (requires root privileges)


Install instructions:
---------------------
1- Create a new user named 'roozmehr' (NOT incuding the quotes!)

2- Extract the CONTENTS of this zip file to:
	/home/roozmehr/Homeworks/RTEmbSys/Project/

3- Create a new MariaDB user with the following parameters (quotes are NOT included!), and grant all privileges to it:
	Username: 'rjh'
	Password: '1379'

4- Run the following and enter 'y' to configure MySQL database for first use:
	/home/roozmehr/Homeworks/RTEmbSys/Project/mysqlInitializer/build/bin/MySQL_Initializer

5- You may now restrict privileges for 'rjh' to only this database.

6- Run the following bash script to install the services:
	/home/roozmehr/Homeworks/RTEmbSys/Project/servicesInstaller.sh

7- If microphone isn't being recorded and you get a 'Capture device not found' from journalctl, refer to project report.