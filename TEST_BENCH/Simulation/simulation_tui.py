
from enum import Enum, auto
from queue import Queue
import threading
import paho.mqtt.client as mqtt
import time
import os


# I DONT' WANT TO QUEUE A STREAM !!!!!!!

BROKER_ADDRESS                  = "192.168.1.11"
BROKER_PORT                     = 1883

PROCESS_NAME_VIDEO              = "video"
PROCESS_NAME_ECALL              = "ecall"
PROCESS_NAME_ADR                = "adr"
PROCESS_NAME_BLACK_BOX_VIDEO    = "black_box_video"
PROCESS_NAME_SENSORS            = "sensors"
PROCESS_NAME_SIGN               = "sign"
PROCESS_NAME_CAN                = "can"
PROCESS_NAME_AUDIO_MOBILE       = "audio_mobile"
PROCESS_NAME_AUDIO_MANGER       = "audio_manger"
PROCESS_NAME_BLACK_BOX_AUDIO    = "black_box_audio"
PROCESS_NAME_BLACK_BOX_SENSORS  = "black_box_sensors"
PROCESS_NAME_AUDIO_PLAYBACK     = "audio_playback"



# TODO: store them in an array and fix the simulation/cmd/
TOPIC_CMD_ECALL                         = "cmd/" + PROCESS_NAME_ECALL
TOPIC_CMD_ADR                           = "cmd/" + PROCESS_NAME_ADR
TOPIC_CMD_BLACK_BOX_VIDEO               = "cmd/" + PROCESS_NAME_BLACK_BOX_VIDEO
TOPIC_CMD_SENSORS                       = "cmd/" + PROCESS_NAME_SENSORS
TOPIC_CMD_VIDEO                         = "cmd/" + PROCESS_NAME_VIDEO
TOPIC_CMD_SIGN                          = "cmd/" + PROCESS_NAME_SIGN
TOPIC_CMD_CAN                           = "cmd/" + PROCESS_NAME_CAN
TOPIC_CMD_AUDIO_MOBILE                  = "cmd/" + PROCESS_NAME_AUDIO_MOBILE
TOPIC_CMD_AUDIO_MANGER                  = "cmd/" + PROCESS_NAME_AUDIO_MANGER
TOPIC_CMD_BLACK_BOX_AUDIO               = "cmd/" + PROCESS_NAME_BLACK_BOX_AUDIO
TOPIC_CMD_BLACK_BOX_SENSORS             = "cmd/" + PROCESS_NAME_BLACK_BOX_SENSORS
TOPIC_CMD_AUDIO_PLAYBACK                = "cmd/" + PROCESS_NAME_AUDIO_PLAYBACK
TOPIC_CMD                               = "cmd"

MESSAGE_START                           = "start"
MESSAGE_KILL                            = "kill"
MESSAGE_PAUSE				            = "pause"
MESSAGE_RESUME				            = "resume"
MESSAGE_TERMINATE                       = "terminate"
MESSAGE_STATUS                          = "status"

TOPIC_STATUS_PROCESS_ECALL	            = "status/" + PROCESS_NAME_ECALL
TOPIC_STATUS_PROCESS_ADR	            = "status/" + PROCESS_NAME_ADR
TOPIC_STATUS_PROCESS_BLACK_BOX_VIDEO    = "status/" + PROCESS_NAME_BLACK_BOX_VIDEO
TOPIC_STATUS_PROCESS_SENSORS            = "status/" + PROCESS_NAME_SENSORS
TOPIC_STATUS_PROCESS_VIDEO	            = "status/" + PROCESS_NAME_VIDEO
TOPIC_STATUS_PROCESS_SIGN	            = "status/" + PROCESS_NAME_SIGN
TOPIC_STATUS_PROCESS_CAN	            = "status/" + PROCESS_NAME_CAN
TOPIC_STATUS_PROCESS_AUDIO_MOBILE 	    = "status/" + PROCESS_NAME_AUDIO_MOBILE
TOPIC_STATUS_PROCESS_AUDIO_MANGER  	    = "status/" + PROCESS_NAME_AUDIO_MANGER
TOPIC_STATUS_PROCESS_BLACK_BOX_AUDIO  	        = "status/" + PROCESS_NAME_BLACK_BOX_AUDIO
TOPIC_STATUS_PROCESS_BLACK_BOX_SENSORS  	    = "status/" + PROCESS_NAME_BLACK_BOX_SENSORS
TOPIC_STATUS_PROCESS_AUDIO_PLAYBACK 	        = "status/" + PROCESS_NAME_AUDIO_PLAYBACK

