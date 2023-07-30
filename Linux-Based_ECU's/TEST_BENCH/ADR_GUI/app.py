from flask import Flask, render_template, jsonify
from queue import Queue
import paho.mqtt.client as mqtt
import threading


TOPIC_GPS = "sensors/gps"
TOPIC_ADR = "sensors/adr"


MESSAGE_OUTAGE = "outage"
MESSAGE_NO_OUTAGE = "no_outage"


BROKER_ADDRESS  = "192.168.1.11"
BROKER_PORT     = 1883


app = Flask(__name__)



mutexSharedGPS = threading.Lock()
mutexSharedADR = threading.Lock()

gps = Queue()
adr = Queue()

from math import radians, sin, cos, sqrt, atan2

def calculate_distance(lat1, lng1, lat2, lng2):
    # Radius of the Earth in kilometers
    R = 6371.0

    # Convert latitude and longitude from degrees to radians
    lat1_rad = radians(lat1)
    lng1_rad = radians(lng1)
    lat2_rad = radians(lat2)
    lng2_rad = radians(lng2)

    # Calculate the differences between the latitudes and longitudes
    dlat = lat2_rad - lat1_rad
    dlng = lng2_rad - lng1_rad

    # Use the Haversine formula to calculate the distance between the points
    a = sin(dlat / 2)**2 + cos(lat1_rad) * cos(lat2_rad) * sin(dlng / 2)**2
    c = 2 * atan2(sqrt(a), sqrt(1 - a))
    distance = R * c

    return distance * 1000


# Callback function for when the client connects to the MQTT broker
def on_connect(client, userdata, flags, rc):
    client.subscribe(TOPIC_GPS)
    client.subscribe(TOPIC_ADR)

# Callback function for when a message is received on a subscribed topic
def on_message(client, userdata, msg):
    topic = msg.topic
    payload = msg.payload.decode()
    
    if topic == TOPIC_GPS:
        
        split_values = payload.split(",")
        gps_lat = float(split_values[1])
        gps_lng = float(split_values[2])
        mutexSharedGPS.acquire()
        gps.put((gps_lat, gps_lng))
        mutexSharedGPS.release()
        
    elif topic == TOPIC_ADR:
        split_values = payload.split(",")
        adr_outage_str = split_values[0]
        
        adr_outage = None
        if adr_outage_str == MESSAGE_NO_OUTAGE:
            adr_outage = 0
            
        elif adr_outage_str == MESSAGE_OUTAGE:
            adr_outage = 1
            
        
        adr_lat = float(split_values[1])
        adr_lng = float(split_values[2])
        
        seconds_since_last_outage = int(split_values[3])
        
        mutexSharedADR.acquire()
        adr.put((adr_outage, adr_lat, adr_lng, seconds_since_last_outage))
        mutexSharedADR.release()
        
            

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/get_location')
def get_location():
    
    if adr.empty() == False and gps.empty() == False:
        
        mutexSharedGPS.acquire()
        [gps_lat, gps_lng] = gps.get()
        while not gps.empty():
            gps.get()
        mutexSharedGPS.release()
        
        mutexSharedADR.acquire()
        [adr_outage, adr_lat, adr_lng, seconds_since_last_outage] = adr.get()
        while not adr.empty():
            adr.get()
        mutexSharedADR.release()
        
        
        
        
        error_distance = calculate_distance(gps_lat, gps_lng, adr_lat, adr_lng)
        data = {
            'adr_outage': adr_outage,
            'gps_lat': gps_lat,
            'gps_lng': gps_lng,
            'adr_lat': adr_lat,
            'adr_lng': adr_lng,
            'seconds_since_last_outage': seconds_since_last_outage,
            'error_distance': error_distance
        }
        
        
        return jsonify(data)
    
    else:
        return jsonify({'error': 'No locations available'})

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
    
    print("over here")
    
    app.run(debug=True)
