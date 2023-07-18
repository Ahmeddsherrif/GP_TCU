import paho.mqtt.client as mqtt
import can
import time

from enum import Enum, auto
from queue import Queue
import threading

# MQTT broker details
BROKER_ADDRESS = "localhost"
BROKER_PORT = 1883

PROCESS_NAME = "can"

TOPIC_CMD = "cmd"
TOPIC_CMD_PROCESS = "cmd/" + PROCESS_NAME
MESSAGE_START = "start"
MESSAGE_KILL = "kill"
MESSAGE_TERMINATE = "terminate"

TOPIC_STATUS_PROCESS = "status/" + PROCESS_NAME
MESSAGE_STATUS = "status"
MESSAGE_STATUS_ACTIVE = "active"
MESSAGE_STATUS_DEAD = "dead"
MESSAGE_STATUS_TERMINATE = "terminate"

TOPIC_SOS = "sos"
MESSAGE_ECALL = "ecall"
MESSAGE_BCALL = "bcall"
MESSAGE_END = "end"

TOPIC_SPEEDOMETER = "sensors/speedometer"


TOPIC_DMS = "dms"
MESSAGE_SLEEP = "sleep"
MESSAGE_DISTRACTED = "distracted"

CAN_MSG_SIMULATION_CMD_ID = 0x09
CAN_MSG_SIMULATION_CMD_DATA_KILL = 0x01
CAN_MSG_SIMULATION_CMD_DATA_START = 0x02

CAN_MSG_SOS_ID = 0x10
CAN_MSG_SOS_DATA_ECALL = 0x01
CAN_MSG_SOS_DATA_BCALL = 0x02
CAN_MSG_SOS_DATA_REQUEST_BCALL = 0x03
CAN_MSG_SOS_DATA_END = 0x04

CAN_MSG_SPEED_ID = 0x12

CAN_MSG_DMS_ID = 0x11
CAN_MSG_DMS_DATA_DISTRACTED = 0x01
CAN_MSG_DMS_DATA_SLEEP = 0x02


class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    etherFrame_ecall_recived = auto()
    etherFrame_end_recived = auto()
    etherFrame_sleep_recived = auto()
    etherFrame_distracted_recived = auto()
    etherFrame_speedometer_recived = auto()
    canFrame_bcall_recived= auto()
    status = auto()
    terminate = auto()


class State(Enum):
    dead = auto()
    active = auto()


INITIAL_STATE = State.dead

# ِ All the variables used by the state Machine


class stateMachineContext:
    def __init__(self):
        self.client = None
        self.bus = None


class EventMessage:
    def __init__(self):
        self.event = Event.idle
        self.ethernetData = None
        self.canData = None


# Create a mutex lock
mutexEventQueue = threading.Lock()
sharedEventMessageQueue = Queue()


