import cv2
import time

# Open the video file
video_path = '/home/ahmed/Desktop/GP-WorkSpace/python/WIN_20230306_12_23_29_Pro.mp4'
cap = cv2.VideoCapture(video_path)

# Check if the video file was successfully opened
if not cap.isOpened():
    print("Error opening video file")
    exit()

# Read and display frames until the video ends
while True:
    # Read a frame from the video file
    ret, frame = None #cap.read()

    # If the frame was not successfully read, then the video has ended
    if not ret:
        break

    # Display the frame in a window called "Video"
    cv2.imshow('Video', frame)

    # Wait for a key press and check if the user wants to exit
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    
    time.sleep(0.25)
    
# Release the video file and close the window
cap.release()
cv2.destroyAllWindows()
