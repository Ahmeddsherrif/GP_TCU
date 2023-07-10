import paho.mqtt.client as mqtt
import cv2
import numpy as np
from tflite_runtime.interpreter import Interpreter
from collections import deque
from enum import Enum, auto
from queue import Queue
import threading


BROKER_ADDRESS                  = "localhost"
BROKER_PORT                     = 1883


PROCESS_NAME                    = "sign"


TOPIC_CMD                       = "cmd"
TOPIC_CMD_PROCESS               = "cmd/" + PROCESS_NAME
MESSAGE_START                   = "start"
MESSAGE_KILL                    = "kill"
MESSAGE_TERMINATE               = "terminate"
MESSAGE_STATUS                  = "status"

TOPIC_STATUS_PROCESS            = "status/" + PROCESS_NAME
MESSAGE_STATUS_ACTIVE           = "active"
MESSAGE_STATUS_DEAD             = "dead"
MESSAGE_STATUS_TERMINATE        = "terminate"

TOPIC_SIGN_VIDEO                = "video/sign"

TOPIC_VIDEO_CONTROL             = "video/control"
MESSAGE_STREAM_END              = "stream_end"

TOPIC_SOS                       = "sos"
MESSAGE_SIGN                    = "sign"
MESSAGE_NO_SIGN                 = "no_sign"

MODEL_PATH                      = "/home/pi/Desktop/CENTRAL_GATEWAY/SIGN_DETECTION/tflite_model_udemy.tflite"
TRIGGER_THRESHOLD               = 0.6
TRIGGER_CONSECUTIVE_FRAMES      = 2


FRAME_RATE                      = 30
FRAME_RATE_PROCESSING           = 3
FRAME_RATE_DIVISOR              = FRAME_RATE / FRAME_RATE_PROCESSING


class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    frame_recieved = auto()
    status = auto()
    terminate = auto()

class State(Enum):
    dead = auto()
    active = auto()

INITIAL_STATE = State.dead

#ِ All the variables used by the state Machine
class stateMachineContext:
    def __init__(self):
        self.client = None
        self.buffer = None
        self.interpreter = None
        self.input_details = None
        self.output_details = None
        self.frame_counter = None


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
    
    def state_active_handler_event_frame_recived_processing(self):
        
        self.context.frame_counter += 1
        
        if self.context.frame_counter == FRAME_RATE_DIVISOR:
            self.context.frame_counter = 0
        
            nparr = np.frombuffer(self.currentEventMessage.data, np.uint8)
            frame = cv2.imdecode(nparr,  cv2.IMREAD_COLOR)        
            processed_frame = cv2.resize(frame, (224, 224))  # Resize to input shape
            processed_frame = np.expand_dims(processed_frame, axis=0)  # Add batch dimension
            processed_frame = processed_frame.astype(np.float32)  # Convert to float32
            processed_frame = processed_frame / 255.0  # Normalize to [0, 1]
            
            # Set the input tensor
            self.context.interpreter.set_tensor(self.context.input_details[0]['index'], processed_frame)
            
            # Run the model inference
            self.context.interpreter.invoke()
            
            # Get the output tensor
            output_data = self.context.interpreter.get_tensor(self.context.output_details[0]['index'])[0]
            
            if output_data[1] >= TRIGGER_THRESHOLD:
                self.context.buffer.append(1)

            else:
                self.context.buffer.append(0)
            

            if sum(self.context.buffer) == TRIGGER_CONSECUTIVE_FRAMES:
                self.context.client.publish(TOPIC_SOS, MESSAGE_SIGN)
                print("SIGN_DETECTED")     
            else:
                self.context.client.publish(TOPIC_SOS, MESSAGE_NO_SIGN)
                print("NaN")


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
            self.state_active_handler_event_frame_recived_processing()
            
            
        elif self.currentEventMessage.event == Event.status:
            self.context.client.publish(TOPIC_STATUS_PROCESS, MESSAGE_STATUS_ACTIVE)
        
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")
            

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
        

def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_CMD)
    client.subscribe(TOPIC_CMD_PROCESS)
    client.subscribe(TOPIC_SIGN_VIDEO)
    client.subscribe(TOPIC_VIDEO_CONTROL)

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
            pass #tempEventMessage.event = Event.kill

    elif topic == TOPIC_SIGN_VIDEO:
        tempEventMessage.event = Event.frame_recieved
        
    mutexEventQueue.acquire()
    sharedEventMessageQueue.put(tempEventMessage)
    mutexEventQueue.release()            



def init(context):
    context.client = mqtt.Client()
    context.client.on_connect = on_connect
    context.client.on_message = on_message
    context.client.connect(BROKER_ADDRESS, BROKER_PORT)
    context.client.loop_start()
    
    context.buffer = deque(maxlen=TRIGGER_CONSECUTIVE_FRAMES)  # Set the maximum size of the circular buffer to 5
    
    # Load the TensorFlow Lite model
    context.interpreter = Interpreter(MODEL_PATH)
    context.interpreter.allocate_tensors()

    # Get the input and output tensors
    context.input_details = context.interpreter.get_input_details()
    context.output_details = context.interpreter.get_output_details()

def end(context):
    context.client.loop_stop()
    context.client.disconnect()
    

def re_init(context):
    if len(context.buffer) != 0:
        context.buffer.clear()
        
    context.frame_counter = 0


def main():
    context = stateMachineContext()

    init(context)

    sm = StateMachine(context)
    sm.run()

    end(context)

if __name__ == "__main__":
    main()
