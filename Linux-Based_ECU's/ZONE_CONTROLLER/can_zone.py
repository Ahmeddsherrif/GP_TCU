import paho.mqtt.client as mqtt
import can

from enum import Enum
from queue import Queue
import threading

# MQTT broker details
BROKER_ADDRESS = "192.168.1.11"
BROKER_PORT = 1883

PROCESS_NAME = "can_zone"

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


TOPIC_ALARM = "alarm"
MESSAGE_ACTIVATE_FAST = "activate_fast"
MESSAGE_ACTIVATE_SLOW = "activate_slow"
MESSAGE_DEACTIVATE    = "deactivate"

CAN_MSG_SOS_ID = 0x09
CAN_MSG_SOS_DATA_REQUEST_BCALL = 0x02

CAN_MSG_ALARM_ID = 0x11
CAN_MSG_ALARM_ACTIVATE_FAST = 0x01
CAN_MSG_ALARM_ACTIVATE_SLOW = 0x02
CAN_MSG_ALARM_DEACTIVATE    = 0x03


class Event(Enum):
    idle = 1
    start = 2
    kill = 3
    etherFrame_ecall_recived = 4
    canFrame_bcall_recived= 5
    status = 6
    terminate = 7
    etherFrame_alarm_recieved = 8


class State(Enum):
    dead = 1
    active = 2


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
            self.currentState = State.dead
            self.stateExit = True
            
        elif self.currentEventMessage.event == Event.etherFrame_ecall_recived:
            message = can.Message(arbitration_id=CAN_MSG_ALARM_ID, data=[CAN_MSG_ALARM_ACTIVATE_FAST], is_extended_id=False)
            self.context.bus.send(message)

        elif self.currentEventMessage.event == Event.canFrame_bcall_recived:
            message = can.Message(arbitration_id=CAN_MSG_ALARM_ID, data=[CAN_MSG_ALARM_ACTIVATE_FAST], is_extended_id=False)
            self.context.bus.send(message)
            self.context.client.publish(TOPIC_SOS, MESSAGE_BCALL)
            
        elif self.currentEventMessage.event == Event.etherFrame_alarm_recieved:
            decodedPayload = self.currentEventMessage.ethernetData.decode()
            if  decodedPayload == MESSAGE_ACTIVATE_FAST:
                message = can.Message(arbitration_id=CAN_MSG_ALARM_ID, data=[CAN_MSG_ALARM_ACTIVATE_FAST], is_extended_id=False)
                
            elif decodedPayload == MESSAGE_ACTIVATE_SLOW:
                message = can.Message(arbitration_id=CAN_MSG_ALARM_ID, data=[CAN_MSG_ALARM_ACTIVATE_SLOW], is_extended_id=False)
                
            elif decodedPayload == MESSAGE_DEACTIVATE:
                message = can.Message(arbitration_id=CAN_MSG_ALARM_ID, data=[CAN_MSG_ALARM_DEACTIVATE], is_extended_id=False)
                
            self.context.bus.send(message)
            
            
            
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
    client.subscribe(TOPIC_ALARM)
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

    elif topic == TOPIC_SOS:
        decodedPayload = payload.decode()
        
        if decodedPayload == MESSAGE_ECALL:
            tempEventMessage.event = Event.etherFrame_ecall_recived
            
    elif topic == TOPIC_ALARM:
        tempEventMessage.event = Event.etherFrame_alarm_recieved

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