MESSAGE_STATUS_ACTIVE           = "active"
MESSAGE_STATUS_DEAD             = "dead"
MESSAGE_STATUS_IDLE             = "idle"
MESSAGE_STATUS_CALL             = "call"
MESSAGE_STATUS_SOS              = "sos"
MESSAGE_STATUS_HALT             = "halt"
MESSAGE_STATUS_TERMINATE        = "terminate"

TOPIC_ADR                       = "sensors/adr"

TOPIC_SOS					    = "sos"
MESSAGE_ECALL				    = "ecall"
MESSAGE_BCALL				    = "bcall"
MESSAGE_SIGN				    = "sign"
MESSAGE_NO_SIGN                 = "no_sign"
MESSAGE_GUN					    = "gun"

TOPIC_OUTAGE				    = "sensors/outage"
MESSAGE_OUTAGE				    = "outage"
MESSAGE_NO_OUTAGE			    = "no_outage"

TOPIC_DMS                       = "dms"
MESSAGE_SLEEP                   = "sleep"
MESSAGE_DISTRACTED              = "distracted"

REFRESH_TIMEOUT_SECONDS         = 1
REFRESH_DAEMON_SECONDS          = 30

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
    received_status_black_box_video = auto()
    received_status_sensors = auto()
    received_status_video = auto()
    received_status_sign = auto()
    received_status_can = auto()
    received_status_audio_manger = auto()
    received_status_audio_mobile = auto()
    received_status_black_box_audio = auto()
    received_status_black_box_sensors = auto()
    received_status_audio_playback = auto()
    received_adr = auto()
    

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
        self.status_black_box_video = None
        self.status_video = None
        self.status_sign = None
        self.status_audio_manger = None
        self.status_audio_mobile = None
        self.status_black_box_audio = None
        self.status_black_box_sensors = None
        self.status_audio_playback = None

        self.data_adr = None
        self.data_sign = None
    


