from flask import Flask, render_template, jsonify
from queue import Queue
import paho.mqtt.client as mqtt
import threading


BROKER_ADDRESS  = "192.168.1.11"
BROKER_PORT     = 1883


TOPIC_SPEEDOMETER = "sensors/speedometer"

TOPIC_SEATBELT = "seatbelt"
MESSAGE_LEFT_SEATBELT_HOOKED = "left_hooked"
MESSAGE_LEFT_SEATBELT_UNHOOKED = "left_unhooked"
MESSAGE_REQUEST = "request"

TOPIC_SOS = "sos"
MESSAGE_ECALL = "ecall"
MESSAGE_BCALL = "bcall"
MESSAGE_END = "end"

is_SOS_triggered = 0
is_seatbelt_hoocked = 0


app = Flask(__name__)



mutexSharedSpeedometer = threading.Lock()
speedometer = Queue()

# Callback function for when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_SPEEDOMETER)
    client.subscribe(TOPIC_SEATBELT)
    client.subscribe(TOPIC_SOS)

# Callback function for when a message is received on a subscribed topic
def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()
    
    global is_seatbelt_hoocked
    global is_SOS_triggered
    
    if topic == TOPIC_SPEEDOMETER:
        
        speed = float(payload)
        
        mutexSharedSpeedometer.acquire()
        speedometer.put(speed)
        mutexSharedSpeedometer.release()
        
    elif topic == TOPIC_SEATBELT:
        if payload == MESSAGE_LEFT_SEATBELT_HOOKED:
            is_seatbelt_hoocked = 1
        
        if payload == MESSAGE_LEFT_SEATBELT_UNHOOKED:
            is_seatbelt_hoocked = 0
            
    elif topic == TOPIC_SOS:
        if payload == MESSAGE_ECALL or payload == MESSAGE_BCALL:
            is_SOS_triggered = 1
            
        elif payload ==  MESSAGE_END:
            is_SOS_triggered = 0
            

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/get_dashboard')
def get_dashboard():
    speed = 0
    if speedometer.empty() == False:
        mutexSharedSpeedometer.acquire()
        speed = speedometer.get()
        mutexSharedSpeedometer.release()
        
    data = {
        'speed': speed,
        'sos': is_SOS_triggered,
        'airbag': is_SOS_triggered,
        'seatbelt': is_seatbelt_hoocked,
    }

    return jsonify(data)

if __name__ == '__main__':
    
        # Create an MQTT client instance
    client = mqtt.Client()

    # Set the callback functions
    client.on_connect = on_connect
    client.on_message = on_message

    # Connect to the MQTT broker
    client.connect(BROKER_ADDRESS, BROKER_PORT)

    # Start the MQTT network loop in a separate thread
    client.loop_start()

    app.run(debug=True)
