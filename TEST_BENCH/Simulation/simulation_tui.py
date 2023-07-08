
from enum import Enum, auto
from queue import Queue
import threading
import paho.mqtt.client as mqtt
import time
import os

BROKER_ADDRESS                  = "192.168.1.11"
BROKER_PORT                     = 1883

TOPIC_CMD                       = "simulation/cmd"
MESSAGE_START                   = "start"
MESSAGE_KILL                    = "kill"
MESSAGE_TERMINATE               = "terminate"
MESSAGE_STATUS                  = "status"

TOPIC_STATUS_PROCESS_SENSORS    = "status/sensors"
TOPIC_STATUS_PROCESS_ECALL	    = "status/ecall"
TOPIC_STATUS_PROCESS_ADR	    = "status/adr"
TOPIC_STATUS_PROCESS_CAN	    = "status/can"
TOPIC_STATUS_PROCESS_BLACK_BOX  = "status/black_box"
TOPIC_STATUS_PROCESS_VIDEO	    = "status/video"
TOPIC_STATUS_PROCESS_SIGN	    = "status/sign"
MESSAGE_STATUS_ACTIVE           = "active"
MESSAGE_STATUS_DEAD             = "dead"
MESSAGE_STATUS_IDLE             = "idle"
MESSAGE_STATUS_CALL             = "call"
MESSAGE_STATUS_SOS              = "sos"
MESSAGE_STATUS_HALT             = "halt"
MESSAGE_STATUS_TERMINATE        = "terminate"

TOPIC_SOS					    = "sos"
MESSAGE_ECALL				    = "ecall"
MESSAGE_BCALL				    = "bcall"
MESSAGE_SIGN				    = "sign"
MESSAGE_GUN					    = "gun"

TOPIC_OUTAGE				    = "simulation/outage"
MESSAGE_OUTAGE				    = "outage"
MESSAGE_NO_OUTAGE			    = "no_outage"

TOPIC_CONTROL_VIDEO			    = "simulation/control/video"
TOPIC_CONTROL_SENSORS		    = "simulation/control/sensors"
MESSAGE_PAUSE				    = "pause"
MESSAGE_RESUME				    = "resume"

TOPIC_DMS                       = "dms"
MESSAGE_SLEEP                   = "sleep"
MESSAGE_DISTRACTED              = "distracted"

REFRESH_TIMEOUT_SECONDS         = 1

INPUT_START                    = "s"
INPUT_KILL                     = "k"
INPUT_TERMINATE                = "t"
INPUT_OUTAGE                   = "o"
INPUT_NO_OUTAGE                = "n"
INPUT_PAUSE                    = "p"
INPUT_RESUME                   = "r"
INPUT_DISTRACTED               = "d"
INPUT_SLEEP                    = "z"
INPUT_REFRESH                  = "q"
INPUT_ECALL                    = "e"

class Event(Enum):
    idle = auto()
    send_start = auto()
    send_kill = auto()
    send_terminate = auto()
    send_pause = auto()
    send_resume = auto()
    send_outage = auto()
    send_no_outage = auto()
    send_refresh = auto()
    send_distracted = auto()
    send_sleep = auto()
    send_ecall = auto()
    refresh_timeout = auto()
    received_sos = auto()
    received_status_ecall = auto()
    received_status_adr = auto()
    received_status_black_box = auto()
    received_status_sensors = auto()
    received_status_video = auto()
    received_status_sign = auto()
    received_status_can = auto()
    

class State(Enum):
    refresh = auto()
    active = auto()
    

INITIAL_STATE = State.refresh