class EventMessage:
    def __init__(self):
        self.event = Event.idle
        self.data = None
        self.index = None
        
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

  

    def cmd_a_process(self, index, MESSAGE):
        if index == None:
            self.context.client.publish(TOPIC_CMD, MESSAGE)
            
        elif index == str(Index.PROCESS_ADR.value):
            self.context.client.publish(TOPIC_CMD_ADR, MESSAGE)  
            
        elif index == str(Index.PROCESS_BLACK_BOX_VIDEO.value):
            self.context.client.publish(TOPIC_CMD_BLACK_BOX_VIDEO, MESSAGE)
           
        elif index == str(Index.PROCESS_CAN.value):
            self.context.client.publish(TOPIC_CMD_CAN, MESSAGE) 
            
        elif index == str(Index.PROCESS_ECALL.value):
            self.context.client.publish(TOPIC_CMD_ECALL, MESSAGE)
               
        elif index == str(Index.PROCESS_SENSORS.value):
            self.context.client.publish(TOPIC_CMD_SENSORS, MESSAGE)
        
        elif index == str(Index.PROCESS_SIGN.value):
            self.context.client.publish(TOPIC_CMD_SIGN, MESSAGE)
            
        elif index == str(Index.PROCESS_VIDEO.value):
            self.context.client.publish(TOPIC_CMD_VIDEO, MESSAGE)
            
        elif index == str(Index.PROCESS_AUDIO_MOBILE.value):
            self.context.client.publish(TOPIC_CMD_AUDIO_MOBILE, MESSAGE)
            
        elif index == str(Index.PROCESS_AUDIO_MANGER.value):
            self.context.client.publish(TOPIC_CMD_AUDIO_MANGER, MESSAGE)
            
        elif index == str(Index.PROCESS_BLACK_BOX_AUDIO.value):
            self.context.client.publish(TOPIC_CMD_BLACK_BOX_AUDIO, MESSAGE)
        
        elif index == str(Index.PROCESS_BLACK_BOX_SENSORS.value):
            self.context.client.publish(TOPIC_CMD_BLACK_BOX_SENSORS, MESSAGE)
        
        elif index == str(Index.PROCESS_AUDIO_PLAYBACK.value):
            self.context.client.publish(TOPIC_CMD_AUDIO_PLAYBACK, MESSAGE)


    def state_active_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            
            print("Entering active State")
            render_TUI(self.context)
            
            self.stateEntryTime = time.time()
  

        # Process State
        self.state_active_handler_processing()

        
        if self.currentEventMessage.event == Event.send_start:
            self.cmd_a_process(self.currentEventMessage.index, MESSAGE_START)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.send_kill:
            self.cmd_a_process(self.currentEventMessage.index, MESSAGE_KILL)
            render_TUI(self.context)
            
            
        elif self.currentEventMessage.event == Event.send_terminate:
            self.cmd_a_process(self.currentEventMessage.index, MESSAGE_TERMINATE)
            render_TUI(self.context)
            
            
        elif self.currentEventMessage.event == Event.send_pause:
            self.cmd_a_process(self.currentEventMessage.index, MESSAGE_PAUSE)
            render_TUI(self.context)
            
        
        elif self.currentEventMessage.event == Event.send_resume:
            self.cmd_a_process(self.currentEventMessage.index, MESSAGE_RESUME)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.send_outage:
            self.context.client.publish(TOPIC_OUTAGE, MESSAGE_OUTAGE)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.send_no_outage:
            self.context.client.publish(TOPIC_OUTAGE, MESSAGE_NO_OUTAGE)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.send_refresh:
            render_TUI(self.context)
           
            self.currentState = State.refresh
            self.stateExit = True
            
            
        elif self.currentEventMessage.event == Event.send_distracted:
            self.context.client.publish(TOPIC_DMS, MESSAGE_DISTRACTED)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.send_sleep: 
            self.context.client.publish(TOPIC_DMS, MESSAGE_SLEEP)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.send_ecall:
            self.context.client.publish(TOPIC_SOS, MESSAGE_ECALL)
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_sos:
            self.context.client.publish(TOPIC_CMD_SENSORS, MESSAGE_PAUSE)

            decodedPayload = self.currentEventMessage.data.decode()

            if decodedPayload == MESSAGE_SIGN or decodedPayload == MESSAGE_NO_SIGN:
                self.context.data_sign = decodedPayload


            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_status_ecall:
            self.context.status_ecall = self.currentEventMessage.data.decode()
            render_TUI(self.context)
   
        elif self.currentEventMessage.event == Event.received_status_adr:
            self.context.status_adr = self.currentEventMessage.data.decode()
            render_TUI(self.context)
          
            
        elif self.currentEventMessage.event == Event.received_status_black_box_video:
            self.context.status_black_box_video = self.currentEventMessage.data.decode()
            render_TUI(self.context)
     
            
        elif self.currentEventMessage.event == Event.received_status_sensors:
            self.context.status_sensors = self.currentEventMessage.data.decode()
            render_TUI(self.context)
         
            
        elif self.currentEventMessage.event == Event.received_status_video:
            self.context.status_video = self.currentEventMessage.data.decode()
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_status_sign:
            self.context.status_sign = self.currentEventMessage.data.decode()
            render_TUI(self.context)
          
        elif self.currentEventMessage.event == Event.received_status_can:
            self.context.status_can = self.currentEventMessage.data.decode()
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_status_audio_manger:
            self.context.status_audio_manger = self.currentEventMessage.data.decode()
            render_TUI(self.context)


        elif self.currentEventMessage.event == Event.received_status_audio_mobile:
            self.context.status_audio_mobile = self.currentEventMessage.data.decode()
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_status_black_box_audio:
            self.context.status_black_box_audio = self.currentEventMessage.data.decode()
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_status_black_box_sensors:
            self.context.status_black_box_sensors = self.currentEventMessage.data.decode()
            render_TUI(self.context)
            
        elif self.currentEventMessage.event == Event.received_status_audio_playback:
            self.context.status_audio_playback = self.currentEventMessage.data.decode()
            render_TUI(self.context)

        elif self.currentEventMessage.event == Event.received_adr:
            self.context.data_adr = self.currentEventMessage.data.decode()
            render_TUI(self.context)

        
        if self.stateExit == True:
            self.stateExit = False
            self.stateEntry = True
            print("Exiting active State")
            
            
            
    
    def state_refresh_handler_processing(self):
        passed_time = time.time() - self.stateEntryTime 
        
        if passed_time >= REFRESH_TIMEOUT_SECONDS:
            self.currentEventMessage.event = Event.refresh_timeout          #TODO: should push it in a queue
            
    def state_active_handler_processing(self):
        passed_time = time.time() - self.stateEntryTime 
        if passed_time >= REFRESH_DAEMON_SECONDS:
            self.currentEventMessage.event = Event.send_refresh          #TODO: should push it in a queue

             

    def state_refresh_handler(self):
        if self.stateEntry == True:
            self.stateEntry = False
            
            re_init(self.context)
            
            self.context.client.publish(TOPIC_CMD, MESSAGE_STATUS)
            
            self.stateEntryTime = time.time()
            
            print("Entering refresh State")

        # Process State
        self.state_refresh_handler_processing()
            
        if self.currentEventMessage.event == Event.received_status_ecall:
            self.context.status_ecall = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_adr:
            self.context.status_adr = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_black_box_video:
            self.context.status_black_box_video = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_sensors:
            self.context.status_sensors = self.currentEventMessage.data.decode()
          
        elif self.currentEventMessage.event == Event.received_status_video:
            self.context.status_video = self.currentEventMessage.data.decode()
        
        elif self.currentEventMessage.event == Event.received_status_sign:
            self.context.status_sign = self.currentEventMessage.data.decode()
      
        elif self.currentEventMessage.event == Event.received_status_can:
            self.context.status_can = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_audio_manger:
            self.context.status_audio_manger = self.currentEventMessage.data.decode()

        elif self.currentEventMessage.event == Event.received_status_audio_mobile:
            self.context.status_audio_mobile = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_black_box_audio:
            self.context.status_black_box_audio = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_black_box_sensors:
            self.context.status_black_box_sensors = self.currentEventMessage.data.decode()
            
        elif self.currentEventMessage.event == Event.received_status_audio_playback:
            self.context.status_audio_playback = self.currentEventMessage.data.decode()
            
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
                
            
                


