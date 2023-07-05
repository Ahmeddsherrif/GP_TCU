import paho.mqtt.client as mqtt
import time
import cv2
import numpy as np
import tensorflow as tf
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

MODEL_PATH = '/home/ahmed/Desktop/GP-WorkSpace/python/tf_model_sign_new.tflite'
TRIGGER_THRESHOLD = 0.6
TRIGGER_CONSECUTIVE_FRAMES = 5


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


buffer = deque(maxlen=TRIGGER_CONSECUTIVE_FRAMES)  # Set the maximum size of the circular buffer to 5

# Load the TensorFlow Lite model
interpreter = tf.lite.Interpreter(MODEL_PATH)
interpreter.allocate_tensors()

# Get the input and output tensors
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

while isActive == True:

    if np.size(data) != 0:
        nparr = np.frombuffer(data, np.uint8)
        frame = cv2.imdecode(nparr,  cv2.IMREAD_COLOR)
        
        processed_frame=frame
        processed_frame = cv2.resize(processed_frame, (224, 224))  # Resize to input shape
        processed_frame = np.expand_dims(processed_frame, axis=0)  # Add batch dimension
        processed_frame = processed_frame.astype(np.float32)  # Convert to float32
        processed_frame = processed_frame / 255.0  # Normalize to [0, 1]
        
        # Set the input tensor
        interpreter.set_tensor(input_details[0]['index'], processed_frame)
        
        # Run the model inference
        interpreter.invoke()
        
        # Get the output tensor
        output_data = interpreter.get_tensor(output_details[0]['index'])
        
        if output_data[0][1] > TRIGGER_THRESHOLD:
            buffer.append(1)

        else:
            buffer.append(0)
        

        if sum(buffer) == TRIGGER_CONSECUTIVE_FRAMES:
            client.publish(TOPIC_SOS, MESSAGE_SIGN)
            print("SIGN_DETECTED")

        
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

client.loop_stop()
client.disconnect()
cv2.destroyAllWindows()