import cv2
import numpy as np
import tensorflow as tf

from collections import deque



VIDEO_PATH = '/home/ahmed/Desktop/GP-WorkSpace/python/WIN_20230306_12_23_29_Pro.mp4'
MODEL_PATH = '/home/ahmed/Desktop/GP-WorkSpace/python/tf_model_sign_new.tflite'

TRIGGER_THRESHOLD = 0.6
TRIGGER_CONSECUTIVE_FRAMES = 5

buffer = deque(maxlen=TRIGGER_CONSECUTIVE_FRAMES)  # Set the maximum size of the circular buffer to 5



# Load the video
cap = cv2.VideoCapture(VIDEO_PATH)

# Load the TensorFlow Lite model
interpreter = tf.lite.Interpreter(MODEL_PATH)
interpreter.allocate_tensors()

# Get the input and output tensors
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

while(cap.isOpened()):
    # Read the frame
    ret, frame = cap.read()
    if not ret:
        break
    
    # Preprocess the frame
    #processed_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert to RGB
    processed_frame=frame
    processed_frame = cv2.resize(processed_frame, (224, 224))  # Resize to input shape
    processed_frame = np.expand_dims(processed_frame, axis=0)  # Add batch dimension
    processed_frame = processed_frame.astype(np.float32)  # Convert to float32
    processed_frame = processed_frame / 255.0  # Normalize to [0, 1]
    
    # Set the input tensor
    interpreter.set_tensor(input_details[0]['index'], processed_frame)
    
    # Run the model inference
    interpreter.invoke()
    
    # Get the output tensor
    output_data = interpreter.get_tensor(output_details[0]['index'])
    
    if output_data[0][1] > TRIGGER_THRESHOLD:
        buffer.append(1)

    else:
        buffer.append(0)
    

    if sum(buffer) == TRIGGER_CONSECUTIVE_FRAMES:
        print("SIGNNNNNN")
    
    cv2.imshow('frame', frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