#ِ All the variables used by the state Machine
class stateMachineContext:
    def __init__(self):
        
        
        self.client = None
        
        # Store them in a list
        self.status_sensors = None
        self.status_ecall = None
        self.status_adr = None
        self.status_can = None
        self.status_black_box = None
        self.status_video = None
        self.status_sign = None


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
        
        self.stateEntryTime = None

        self.context = context

    def getEventMessage(self):
        self.currentEventMessage.event = Event.idle;
                
        mutexEventQueue.acquire()
        if sharedEventMessageQueue.empty() == False:
            self.currentEventMessage = sharedEventMessageQueue.get()
        mutexEventQueue.release()
        
        #TODO: Add another self queue to handle its own event in case of a conflict in one of the state event with shared events

  

    def state_active_handler_processing(self):
        pass




    def state_active_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            print("Entering active State")

        # Process State
        self.state_active_handler_processing()

        
        if self.currentEventMessage.event == Event.send_start:
            self.context.client.publish(TOPIC_CMD, MESSAGE_START)
            
        elif self.currentEventMessage.event == Event.send_kill:
            self.context.client.publish(TOPIC_CMD, MESSAGE_KILL)
            
        elif self.currentEventMessage.event == Event.send_terminate:
            self.context.client.publish(TOPIC_CMD, MESSAGE_TERMINATE)
            
        elif self.currentEventMessage.event == Event.send_pause:
            self.context.client.publish(TOPIC_CONTROL_VIDEO, MESSAGE_PAUSE)
            self.context.client.publish(TOPIC_CONTROL_SENSORS, MESSAGE_PAUSE)
        
        elif self.currentEventMessage.event == Event.send_resume:
            self.context.client.publish(TOPIC_CONTROL_VIDEO, MESSAGE_RESUME)
            self.context.client.publish(TOPIC_CONTROL_SENSORS, MESSAGE_RESUME)
            
        elif self.currentEventMessage.event == Event.send_outage:
            self.context.client.publish(TOPIC_OUTAGE, MESSAGE_OUTAGE)
            
        elif self.currentEventMessage.event == Event.send_no_outage:
            self.context.client.publish(TOPIC_OUTAGE, MESSAGE_NO_OUTAGE)
            
        elif self.currentEventMessage.event == Event.send_refresh:
            self.currentState = State.refresh
            self.stateExit = True
            
        elif self.currentEventMessage.event == Event.send_distracted:
            self.context.client.publish(TOPIC_DMS, MESSAGE_DISTRACTED)
            
        elif self.currentEventMessage.event == Event.send_sleep: 
             self.context.client.publish(TOPIC_DMS, MESSAGE_SLEEP)
            
        elif self.currentEventMessage.event == Event.send_ecall:
             self.context.client.publish(TOPIC_SOS, MESSAGE_ECALL)
            
        elif self.currentEventMessage.event == Event.received_sos:
            self.context.client.publish(TOPIC_CONTROL_SENSORS, MESSAGE_PAUSE)
            
        elif self.currentEventMessage.event == Event.received_status_ecall:
            self.context.status_ecall = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_adr:
            self.context.status_adr = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_black_box:
            self.context.status_black_box = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_sensors:
            self.context.status_sensors = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_video:
            self.context.status_video = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_sign:
            self.context.status_sign = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_can:
            self.context.status_can = self.currentEventMessage.data.decode()
        
        
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")
            
            
            
    
    def state_refresh_handler_processing(self):
        passed_time = time.time() - self.stateEntryTime 
        if passed_time >= REFRESH_TIMEOUT_SECONDS:
            self.currentEventMessage.event = Event.refresh_timeout          #TODO: should push it in a queue
             

    def state_refresh_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            self.context.status_sensors = "-"
            self.context.status_ecall = "-"
            self.context.status_adr = "-"
            self.context.status_can = "-"
            self.context.status_black_box = "-"
            self.context.status_video = "-"
            self.context.status_sign = "-"
            
            self.context.client.publish(TOPIC_CMD, MESSAGE_STATUS)
            
            self.stateEntryTime = time.time()
            
            print("Entering refresh State")

        # Process State
        self.state_refresh_handler_processing()
            
        if self.currentEventMessage.event == Event.received_status_ecall:
            self.context.status_ecall = self.currentEventMessage.data.decode()
            print("STATUS ECALL: " + self.context.status_ecall)
            
        elif self.currentEventMessage.event == Event.received_status_adr:
            self.context.status_adr = self.currentEventMessage.data.decode()
            print("STATUS ADR: " + self.context.status_adr)
            
        elif self.currentEventMessage.event == Event.received_status_black_box:
            self.context.status_black_box = self.currentEventMessage.data.decode()
            print("STATUS BBOX: " + self.context.status_black_box)
            
        elif self.currentEventMessage.event == Event.received_status_sensors:
            self.context.status_sensors = self.currentEventMessage.data.decode()
            print("STATUS SENSORS: " + self.context.status_sensors)
            
        elif self.currentEventMessage.event == Event.received_status_video:
            self.context.status_video = self.currentEventMessage.data.decode()
            print("STATUS VIDEO: " + self.context.status_video)
            
        elif self.currentEventMessage.event == Event.received_status_sign:
            self.context.status_sign = self.currentEventMessage.data.decode()
            print("STATUS SIGN: " + self.context.status_sign)
            
        elif self.currentEventMessage.event == Event.received_status_can:
            self.context.status_can = self.currentEventMessage.data.decode()
            print("STATUS CAN: " + self.context.status_can)
            
        elif self.currentEventMessage.event == Event.refresh_timeout:
            self.currentState = State.active
            self.stateExit = True
        
        
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting refresh State")


    def run(self):
        while(True):

            self.getEventMessage()

            if self.currentState == State.active:
                self.state_active_handler()
                
            elif self.currentState == State.refresh:
                self.state_refresh_handler()
                
            if self.currentEventMessage == Event.send_terminate:
                print("BYE BYE")
                break
            
            
                


