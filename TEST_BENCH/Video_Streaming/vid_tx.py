
from enum import Enum, auto
from queue import Queue
import threading
import time
import cv2
import paho.mqtt.client as mqtt

BROKER_ADDRESS              = "192.168.1.11"
BROKER_PORT                 = 1883


TOPIC_CMD                   = "simulation/cmd"
MESSAGE_START               = "start"
MESSAGE_KILL                = "kill"
MESSAGE_TERMINATE           = "terminate"
MESSAGE_STATUS              = "status"

TOPIC_STATUS_PROCESS        = "status/video"
MESSAGE_STATUS_ACTIVE       = "active"
MESSAGE_STATUS_DEAD         = "dead"
MESSAGE_STATUS_HALT         = "halt"
MESSAGE_STATUS_TERMINATE    = "terminate"

TOPIC_SIGN_VIDEO            = "video/sign"

TOPIC_VIDEO_CONTROL         = "simulation/control/video"
MESSAGE_STREAM_END          = "stream_end"
MESSAGE_PAUSE	            = "pause"
MESSAGE_RESUME	            = "resume"

WIDTH = 640
HEIGHT = 480
FRAME_RATE = 30

FRAME_DELAY = 1.0 / FRAME_RATE

PATH_VIDEO_SIGN = "input_sign.mp4"


class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    terminate = auto()
    pause = auto()
    status = auto()
    resume = auto()

class State(Enum):
    dead = auto()
    active = auto()
    halt = auto()

INITIAL_STATE = State.dead

#ِ All the variables used by the state Machine
class stateMachineContext:
    def __init__(self):
        self.client = None
        self.cap = None
        self.video_path = None
        self.video_topic = None

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

    def state_active_handler_processing(self):
        start_time = time.time()
        
        ret, frame = self.context.cap.read()
        
        if ret == True:
            frame = cv2.resize(frame, (WIDTH ,HEIGHT))  
            frame_str = cv2.imencode('.jpg', frame)[1].tobytes()
            self.context.client.publish(self.context.video_topic, frame_str)
            
            cv2.imshow('Video', frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                self.currentEventMessage.event = Event.kill
                
            elapsed_time = time.time() - start_time
            time.sleep(max(0, FRAME_DELAY - elapsed_time))
                
        else:
            self.context.client.publish(TOPIC_VIDEO_CONTROL, MESSAGE_STREAM_END)
            time.sleep(1)
            self.currentEventMessage.event = Event.kill



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


    def state_active_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)
            print("Entering active State")

        # Process State
        self.state_active_handler_processing()

        # Handle Events
        if self.currentEventMessage.event == Event.kill:
            
            if self.context.cap.isOpened() == True:
                self.context.cap.release()
                
            
            self.currentState = State.dead
            self.stateExit = True
            
        elif self.currentEventMessage.event == Event.pause:
            self.currentState = State.halt
            self.stateExit = True
       
        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)
  
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")

    def state_halt_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_HALT)
            print("Entering halt State")
            # TODO: add text on opencv window

        # Process State
        self.state_halt_handler_processing()

        # Handle Events
        if self.currentEventMessage.event == Event.kill:
            
            if self.context.cap.isOpened() == True:
                self.context.cap.release()
                
            
            self.currentState = State.dead
            self.stateExit = True
            
        if self.currentEventMessage.event == Event.resume:
            self.currentState = State.active
            self.stateExit = True

        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_HALT)
        
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting halt State")
            
    def state_halt_handler_processing(self):
        pass


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
                
            elif self.currentState == State.halt:
                self.state_halt_handler()

        print("HASTA LAVISTA")



def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_VIDEO_CONTROL)

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload

    tempEventMessage = EventMessage()
    tempEventMessage.data = payload

    if topic == TOPIC_CMD:
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
        if decodedPayload == MESSAGE_PAUSE:
            tempEventMessage.event = Event.pause
        elif decodedPayload == MESSAGE_RESUME:
            tempEventMessage.event = Event.resume

    mutexEventQueue.acquire()
    sharedEventMessageQueue.put(tempEventMessage)
    mutexEventQueue.release()
            
def init(context):
    context.client = mqtt.Client()
    context.client.on_connect = on_connect
    context.client.on_message = on_message
    context.client.connect(BROKER_ADDRESS, BROKER_PORT)
    context.client.loop_start()

    
    
def end(context):
    context.client.loop_stop()
    context.client.disconnect()
    
    if context.cap.isOpened():
        context.cap.release()
    
    cv2.destroyAllWindows() 

def re_init(context):
    
    # Change it on start events in case of diffrent scienarios
    context.video_path = PATH_VIDEO_SIGN
    context.video_topic = TOPIC_SIGN_VIDEO
    
    context.cap = cv2.VideoCapture(context.video_path)
    if not context.cap.isOpened():
        print("Error opening video file")
        exit()

def main():
    context = stateMachineContext()

    init(context)

    sm = StateMachine(context)
    sm.run()

    end(context)

if __name__ == "__main__":
    main()
