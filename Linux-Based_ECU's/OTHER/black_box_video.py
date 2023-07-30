import paho.mqtt.client as mqtt
import cv2
import numpy as np
import shutil
from collections import deque
from enum import Enum, auto
from queue import Queue
import threading
from datetime import datetime
import os
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication

def write_deque_to_csv(deque_data, filename):
    
    # Open the CSV file in 'w' (write) mode
    with open(filename, 'w', newline='') as file:
        
        # Write each item in the deque as a row in the CSV file
        for item in deque_data:
            file.write(item)


BROKER_ADDRESS              = "192.168.1.11"
BROKER_PORT                 = 1883

PROCESS_NAME                = "black_box_video"

OUTPUT_DIR                  = "/home/ahmed/Desktop/TELEMATICS_CONTROL_UNIT/BLACK_BOX/data/"

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
MESSAGE_ECALL               = "ecall"
MESSAGE_BCALL               = "bcall"

TOPIC_IMU					= "sensors/imu"
TOPIC_SPEEDOMETER			= "sensors/speedometer"
TOPIC_GPS					= "sensors/adr"

WIDTH                       = 640
HEIGHT                      = 480


RATE_FRAME                  = 15
RATE_IMU                    = 100
RATE_GPS                    = 1
RATE_SPEEDOMETER            = 40
RECORD_TIME_SECONDS         = 5

BUFFER_MAX_LENGTH_FRAME = RATE_FRAME * RECORD_TIME_SECONDS
BUFFER_MAX_LENGTH_IMU = RATE_IMU * RECORD_TIME_SECONDS
BUFFER_MAX_LENGTH_GPS = RATE_GPS * RECORD_TIME_SECONDS
BUFFER_MAX_LENGTH_SPEEDOMETER = RATE_SPEEDOMETER * RECORD_TIME_SECONDS


CSV_TIME_FORMAT = "%d-%b-%Y %H:%M:%S.%f"
DIR_TIME_FORMAT = "%Y-%m-%d_%H-%M-%S"

# Define the position and other text parameters
FONT = cv2.FONT_HERSHEY_SIMPLEX
FONT_SCALE = 1
COLOR = (255, 255, 255)  # White color in BGR format (OpenCV uses BGR instead of RGB)
THICKNESS = 2
POSITION = (10, 30)  # (x, y) coordinates for the top-left corner of the text



SMTP_PORT = 587
SMTP_SERVER = "smtp.gmail.com"
EMAIL_SENDER = "ahmedsherifcollage@gmail.com"
PASSWORD     = "iwjkbqdqnopxcxsh"
EMAIL_RECIEVER = "ahmedselfiky11@gmail.com"



class Event(Enum):
    idle = auto()
    start = auto()
    kill = auto()
    sos = auto()
    status = auto()
    frame_recieved = auto()
    terminate = auto()
    sample_recievied_imu = auto()
    sample_recievied_gps = auto()
    sample_recievied_speedometer= auto()
class State(Enum):
    dead = auto()
    active = auto()

INITIAL_STATE = State.dead

#ِ All the variables used by the state Machine
class stateMachineContext:
    def __init__(self):
        self.client = None
        self.buffer_frame = None
        self.buffer_imu = None
        self.buffer_gps = None
        self.buffer_speedometer = None
        self.server = None

class EventMessage:
    def __init__(self):
        self.event = Event.idle
        self.data = None

# Create a mutex lock
mutexEventQueue = threading.Lock()
sharedEventMessageQueue = Queue()


