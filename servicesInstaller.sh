#!/bin/bash
# Real-Time Embedded Systems - Final Project - Services Installer
# Author: Roozmehr Jalilian (97101467)

cd $(pwd)
SRC="./Services/*"
DEST="/etc/systemd/system/"
echo Installing services...
for FILE in $SRC
do
	f_name="$(basename -- $FILE)"
	sudo cp "$FILE" "$DEST"
	sudo systemctl daemon-reload
	sudo systemctl enable "$f_name"
	sudo systemctl start "$f_name"
done
