import paho.mqtt.client as mqtt
import can

from enum import Enum, auto
from queue import Queue
import threading

# MQTT broker details
BROKER_ADDRESS = "localhost"
BROKER_PORT = 1883

PROCESS_NAME = "can_central"

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

TOPIC_SEATBELT = "seatbelt"
MESSAGE_RIGHT_SEAT_BELT_HOOKED   = "right_hooked"
MESSAGE_LEFT_SEAT_BELT_HOOKED    = "left_hooked"
MESSAGE_RIGHT_SEAT_BELT_UNHOOKED = "right_unhooked"
MESSAGE_LEFT_SEAT_BELT_UNHOOKED  = "left_unhooked"
MESSAGE_SEAT_BELT_REQUEST        = "request"

TOPIC_ALARM           = "alarm"
MESSAGE_ACTIVATE_FAST = "activate_fast"
MESSAGE_ACTIVATE_SLOW = "activate_slow"
MESSAGE_DEACTIVATE    = "deactivate"


CAN_MSG_SEAT_BELT_ID                            = 0x13
CAN_LEFT_SEAT_BELT_HOOKED		                = 0x1
CAN_LEFT_SEAT_BELT_UNHOOKED		                = 0x2
CAN_RIGHT_SEAT_BELT_HOOKED		                = 0x3
CAN_RIGHT_SEAT_BELT_UNHOOKED	                = 0x4
CAN_SEAT_BELT_REQUEST		                    = 0x5
 

class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    canFrame_seatbelt_recived= auto()
    etherFrame_request_seatbelt = auto()
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
        self.isHooked = None


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
            
            if can_id == CAN_MSG_SEAT_BELT_ID:
                self.currentEventMessage.event = Event.canFrame_seatbelt_recived
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
            
            message = can.Message(arbitration_id=CAN_MSG_SEAT_BELT_ID, data=[CAN_SEAT_BELT_REQUEST], is_extended_id=False)
            self.context.bus.send(message)
            
            print("Entering active State")

        # Process State
        self.state_active_handler_processing()
        
        # Handle Events
        if self.currentEventMessage.event == Event.kill:
            self.currentState = State.dead
            self.stateExit = True
            
            
        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)

        elif self.currentEventMessage.event == Event.canFrame_seatbelt_recived:
            if self.currentEventMessage.canData == CAN_LEFT_SEAT_BELT_HOOKED:
                self.context.client.publish(TOPIC_SEATBELT, MESSAGE_LEFT_SEAT_BELT_HOOKED)
                
                if self.context.isHooked == False:
                    self.context.client.publish(TOPIC_ALARM, MESSAGE_DEACTIVATE)
                    self.context.isHooked = True

            elif self.currentEventMessage.canData == CAN_LEFT_SEAT_BELT_UNHOOKED:
                self.context.client.publish(TOPIC_SEATBELT, MESSAGE_LEFT_SEAT_BELT_UNHOOKED)
                
                if self.context.isHooked == True:
                    self.context.client.publish(TOPIC_ALARM, MESSAGE_ACTIVATE_SLOW)
                    self.context.isHooked = False

            elif self.currentEventMessage.canData == CAN_RIGHT_SEAT_BELT_HOOKED:
                self.context.client.publish(TOPIC_SEATBELT, MESSAGE_RIGHT_SEAT_BELT_HOOKED)

            elif self.currentEventMessage.canData == CAN_RIGHT_SEAT_BELT_UNHOOKED:
                self.context.client.publish(TOPIC_SEATBELT, MESSAGE_RIGHT_SEAT_BELT_UNHOOKED)
                
        elif self.currentEventMessage.event == Event.etherFrame_request_seatbelt:
            message = can.Message(arbitration_id=CAN_MSG_SEAT_BELT_ID, data=[CAN_SEAT_BELT_REQUEST], is_extended_id=False)
            self.context.bus.send(message)
                
        
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
    client.subscribe(TOPIC_SEATBELT)


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
            
    if topic == TOPIC_SEATBELT:
        decodedPayload = payload.decode()
        if decodedPayload == MESSAGE_SEAT_BELT_REQUEST:
            tempEventMessage.event = Event.etherFrame_request_seatbelt

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
    
    context.isHooked = False


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