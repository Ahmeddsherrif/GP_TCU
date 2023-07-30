#!/bin/bash


echo "running ADR"
/home/user/Desktop/TELEMATICS_CONTROL_UNIT/ADR/Debug/ADR &

echo "running ecall"
/home/user/Desktop/TELEMATICS_CONTROL_UNIT/ECALL/Debug/ECALL &

echo "Running black_box"
python3 /home/user/Desktop/TELEMATICS_CONTROL_UNIT/BLACK_BOX/black_box.py &
