#!/bin/bash


echo "init can network"
sudo ip link set can0 up type can bitrate 125000


echo "Running CAN Manger"
python3 CAN_MANGER/canManger.py &


echo "Running Sign Detection"
python3 SIGN_DETECTION/sign-rp.py &
