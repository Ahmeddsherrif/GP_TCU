import paho.mqtt.client as mqtt
import time
import can

# MQTT broker details
broker_address = "192.168.1.21"
broker_port = 1883


TOPIC_CMD     = "simulation/cmd"
MESSAGE_START = "start"
MESSAGE_KILL  = "kill"

TOPIC_SOS     = "sos"
MESSAGE_ECALL = "ecall"
MESSAGE_BCALL = "bcall"
MESSAGE_END   = "end"

TOPIC_SPEEDOMETER = "sensors/speedometer"




TOPIC_DMS           = "dms"
MESSAGE_SLEEP       = "sleep"
MESSAGE_DISTRACTED  = "distracted"

CAN_MSG_SIMULATION_CMD_ID		    =   0x09
CAN_MSG_SIMULATION_CMD_DATA_KILL    =   0x01
CAN_MSG_SIMULATION_CMD_DATA_START   =   0x02

CAN_MSG_SOS_ID					    =   0x10
CAN_MSG_SOS_DATA_ECALL			    =   0x01
CAN_MSG_SOS_DATA_BCALL			    =   0x02
CAN_MSG_SOS_DATA_REQUEST_BCALL	    =   0x03
CAN_MSG_SOS_DATA_END			    =   0x04

CAN_MSG_SPEED_ID				    =   0x12

CAN_MSG_DMS_ID						=   0x11
CAN_MSG_DMS_DATA_DISTRACTED			=   0x01
CAN_MSG_DMS_DATA_SLEEP				=   0x02

bus = can.interface.Bus(channel='can0', bustype='socketcan')
isActive = False

# Callback function for when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_SOS)
    client.subscribe(TOPIC_DMS)
    client.subscribe(TOPIC_SPEEDOMETER)

# Callback function for when a message is received on a subscribed topic
def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()

    global isActive
    global bus

    if topic == TOPIC_CMD:
        if payload == MESSAGE_START:
            print("Starting ...")
            isActive = True
            message = can.Message(arbitration_id=CAN_MSG_SIMULATION_CMD_ID, data=[CAN_MSG_SIMULATION_CMD_DATA_START], is_extended_id=False)
            bus.send(message)

        elif payload == MESSAGE_KILL:
            print("Ending ...")
            isActive = False
            message = can.Message(arbitration_id=CAN_MSG_SIMULATION_CMD_ID, data=[CAN_MSG_SIMULATION_CMD_DATA_KILL], is_extended_id=False)
            bus.send(message)

    elif topic == TOPIC_SPEEDOMETER:
        speed_float = float(payload)
        speed_int = int(speed_float)
        message = can.Message(arbitration_id=CAN_MSG_SPEED_ID, data=[speed_int], is_extended_id=False)
        bus.send(message)

    elif topic == TOPIC_SOS:
        if payload == MESSAGE_ECALL:
            print("ECALL SIGNAL RECIEVED")
            message = can.Message(arbitration_id=CAN_MSG_SOS_ID, data=[CAN_MSG_SOS_DATA_ECALL], is_extended_id=False)
            bus.send(message)
        
        elif payload == MESSAGE_END:
            print("END OF SOS")
            message = can.Message(arbitration_id=CAN_MSG_SOS_ID, data=[CAN_MSG_SOS_DATA_END], is_extended_id=False)
            bus.send(message)
    
    elif topic == TOPIC_DMS:
        if payload == MESSAGE_SLEEP:
            print("SLEEP DETECTED")
            message = can.Message(arbitration_id=CAN_MSG_DMS_ID, data=[CAN_MSG_DMS_DATA_SLEEP], is_extended_id=False)
            bus.send(message)
    
        elif payload == MESSAGE_DISTRACTED:
            print("DISTRACTION DETECTED")
            message = can.Message(arbitration_id=CAN_MSG_DMS_ID, data=[CAN_MSG_DMS_DATA_DISTRACTED], is_extended_id=False)
            bus.send(message)
    


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
    message = bus.recv(timeout=1.0)

    if message is not None:
        id = message.arbitration_id
        data = message.data[0]

        if id == CAN_MSG_SOS_ID:
            if data == CAN_MSG_SOS_DATA_REQUEST_BCALL:
                print("BCALL SIGNAL RECIEVED")
                client.publish(TOPIC_SOS, MESSAGE_BCALL)
                message = can.Message(arbitration_id=CAN_MSG_SOS_ID, data=[CAN_MSG_SOS_DATA_BCALL], is_extended_id=False)
                bus.send(message)

client.loop_stop()
client.disconnect()