import time
import cv2
import paho.mqtt.client as mqtt

broker_address = localhost #"192.168.1.11"
broker_port = 1883


TOPIC_CMD = "simulation/cmd"
MESSAGE_START = "start"
MESSAGE_KILL = "kill"

TOPIC_SIGN_VIDEO = "video/sign"

TOPIC_VIDEO_CONTROL = "video/control"
MESSAGE_STREAM_END = "stream_end"

WIDTH = 640
HEIGHT = 480
FRAME_RATE = 30


VIDEO_PATH = '/home/ahmed/Desktop/GP-WorkSpace/python/WIN_20230306_12_23_29_Pro.mp4'


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

cap = cv2.VideoCapture(VIDEO_PATH)

if not cap.isOpened():
    print("Error opening video file")
    exit()



frame_delay = 1.0 / FRAME_RATE

while isActive == True:
    start_time = time.time()

    ret, frame = cap.read()

    if not ret:
        client.publish(TOPIC_VIDEO_CONTROL, MESSAGE_STREAM_END)
        time.sleep(1)
        break

    frame = cv2.resize(frame, (WIDTH ,HEIGHT))  
    frame_str = cv2.imencode('.jpg', frame)[1].tobytes()

    client.publish(TOPIC_SIGN_VIDEO, frame_str)

    # Break the loop if the 'q' key is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

    elapsed_time = time.time() - start_time
    time.sleep(max(0, frame_delay - elapsed_time))


client.loop_stop()
client.disconnect()
