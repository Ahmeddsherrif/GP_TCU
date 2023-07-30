import time
import cv2
import shutil
import base64
import socket
import threading
import os
from collections import deque

HOST = '127.0.0.1'  # The receiver's hostname or IP address
PORT = 65432        # The port used by the receiver

# Set the time that the video was captured at
current_time = time.strftime('%Y%m%d-%H%M%S')

# Prepare camera to take video (0)laptop camera , path of the recorded , name of the video if it is the same folder
cap = cv2.VideoCapture('video.mp4')

# Set height and width of video
width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))


# Remove any existing video files
if os.path.exists('basicvideo.mp4'):
    os.remove('basicvideo.mp4')

if os.path.exists('trimmedvideo.mp4'):
    os.remove('trimmedvideo.mp4')

# Prepare to write video , 30 will be changed according to video will be tested hykon kam frame per sec 3lshan ykon mazbot msh mtsr3
writer = cv2.VideoWriter('basicvideo.mp4', cv2.VideoWriter_fourcc('m', 'p', '4', 'v'), 30, (width, height))


# Flag variable to indicate when to stop recording and move the video
stop_recording = False

# Create a buffer to store the last 10 seconds of frames ,  30 will be changed according to video will be tested hykon kam frame per sec 3lshan ykon mazbot msh mtsr3 
frame_buffer = deque(maxlen=10*30)

def socket_listener():
    # Create a socket for listening to incoming data
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        while True:
            conn, addr = s.accept()
            with conn:
                data = conn.recv(1024).decode()
                x = str(data)

                #print will be commented
                print(f"Received: {x}")
                if x == 'gun detected BB done':
				    #part of photo will not be used currently bs hwa sh8al
					
                   #create a file name for the image
                    photo_filename = f'photo_{current_time}.jpg'
                    #capturing the image
                    cv2.imwrite(photo_filename, frame)
                    # Set the flag to stop recording
                    global stop_recording
                    stop_recording = True
                    
             # Open the image file in binary mode
            with open(photo_filename, 'rb') as image_file:

                 # Read the binary data from the image file
                image_data = image_file.read()

            # Encode the image data using base64 encoding
            base64_encoded_image = base64.b64encode(image_data).decode('utf-8')

            # Open a text file for writing
            with open('encoded_photo.txt', 'w') as f:

                 # Write the encoded image string to the file
                f.write(base64_encoded_image)

    # Release the camera and destroy windows
    cap.release()
    writer.release()
    cv2.destroyAllWindows()

# Start the socket listener in a separate thread 3lshan ykon sha8al f ay f2t wy receive 
listener_thread = threading.Thread(target=socket_listener)
listener_thread.start()

while True:
    # Start or restart -if it ended- the video capture
    cap = cv2.VideoCapture('video.mp4')

    # Get the frame of video captured and write it to file
    ret, frame = cap.read()
    while ret:
        writer.write(frame)
        cv2.imshow('Frame', frame)
        cv2.waitKey(1)

        # Append the frame to the buffer
        frame_buffer.append(frame)

        # Check if it's time to stop recording
        if stop_recording:
            break

        # Read the next frame
        ret, frame = cap.read()

    # Release the camera
    cap.release()

    # Check if it's time to stop recording
    if stop_recording:
        # Release the writer and destroy windows
        writer.release()
        cv2.destroyAllWindows()

        # Trim the recorded video to the last 10 seconds of frames , 30 will be changed according to video will be tested hykon kam frame per sec 3lshan ykon mazbot msh mtsr3
        trimmed_frames = list(frame_buffer)[-10*30:]
        trimmed_writer = cv2.VideoWriter('trimmedvideo.mp4', cv2.VideoWriter_fourcc('m', 'p', '4', 'v'), 30, (width, height))
        for frame in trimmed_frames:
            trimmed_writer.write(frame)
        trimmed_writer.release() 
        
        break