def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_SOS)
    client.subscribe(TOPIC_STATUS_PROCESS_SENSORS)
    client.subscribe(TOPIC_STATUS_PROCESS_ECALL)
    client.subscribe(TOPIC_STATUS_PROCESS_ADR)
    client.subscribe(TOPIC_STATUS_PROCESS_CAN)
    client.subscribe(TOPIC_STATUS_PROCESS_BLACK_BOX_VIDEO)
    client.subscribe(TOPIC_STATUS_PROCESS_VIDEO)
    client.subscribe(TOPIC_STATUS_PROCESS_SIGN)
    client.subscribe(TOPIC_STATUS_PROCESS_AUDIO_MANGER)
    client.subscribe(TOPIC_STATUS_PROCESS_AUDIO_MOBILE)
    client.subscribe(TOPIC_STATUS_PROCESS_BLACK_BOX_AUDIO)
    client.subscribe(TOPIC_STATUS_PROCESS_BLACK_BOX_SENSORS)
    client.subscribe(TOPIC_STATUS_PROCESS_AUDIO_PLAYBACK)

    client.subscribe(TOPIC_ADR)

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
        
    elif topic == TOPIC_STATUS_PROCESS_BLACK_BOX_VIDEO:
        tempEventMessage.event = Event.received_status_black_box_video
        
    elif topic == TOPIC_STATUS_PROCESS_VIDEO:
        tempEventMessage.event = Event.received_status_video
        
    elif topic == TOPIC_STATUS_PROCESS_SIGN:
        tempEventMessage.event = Event.received_status_sign
        
    elif topic == TOPIC_STATUS_PROCESS_AUDIO_MOBILE:
        tempEventMessage.event = Event.received_status_audio_mobile
        
    elif topic == TOPIC_STATUS_PROCESS_AUDIO_MANGER:
        tempEventMessage.event = Event.received_status_audio_manger
        
    elif topic == TOPIC_STATUS_PROCESS_BLACK_BOX_AUDIO:
        tempEventMessage.event = Event.received_status_black_box_audio
        
    elif topic == TOPIC_STATUS_PROCESS_BLACK_BOX_SENSORS:
        tempEventMessage.event = Event.received_status_black_box_sensors
        
    elif topic == TOPIC_STATUS_PROCESS_AUDIO_PLAYBACK:
        tempEventMessage.event = Event.received_status_audio_playback
        
    elif topic == TOPIC_ADR:
        tempEventMessage.event = Event.received_adr
        
    mutexEventQueue.acquire()
    sharedEventMessageQueue.put(tempEventMessage)
    mutexEventQueue.release()
            
