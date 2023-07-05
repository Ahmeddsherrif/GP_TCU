import time
import cv2
import numpy as np
import paho.mqtt.client as mqtt

broker_address = "localhost"
broker_port = 1883


TOPIC_CMD = "simulation/cmd"
MESSAGE_START = "start"
MESSAGE_KILL = "kill"

TOPIC_VIDEO = "video"


video_path = '/home/ahmed/Desktop/GP-WorkSpace/python/WIN_20230306_12_23_29_Pro.mp4'


isActive = False

def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()

    global isActive

    if topic == TOPIC_CMD:
        if payload == MESSAGE_START:
            print("Starting ...")
            isActive = True
            
        elif payload == MESSAGE_KILL:
            print("Ending ...")
            isActive = False
            


client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message

client.connect(broker_address, broker_port)

client.loop_start()

print("Waiting ...")

while isActive == False:
    time.sleep(1)

cap = cv2.VideoCapture(video_path)

if not cap.isOpened():
    print("Error opening video file")
    exit()

time.sleep(3)

while isActive == True:
    ret, frame = cap.read()

    if not ret:
        break

    frame = cv2.resize(frame, (640 ,480))  
    frame_str = cv2.imencode('.jpg', frame)[1].tobytes()

    client.publish(TOPIC_VIDEO, frame_str)

client.loop_stop()
client.disconnect()