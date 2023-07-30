import paho.mqtt.client as mqtt
import time

# MQTT broker details
broker_address = "localhost"
broker_port = 1883


TOPIC_CMD = "simulation/cmd"
MESSAGE_START = "start"
MESSAGE_KILL = "kill"

TOPIC_SOS = "simulation/sos"
MESSAGE_SIGN = "sign"

isActive = False

# Callback function for when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)

# Callback function for when a message is received on a subscribed topic
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
            


# Create an MQTT client instance
client = mqtt.Client()

# Set the callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker
client.connect(broker_address, broker_port)

# Start the MQTT network loop in a separate thread
client.loop_start()

# Publish a message to a topic
#client.publish(TOPIC_SOS, MESSAGE_SIGN)


print("Waiting ...")

while isActive == False:
    time.sleep(1)


while isActive == True:
    time.sleep(1)

client.loop_stop()
client.disconnect()