def init(context):
    context.client = mqtt.Client()
    context.client.on_connect = on_connect
    context.client.on_message = on_message
    context.client.connect(BROKER_ADDRESS, BROKER_PORT)
    context.client.loop_start()
    
    re_init(context)
    
def end(context):
    context.client.loop_stop()
    context.client.disconnect()

def re_init(context):
    context.status_sensors = "-"
    context.status_ecall = "-"
    context.status_adr = "-"
    context.status_can = "-"
    context.status_black_box_video = "-"
    context.status_video = "-"
    context.status_sign = "-"
    context.status_audio_manger = "-"
    context.status_audio_mobile = "-"

    context.status_black_box_audio = "-"
    context.status_black_box_sensors = "-"
    context.status_audio_playback = "-"
    
    context.data_sign = "-"
    context.data_adr = "-"


def input_thread():
    while True:
        inputCMD = input()
        
        parsed_CMD = inputCMD.split()
        
        
        tempEventMessage = EventMessage()
      
        if parsed_CMD[0] == INPUT_START:
            tempEventMessage.event  = Event.send_start
            if len(parsed_CMD) == 2:
                tempEventMessage.index = parsed_CMD[1]
            
        elif parsed_CMD[0] == INPUT_KILL:
            tempEventMessage.event  = Event.send_kill
            if len(parsed_CMD) == 2:
                tempEventMessage.index = parsed_CMD[1]
            
        elif parsed_CMD[0] == INPUT_PAUSE :
            tempEventMessage.event  = Event.send_pause
            if len(parsed_CMD) == 2:
                tempEventMessage.index = parsed_CMD[1]
            
        elif parsed_CMD[0] == INPUT_RESUME:
            tempEventMessage.event  = Event.send_resume
            if len(parsed_CMD) == 2:
                tempEventMessage.index = parsed_CMD[1]
            
        elif parsed_CMD[0] == INPUT_TERMINATE:
            tempEventMessage.event  = Event.send_terminate
            if len(parsed_CMD) == 2:
                tempEventMessage.index = parsed_CMD[1]
            
        elif parsed_CMD[0] == INPUT_OUTAGE:
            tempEventMessage.event  = Event.send_outage
            
        elif parsed_CMD[0] == INPUT_NO_OUTAGE:
            tempEventMessage.event = Event.send_no_outage

            
        elif parsed_CMD[0] == INPUT_DISTRACTED:
            tempEventMessage.event  = Event.send_distracted
            
        elif parsed_CMD[0] == INPUT_SLEEP:
            tempEventMessage.event  = Event.send_sleep
            
        elif parsed_CMD[0] == INPUT_REFRESH:
            tempEventMessage.event  = Event.send_refresh
            
        elif parsed_CMD[0] == INPUT_ECALL:
            tempEventMessage.event  = Event.send_ecall
            
        mutexEventQueue.acquire()
        sharedEventMessageQueue.put(tempEventMessage)
        mutexEventQueue.release()


        
        
        