def get_time_str(time_format):
    current_datetime = datetime.now()
    time_string = current_datetime.strftime(time_format)
    
    return time_string


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
            current_time = get_time_str(CSV_TIME_FORMAT)
            nparr = np.frombuffer(self.currentEventMessage.data, np.uint8)
            frame = cv2.imdecode(nparr,  cv2.IMREAD_COLOR)    
            frame_with_time = cv2.putText(frame, current_time, POSITION, FONT, FONT_SCALE, COLOR, THICKNESS, cv2.LINE_AA)    
            self.context.buffer_frame.append(frame_with_time)
            
        elif self.currentEventMessage.event == Event.sample_recievied_imu:
            current_time = get_time_str(CSV_TIME_FORMAT)
            current_reading = self.currentEventMessage.data
            str = current_time + "," + current_reading.decode()
            self.context.buffer_imu.append(str)
            
        elif self.currentEventMessage.event == Event.sample_recievied_gps:
            current_time = get_time_str(CSV_TIME_FORMAT)
            current_reading = self.currentEventMessage.data
            str = current_time + "," + current_reading.decode() + "\n"
            self.context.buffer_gps.append(str)
            
        elif self.currentEventMessage.event == Event.sample_recievied_speedometer:
            current_time = get_time_str(CSV_TIME_FORMAT)
            current_reading = self.currentEventMessage.data
            str = current_time + "," + current_reading.decode()
            self.context.buffer_speedometer.append(str)
            
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
        
        print("FRAME:" + str(len(self.context.buffer_frame)) + "/" + str(BUFFER_MAX_LENGTH_FRAME))
        print("IMU: " + str(len(self.context.buffer_imu)) + "/" + str(BUFFER_MAX_LENGTH_IMU))
        print("GPS: " +str(len(self.context.buffer_gps)) + "/" + str(BUFFER_MAX_LENGTH_GPS))
        print("Speedometer: " + str(len(self.context.buffer_speedometer)) + "/" + str(BUFFER_MAX_LENGTH_SPEEDOMETER))
        
        current_time = get_time_str(DIR_TIME_FORMAT)

        if len(self.context.buffer_frame) == BUFFER_MAX_LENGTH_FRAME \
            or len(self.context.buffer_imu) == BUFFER_MAX_LENGTH_IMU \
            or len(self.context.buffer_gps) == BUFFER_MAX_LENGTH_GPS \
            or len(self.context.buffer_speedometer) == BUFFER_MAX_LENGTH_SPEEDOMETER:
                
            current_blackbox_folder_name = OUTPUT_DIR +"BB_"+ current_time
            os.system("mkdir " + current_blackbox_folder_name)      
            
            if len(self.context.buffer_frame) == BUFFER_MAX_LENGTH_FRAME:
                current_blackbox_folder_name_video = current_blackbox_folder_name + "/video"
                os.system("mkdir " + current_blackbox_folder_name_video)
                output_video_path = current_blackbox_folder_name_video + "/"
                output_video_name = output_video_path + "BB_VID.mp4"
                output_codec = cv2.VideoWriter_fourcc(*'mp4v')
                output_writer = cv2.VideoWriter(output_video_name, output_codec, RATE_FRAME, (WIDTH, HEIGHT))

                # Write frames from the deque to the output video
                for frame in self.context.buffer_frame:
                    output_writer.write(frame)

                # Release the output video writer
                output_writer.release()
                print("Done Saving!")
            
            
            if len(self.context.buffer_imu) == BUFFER_MAX_LENGTH_IMU \
                or len(self.context.buffer_gps) == BUFFER_MAX_LENGTH_GPS \
                or len(self.context.buffer_speedometer) == BUFFER_MAX_LENGTH_SPEEDOMETER:
                    
                current_blackbox_folder_name_sensors= current_blackbox_folder_name + "/sensors"
                os.system("mkdir " + current_blackbox_folder_name_sensors)
                output_sensor_path = current_blackbox_folder_name_sensors + "/"

                if len(self.context.buffer_imu) == BUFFER_MAX_LENGTH_IMU:
                    output_imu_name = output_sensor_path + "BB_IMU.csv"
                    write_deque_to_csv(self.context.buffer_imu, output_imu_name)
                
                if len(self.context.buffer_gps) == BUFFER_MAX_LENGTH_GPS:
                    output_gps_name = output_sensor_path + "BB_GPS.csv"
                    write_deque_to_csv(self.context.buffer_gps, output_gps_name)
                
                if len(self.context.buffer_speedometer) == BUFFER_MAX_LENGTH_SPEEDOMETER:
                    output_speedometer_name = output_sensor_path + "BB_SPEEDOMETER.csv"
                    write_deque_to_csv(self.context.buffer_speedometer, output_speedometer_name)
                    
            print("Zipping File ... ")
            output_zip_file_name = OUTPUT_DIR + "output"
            shutil.make_archive(output_zip_file_name, 'zip', current_blackbox_folder_name)
            output_zip_file_path = output_zip_file_name + ".zip"
            print("Done Zipping!")
            
            
            # Email configuration
            subject = "ECALL"
            body = "1A00A23132BCA45A4C0CA54C0AC40000A5C45AC404540C45AS56C456AC40C0000000AC554C"

            # File attachment details
            attachment_path = output_zip_file_path

            # Create a MIMEText object to represent the email content
            msg = MIMEMultipart()
            msg['From'] = EMAIL_SENDER
            msg['To'] = EMAIL_RECIEVER
            msg['Subject'] = subject
            msg.attach(MIMEText(body, 'plain'))

            # Attach the .zip file to the email
            with open(attachment_path, "rb") as file:
                attachment = MIMEApplication(file.read(), Name="BLACK_BOX.zip")  # Replace "your_file_name.zip" with the desired attachment filename
                attachment['Content-Disposition'] = f'attachment; filename="{attachment.get_filename()}"'
                msg.attach(attachment)
                   
            print("Sending Email...")     
            self.context.server.sendmail(EMAIL_SENDER, EMAIL_RECIEVER, msg.as_string())
            print("Done Sending!")
        
            os.system("rm " + attachment_path)
            
            print("Done Removing Zip File")
        


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
    client.subscribe(TOPIC_SOS)
    client.subscribe(TOPIC_IMU)
    client.subscribe(TOPIC_SPEEDOMETER)
    client.subscribe(TOPIC_GPS)

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
        if decodedPayload == MESSAGE_SIGN or decodedPayload == MESSAGE_ECALL or decodedPayload == MESSAGE_BCALL:
            tempEventMessage.event = Event.sos
            
    elif topic == TOPIC_IMU:
        tempEventMessage.event = Event.sample_recievied_imu
        
    elif topic == TOPIC_GPS:
        tempEventMessage.event = Event.sample_recievied_gps
        
    elif topic == TOPIC_SPEEDOMETER:
        tempEventMessage.event = Event.sample_recievied_speedometer
            
    mutexEventQueue.acquire()
    sharedEventMessageQueue.put(tempEventMessage)
    mutexEventQueue.release()
            
            
