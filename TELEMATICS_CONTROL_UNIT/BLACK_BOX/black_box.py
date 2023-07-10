import paho.mqtt.client as mqtt
import cv2
import numpy as np
import time
from collections import deque
from enum import Enum, auto
from queue import Queue
import threading

import os



BROKER_ADDRESS              = "192.168.1.11"
BROKER_PORT                 = 1883

PROCESS_NAME                = "black_box"

OUTPUT_VIDEO_DIR            = "/home/user/Desktop/TELEMATICS_CONTROL_UNIT/BLACK_BOX/Data/"

TOPIC_CMD                   = "cmd"
TOPIC_CMD_PROCESS           = "cmd/" + PROCESS_NAME
MESSAGE_START               = "start"
MESSAGE_KILL                = "kill"
MESSAGE_TERMINATE           = "terminate"
MESSAGE_STATUS              = "status"

TOPIC_STATUS_PROCESS        = "status/" + PROCESS_NAME
MESSAGE_STATUS_ACTIVE       = "active"
MESSAGE_STATUS_DEAD         = "dead"
MESSAGE_STATUS_TERMINATE    = "terminate"

TOPIC_SIGN_VIDEO            = "video/sign"

TOPIC_VIDEO_CONTROL         = "video/control"
MESSAGE_STREAM_END          = "stream_end"

TOPIC_SOS                   = "sos"
MESSAGE_SIGN                = "sign"

WIDTH                       = 640
HEIGHT                      = 480
FRAME_RATE                  = 30

RECORD_TIME_SECONDS         = 2


class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    sos = auto()
    status = auto()
    frame_recieved = auto()
    terminate = auto()

class State(Enum):
    dead = auto()
    active = auto()

INITIAL_STATE = State.dead

#ِ All the variables used by the state Machine
class stateMachineContext:
    def __init__(self):
        self.client = None
        self.frame_buffer = None

class EventMessage:
    def __init__(self):
        self.event = Event.idle
        self.data = None

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
        self.currentEventMessage.event = Event.idle;
                
        mutexEventQueue.acquire()
        if sharedEventMessageQueue.empty() == False:
            self.currentEventMessage = sharedEventMessageQueue.get()
        mutexEventQueue.release()
        
        #TODO: Add another self queue to handle its own event in case of a conflict in one of the state event with shared events

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
            
        elif self.currentEventMessage.event == Event.frame_recieved:
            nparr = np.frombuffer(self.currentEventMessage.data, np.uint8)
            frame = cv2.imdecode(nparr,  cv2.IMREAD_COLOR)        
            self.context.frame_buffer.append(frame)
            
        elif self.currentEventMessage.event == Event.sos:
            self.state_active_handler_event_sos_processing()
            self.currentState = State.dead
            self.stateExit = True
            
        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)
           
            
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")
            
    def state_active_handler_event_sos_processing(self):
        print("Saving Video ... ")
        current_time = time.strftime('%Y-%m-%d_%H-%M-%S')
        output_video_name = OUTPUT_VIDEO_DIR + "BB_" + current_time + ".mp4"
        output_codec = cv2.VideoWriter_fourcc(*'mp4v')
        output_writer = cv2.VideoWriter(output_video_name, output_codec, FRAME_RATE, (WIDTH, HEIGHT))

        # Write frames from the deque to the output video
        for frame in self.context.frame_buffer:
            output_writer.write(frame)

        # Release the output video writer
        output_writer.release()
        print("Done Saving!")
        
        send_video(output_video_name)
        print("Done Sending!")
        
        


    def run(self):
        while(True):

            self.getEventMessage()

            # Break Condition
            if self.currentEventMessage.event == Event.terminate:
                self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_TERMINATE)
                break;

            # Process States
            if self.currentState == State.dead:
                self.state_dead_handler()

            elif self.currentState == State.active:
                self.state_active_handler()

        print("HASTA LAVISTA")
        
def send_video(VIDEO_PATH):
    os.system("scp " + VIDEO_PATH + " ahmed@192.168.1.110:~/Desktop/TEST_BENCH/BLACK_BOX_OUTPUT")
    
def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_CMD_PROCESS)
    client.subscribe(TOPIC_SIGN_VIDEO)
    client.subscribe(TOPIC_VIDEO_CONTROL)
    client.subscribe(TOPIC_SOS)

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload
    

    tempEventMessage = EventMessage()
    tempEventMessage.data = payload

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

    elif topic == TOPIC_VIDEO_CONTROL:
        decodedPayload = payload.decode()
        
        if decodedPayload == MESSAGE_STREAM_END:
            tempEventMessage.event = Event.kill

    elif topic == TOPIC_SIGN_VIDEO:
        tempEventMessage.event = Event.frame_recieved

    elif topic == TOPIC_SOS:
        decodedPayload = payload.decode()
        if decodedPayload == MESSAGE_SIGN:
            tempEventMessage.event = Event.sos
            
    mutexEventQueue.acquire()
    sharedEventMessageQueue.put(tempEventMessage)
    mutexEventQueue.release()
            
            
def init(context):
    context.client = mqtt.Client()
    context.client.on_connect = on_connect
    context.client.on_message = on_message
    context.client.connect(BROKER_ADDRESS, BROKER_PORT)
    context.client.loop_start()
    
    context.frame_buffer = deque(maxlen=FRAME_RATE * RECORD_TIME_SECONDS)

def end(context):
    context.client.loop_stop()
    context.client.disconnect()
    

def re_init(context):
    if len(context.frame_buffer) != 0:
        context.frame_buffer.clear()
        


def main():
    context = stateMachineContext()

    init(context)

    sm = StateMachine(context)
    sm.run()

    end(context)

if __name__ == "__main__":
    main()