def print_welcome():
    print("---------------------------------------------") 
    print("S <index>: Start process <index>")
    print("K <index>: Kill process <index>")
    print("P <index>: Pause process <index>")
    print("R <index>: Resume process <index>")
    print("T <index>: Terminate process <index>")
    print("O : Start a GPS Outage")
    print("N : End a GPS Outage")
    print("E : Generate ECALL")
    print("D : [TEST_ONLY] Distracted Driver")
    print("Z : [TEST_ONLY] Sleeping Driver")
    print("Q : refresh")
    print("---------------------------------------------")  
    print()



class Index(Enum):
    PROCESS_ECALL = auto()
    PROCESS_ADR = auto()
    PROCESS_BLACK_BOX_VIDEO = auto()
    PROCESS_BLACK_BOX_AUDIO = auto()
    PROCESS_BLACK_BOX_SENSORS = auto()
    PROCESS_AUDIO_MOBILE = auto()
    PROCESS_SIGN = auto()
    PROCESS_CAN = auto()
    PROCESS_SENSORS = auto()
    PROCESS_VIDEO = auto()
    PROCESS_AUDIO_MANGER = auto()
    PROCESS_AUDIO_PLAYBACK = auto()
    


def print_status(context):
    print("<" + str(Index.PROCESS_ECALL.value)             + ">  PROCESS ECALL              : " + context.status_ecall)
    print("<" + str(Index.PROCESS_ADR.value)               + ">  PROCESS ADR                : " + context.status_adr)
    print("<" + str(Index.PROCESS_BLACK_BOX_VIDEO.value)   + ">  PROCESS BLACK BOX VIDEO    : " + context.status_black_box_video)
    print("<" + str(Index.PROCESS_BLACK_BOX_AUDIO.value)   + ">  PROCESS BLACK BOX AUDIO    : " + context.status_black_box_audio)
    print("<" + str(Index.PROCESS_BLACK_BOX_SENSORS.value) + ">  PROCESS BLACK BOX SENSORS  : " + context.status_black_box_sensors)
    print("<" + str(Index.PROCESS_AUDIO_MOBILE.value)      + ">  PROCESS AUDIO MOBILE       : " + context.status_audio_mobile)
    print()

    

    
    print("<" + str(Index.PROCESS_SIGN.value)              + ">  PROCESS SIGN               : " + context.status_sign)
    print("<" + str(Index.PROCESS_CAN.value)               + ">  PROCESS CAN                : " + context.status_can)
    print()
    

    
    print("<" + str(Index.PROCESS_SENSORS.value)           + ">  PROCESS SENSORS            : " + context.status_sensors)
    print("<" + str(Index.PROCESS_VIDEO.value)             + "> PROCESS VIDEO              : " + context.status_video)
    print("<" + str(Index.PROCESS_AUDIO_MANGER.value)      + "> PROCESS AUDIO MANGER       : " + context.status_audio_manger)
    print("<" + str(Index.PROCESS_AUDIO_PLAYBACK.value)    + "> PROCESS AUDIO PLAYBACK     : " + context.status_audio_playback)
    print()
 
    
def print_data(context):
    print("LAST DATA ADR         : " + context.data_adr)
    print("LAST DATA SIGN        : " + context.data_sign)
    print()


def render_TUI(context):
    os.system("clear")
    print_welcome()
    
    print_status(context)
    print_data(context)
    

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