def init(context):
    context.client = mqtt.Client()
    context.client.on_connect = on_connect
    context.client.on_message = on_message
    context.client.connect(BROKER_ADDRESS, BROKER_PORT)
    context.client.loop_start()
    
    
    context.buffer_frame = deque(maxlen=BUFFER_MAX_LENGTH_FRAME)
    context.buffer_imu = deque(maxlen=BUFFER_MAX_LENGTH_IMU)
    context.buffer_gps = deque(maxlen=BUFFER_MAX_LENGTH_GPS)
    context.buffer_speedometer = deque(maxlen=BUFFER_MAX_LENGTH_SPEEDOMETER)
    
    context.server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
    context.server.starttls()
    context.server.login(EMAIL_SENDER, PASSWORD)


def end(context):
    context.client.loop_stop()
    context.client.disconnect()
    
    context.server.quit()
    

def re_init(context):
    if len(context.buffer_frame) != 0:
        context.buffer_frame.clear()
        
    if len(context.buffer_imu) != 0:
        context.buffer_imu.clear()
        
    if len(context.buffer_gps) != 0:
        context.buffer_gps.clear()
        
    if len(context.buffer_speedometer) != 0:
        context.buffer_speedometer.clear()
        


def main():
    context = stateMachineContext()

    init(context)

    sm = StateMachine(context)
    sm.run()

    end(context)

if __name__ == "__main__":
    main()
