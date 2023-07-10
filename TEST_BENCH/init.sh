#!/bin/bash

terminator -T "SIMULATION" -e "/home/ahmed/Desktop/TEST_BENCH/Simulation/Debug/Simulation" & 
terminator -T "VIDEO_STREAMING" -e "python3 /home/ahmed/Desktop/TEST_BENCH/Video_Streaming/vid_tx.py" & 
terminator -T "SENSORS" -e "/home/ahmed/Desktop/TEST_BENCH/Sensors/Debug/Sensors" & 
terminator -T "PSAP" -e "/home/ahmed/Desktop/TEST_BENCH/PSAP/Debug/PSAP" &

