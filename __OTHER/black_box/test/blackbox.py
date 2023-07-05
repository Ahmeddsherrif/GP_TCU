import cv2
from collections import deque
import time

# Input and output video file paths
input_video_path = '/home/ahmed/Desktop/GP-WorkSpace/python/WIN_20230306_12_23_29_Pro.mp4'

# Open the video file
video_capture = cv2.VideoCapture(input_video_path)


WIDTH = 640
HEIGHT = 480

RECORD_TIME_SECONDS = 3
FRAME_RATE = 30



frame_buffer = deque(maxlen=int(FRAME_RATE) * RECORD_TIME_SECONDS)

current_time = time.strftime('%Y-%m-%d_%H-%M-%S')
output_video_name = "BB_" + current_time + ".mp4"
output_codec = cv2.VideoWriter_fourcc(*'mp4v')
output_writer = cv2.VideoWriter(output_video_name, output_codec, FRAME_RATE, (WIDTH, HEIGHT))


# Read frames from the input video and store them in the deque
while True:
    # Read the next frame
    ret, frame = video_capture.read()
    
    if not ret:
        break
    
    # Store the frame in the deque
    frame_buffer.append(frame)
    
    # Display the frame (optional)
    cv2.imshow('Video', frame)
    
    # Press 'q' to stop the video playback
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video capture
video_capture.release()
cv2.destroyAllWindows()

# Write frames from the deque to the output video
for frame in frame_buffer:
    output_writer.write(frame)

# Release the output video writer
output_writer.release()