class StateMachine:
    def __init__(self, context):
        self.currentState = INITIAL_STATE
        self.currentEventMessage = EventMessage()

        self.stateEntry = True
        self.stateExit = False

        self.context = context

    def getEventMessage(self):
        self.currentEventMessage.event = Event.idle

        # get can bus messages
        message = self.context.bus.recv(timeout=0)

        if message is not None:
            can_id = message.arbitration_id
            data = message.data[0]

            self.currentEventMessage.canData = data
            self.currentEventMessage.ethernetData = ""
            
            if can_id == CAN_MSG_SOS_ID:
                if data == CAN_MSG_SOS_DATA_REQUEST_BCALL:
                    self.currentEventMessage.event = Event.canFrame_bcall_recived
        else:
            mutexEventQueue.acquire()
            if sharedEventMessageQueue.empty() == False:
                self.currentEventMessage = sharedEventMessageQueue.get()
            mutexEventQueue.release()

        # TODO: Add another self queue to handle its own event in case of a conflict in one of the state event with shared events

    def state_dead_handler_processing(self):
        pass

    def state_dead_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_DEAD)
            print("Entering Dead State")

        # Process State
        self.state_dead_handler_processing()
        
        

        # Handle Events
        if self.currentEventMessage.event == Event.start:
            message = can.Message(arbitration_id=CAN_MSG_SIMULATION_CMD_ID, data=[CAN_MSG_SIMULATION_CMD_DATA_START], is_extended_id=False)
            self.context.bus.send(message)
            
            self.currentState = State.active
            self.stateExit = True
            
        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_DEAD)

        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True

            re_init(self.context)

            print("Exiting Dead State")

    def state_active_handler_processing(self):
        pass

    def state_active_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)
            print("Entering active State")

        # Process State
        self.state_active_handler_processing()
        
        # Handle Events
        if self.currentEventMessage.event == Event.kill:
            message = can.Message(arbitration_id=CAN_MSG_SIMULATION_CMD_ID, data=[CAN_MSG_SIMULATION_CMD_DATA_KILL], is_extended_id=False)
            self.context.bus.send(message)
            
            self.currentState = State.dead
            self.stateExit = True
            
        elif self.currentEventMessage.event == Event.etherFrame_ecall_recived:
            message1 = can.Message(arbitration_id=CAN_MSG_DMS_ID, data=[CAN_MSG_DMS_DATA_SLEEP], is_extended_id=False)
            message2 = can.Message(arbitration_id=CAN_MSG_SOS_ID, data=[CAN_MSG_SOS_DATA_ECALL], is_extended_id=False)
            self.context.bus.send(message1)
            self.context.bus.send(message2)
        
        elif self.currentEventMessage.event == Event.etherFrame_end_recived:
            message = can.Message(arbitration_id=CAN_MSG_SOS_ID, data=[CAN_MSG_SOS_DATA_END], is_extended_id=False)
            self.context.bus.send(message)
        
        elif self.currentEventMessage.event == Event.etherFrame_sleep_recived:
            message = can.Message(arbitration_id=CAN_MSG_DMS_ID, data=[CAN_MSG_DMS_DATA_SLEEP], is_extended_id=False)
            self.context.bus.send(message)
        
        elif self.currentEventMessage.event == Event.etherFrame_distracted_recived:
            message = can.Message(arbitration_id=CAN_MSG_DMS_ID, data=[CAN_MSG_DMS_DATA_DISTRACTED], is_extended_id=False)
            self.context.bus.send(message)
        
        elif self.currentEventMessage.event == Event.etherFrame_speedometer_recived:
            speed_float = float(self.currentEventMessage.ethernetData)
            speed_int = int(speed_float)
            message = can.Message(arbitration_id=CAN_MSG_SPEED_ID, data=[speed_int], is_extended_id=False)
            self.context.bus.send(message)
        
        elif self.currentEventMessage.event == Event.canFrame_bcall_recived:
            self.context.client.publish(TOPIC_SOS, MESSAGE_BCALL)
            message1 = can.Message(arbitration_id=CAN_MSG_SOS_ID, data=[CAN_MSG_SOS_DATA_BCALL], is_extended_id=False)
            message2 = can.Message(arbitration_id=CAN_MSG_DMS_ID, data=[CAN_MSG_DMS_DATA_SLEEP], is_extended_id=False)
            self.context.bus.send(message1)
            self.context.bus.send(message2)
            
        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)
        
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")


    def run(self):
        while (True):

            self.getEventMessage()

            # Break Condition
            if self.currentEventMessage.event == Event.terminate:
                message = can.Message(arbitration_id=CAN_MSG_SIMULATION_CMD_ID, data=[CAN_MSG_SIMULATION_CMD_DATA_KILL], is_extended_id=False)
                self.context.bus.send(message)
                
                
                self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_TERMINATE)
                
                break

            # Process States
            if self.currentState == State.dead:
                self.state_dead_handler()

            elif self.currentState == State.active:
                self.state_active_handler()

        print("HASTA LAVISTA")


# Callback function for when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_CMD_PROCESS)
    client.subscribe(TOPIC_SOS)
    client.subscribe(TOPIC_DMS)
    client.subscribe(TOPIC_SPEEDOMETER)

# Callback function for when a message is received on a subscribed topic


def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload
    
    tempEventMessage = EventMessage()
    tempEventMessage.ethernetData = payload
    tempEventMessage.canData = ""

    if topic == TOPIC_CMD or topic == TOPIC_CMD_PROCESS:
        decodedPayload = payload.decode()
        
        if decodedPayload == MESSAGE_START:
            tempEventMessage.event = Event.start

        elif decodedPayload == MESSAGE_KILL:
            tempEventMessage.event = Event.kill
        
        elif decodedPayload == MESSAGE_TERMINATE:
            tempEventMessage.event = Event.terminate
            
        elif decodedPayload == MESSAGE_STATUS:
            tempEventMessage.event = Event.status

    elif topic == TOPIC_SPEEDOMETER:
        tempEventMessage.event = Event.etherFrame_speedometer_recived

    elif topic == TOPIC_SOS:
        decodedPayload = payload.decode()
        
        if decodedPayload == MESSAGE_ECALL:
            tempEventMessage.event = Event.etherFrame_ecall_recived

        elif decodedPayload == MESSAGE_END:

            tempEventMessage.event = Event.etherFrame_end_recived

    elif topic == TOPIC_DMS:
        decodedPayload = payload.decode()
        
        if decodedPayload == MESSAGE_SLEEP:
            tempEventMessage.event = Event.etherFrame_sleep_recived

        elif decodedPayload == MESSAGE_DISTRACTED:
            tempEventMessage.event = Event.etherFrame_distracted_recived

    mutexEventQueue.acquire()
    sharedEventMessageQueue.put(tempEventMessage)
    mutexEventQueue.release()

def init(context):
    context.client = mqtt.Client()
    context.client.on_connect = on_connect
    context.client.on_message = on_message
    context.client.connect(BROKER_ADDRESS, BROKER_PORT)
    context.client.loop_start()

    context.bus = can.interface.Bus(channel='can0', bustype='socketcan')


def end(context):
    context.client.loop_stop()
    context.client.disconnect()


def re_init(context):
    pass


def main():
    context = stateMachineContext()

    init(context)

    sm = StateMachine(context)
    sm.run()

    end(context)


if __name__ == "__main__":
    main()