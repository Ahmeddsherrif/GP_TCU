import paho.mqtt.client as mqtt
import cv2
import numpy as np
import time


broker_address = "192.168.1.11"
broker_port = 1883


TOPIC_CMD = "simulation/cmd"
MESSAGE_START = "start"
MESSAGE_KILL = "kill"

TOPIC_SIGN_VIDEO = "video/sign"

TOPIC_VIDEO_CONTROL = "video/control"
MESSAGE_STREAM_END = "stream_end"


data = np.empty(shape=(0,), dtype=bytes)
isActive = False


def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_SIGN_VIDEO)
    client.subscribe(TOPIC_VIDEO_CONTROL)

def on_message(client, userdata, msg):
    topic = msg.topic

    global isActive
    global data

    if topic == TOPIC_CMD:
        payload = msg.payload.decode()
        if payload == MESSAGE_START:
            print("Starting ...")
            isActive = True
            
        elif payload == MESSAGE_KILL:
            print("Ending ...")
            isActive = False

    elif topic == TOPIC_VIDEO_CONTROL:
        payload = msg.payload.decode()
        if payload == MESSAGE_STREAM_END:
            print("Ending ...")
            isActive = False

    elif topic == TOPIC_SIGN_VIDEO:
        data = msg.payload


client = mqtt.Client()


client.on_connect = on_connect
client.on_message = on_message


client.connect(broker_address, broker_port)


client.loop_start()


print("Waiting ...")

while isActive == False:
    time.sleep(1)


while isActive == True:

    if np.size(data) != 0:
        nparr = np.frombuffer(data, np.uint8)
        frame = cv2.imdecode(nparr,  cv2.IMREAD_COLOR)
        
        cv2.imshow('Received Frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

client.loop_stop()
client.disconnect()
cv2.destroyAllWindows()