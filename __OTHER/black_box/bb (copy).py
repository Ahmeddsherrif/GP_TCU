import paho.mqtt.client as mqtt
import cv2
import numpy as np
import time
from collections import deque


broker_address = "localhost"
broker_port = 1883


TOPIC_CMD = "simulation/cmd"
MESSAGE_START = "start"
MESSAGE_KILL = "kill"

TOPIC_SIGN_VIDEO = "video/sign"

TOPIC_VIDEO_CONTROL = "video/control"
MESSAGE_STREAM_END = "stream_end"

TOPIC_SOS = "sos"
MESSAGE_SIGN = "sign"

WIDTH = 640
HEIGHT = 480
FRAME_RATE = 30

RECORD_TIME_SECONDS = 2


frame_buffer = deque(maxlen=FRAME_RATE * RECORD_TIME_SECONDS)


data = np.empty(shape=(0,), dtype=bytes)
isActive = False
isTriggerOccured = False

def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_SIGN_VIDEO)
    client.subscribe(TOPIC_VIDEO_CONTROL)
    client.subscribe(TOPIC_SOS)

def on_message(client, userdata, msg):
    topic = msg.topic

    global data
    global isActive
    global isTriggerOccured
    

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

    elif topic == TOPIC_SOS:
        payload = msg.payload.decode()
        if payload == MESSAGE_SIGN:
            isActive = False
            isTriggerOccured = True
            print("Sign Detected!")

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
        
        frame_buffer.append(frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break



if isTriggerOccured == True:
    print("Saving Video ... ")

    current_time = time.strftime('%Y-%m-%d_%H-%M-%S')
    output_video_name = "BB_" + current_time + ".mp4"
    output_codec = cv2.VideoWriter_fourcc(*'mp4v')
    output_writer = cv2.VideoWriter(output_video_name, output_codec, FRAME_RATE, (WIDTH, HEIGHT))

    # Write frames from the deque to the output video
    for frame in frame_buffer:
        output_writer.write(frame)

    # Release the output video writer
    output_writer.release()
    print("Done Saving!")

client.loop_stop()
client.disconnect()
cv2.destroyAllWindows()