def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_SOS)
    client.subscribe(TOPIC_STATUS_PROCESS_SENSORS)
    client.subscribe(TOPIC_STATUS_PROCESS_ECALL)
    client.subscribe(TOPIC_STATUS_PROCESS_ADR)
    client.subscribe(TOPIC_STATUS_PROCESS_CAN)
    client.subscribe(TOPIC_STATUS_PROCESS_BLACK_BOX)
    client.subscribe(TOPIC_STATUS_PROCESS_VIDEO)
    client.subscribe(TOPIC_STATUS_PROCESS_SIGN)

def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload

    tempEventMessage = EventMessage()
    tempEventMessage.data = payload

    if topic == TOPIC_SOS:
        tempEventMessage.event = Event.received_sos
        
    elif topic == TOPIC_STATUS_PROCESS_SENSORS:
        tempEventMessage.event = Event.received_status_sensors
        
    elif topic == TOPIC_STATUS_PROCESS_ECALL:
        tempEventMessage.event = Event.received_status_ecall
        
    elif topic == TOPIC_STATUS_PROCESS_ADR:
        tempEventMessage.event = Event.received_status_adr
        
    elif topic == TOPIC_STATUS_PROCESS_CAN:
        tempEventMessage.event = Event.received_status_can
        
    elif topic == TOPIC_STATUS_PROCESS_BLACK_BOX:
        tempEventMessage.event = Event.received_status_black_box
        
    elif topic == TOPIC_STATUS_PROCESS_VIDEO:
        tempEventMessage.event = Event.received_status_video
        
    elif topic == TOPIC_STATUS_PROCESS_SIGN:
        tempEventMessage.event = Event.received_status_sign
        
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

def re_init(context):
    pass


def input_thread():
    while True:
        print_welcome()
        inputCMD = input("Enter CMD: ")
        os.system("clear")
        
        tempEventMessage = EventMessage()
        tempEventMessage.data = ""
        
        if inputCMD == INPUT_START:
            tempEventMessage.event  == Event.send_start
            
        elif inputCMD == INPUT_KILL:
            tempEventMessage.event  == Event.send_kill
            
        elif inputCMD == INPUT_TERMINATE:
            tempEventMessage.event  == Event.send_terminate
            
        elif inputCMD == INPUT_OUTAGE:
            tempEventMessage.event  == Event.send_outage
            
        elif inputCMD == INPUT_NO_OUTAGE:
            tempEventMessage.event  == Event.send_no_outage
            
        elif inputCMD == INPUT_PAUSE :
            tempEventMessage.event  == Event.send_pause
            
        elif inputCMD == INPUT_RESUME:
            tempEventMessage.event  == Event.send_resume
            
        elif inputCMD == INPUT_DISTRACTED:
            tempEventMessage.event  == Event.send_distracted
            
        elif inputCMD == INPUT_SLEEP:
            tempEventMessage.event  == Event.send_sleep
            
        elif inputCMD == INPUT_REFRESH:
            tempEventMessage.event  == Event.send_refresh
            
        elif inputCMD == INPUT_ECALL:
            tempEventMessage.event  == Event.send_ecall
            
        mutexEventQueue.acquire()
        sharedEventMessageQueue.put(tempEventMessage)
        mutexEventQueue.release()
        
        
        if tempEventMessage.event == INPUT_TERMINATE:
            break
        
        

def print_welcome():
    print("---------------------------------------------") 
    print("S -> Start Simulation" 			             )
    print("O -> Start a GPS Outage" 			         )
    print("N -> End a GPS Outage" 			             )
    print("E -> Generate ECALL" 				         )
    print("P -> Pause Sensor Readings" 		             )
    print("R -> Resume Sensor Readings" 		         )
    print("D -> Distracted Driver" 			             )
    print("Z -> Sleeping Driver (zZzZZ)" 		         )
    print("K -> reset Simulation" 			             )
    print("Q -> refresh States" 				         )
    print("T -> Terminate all" 				             )
    print("--------------------------------------------" )       
        

def main():
    context = stateMachineContext()

    init(context)
    
    thread = threading.Thread(target=input_thread)
    thread.start()

    sm = StateMachine(context)
    sm.run()
    
    thread.join()

    end(context)

if __name__ == "__main__":
    main()
