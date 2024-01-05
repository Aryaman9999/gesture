import cv2
import mediapipe as mp
import numpy as np
import time
import serial  # Import the serial library for communication with Arduino

# Initialize the MediaPipe Hand module
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()

# Initialize OpenCV's VideoCapture
cap = cv2.VideoCapture(0)

# Define constants for finger landmarks
THUMB_TIP = 4
INDEX_FINGER_TIP = 8
MIDDLE_FINGER_TIP = 12
RING_FINGER_TIP = 16
LITTLE_FINGER_TIP = 20

# Define finger thresholds for counting
THUMB_THRESHOLD = 0.5  # Adjust as needed
INDEX_THRESHOLD = 0.7  # Adjust as needed
MIDDLE_THRESHOLD = 0.7  # Adjust as needed
RING_THRESHOLD = 0.7  # Adjust as needed
LITTLE_THRESHOLD = 0.7  # Adjust as needed

# Initialize variables for timing
previous_time = time.time()
current_time = time.time()
previous_gesture = ''

# Initialize the serial connection to the Arduino
arduino = serial.Serial('COM15', 9600)  # Replace 'COM3' with your Arduino's serial port

while True:
    ret, frame = cap.read()

    # Convert the frame to RGB format for hand detection
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    results = hands.process(frame_rgb)

    if results.multi_hand_landmarks:
        for landmarks in results.multi_hand_landmarks:
     
            hand_landmarks = np.array([[landmark.x, landmark.y] for landmark in landmarks.landmark])

            thumb_tip = hand_landmarks[THUMB_TIP][1]
            index_finger_tip = hand_landmarks[INDEX_FINGER_TIP][1]
            middle_finger_tip = hand_landmarks[MIDDLE_FINGER_TIP][1]
            ring_finger_tip = hand_landmarks[RING_FINGER_TIP][1]
            little_finger_tip = hand_landmarks[LITTLE_FINGER_TIP][1]

           
            finger_count = 0
            if thumb_tip < THUMB_THRESHOLD:
                finger_count += 1
            if index_finger_tip < INDEX_THRESHOLD:
                finger_count += 1
            if middle_finger_tip < MIDDLE_THRESHOLD:
                finger_count += 1
            if ring_finger_tip < RING_THRESHOLD:
                finger_count += 1
            if little_finger_tip < LITTLE_THRESHOLD:
                finger_count += 1

           
            if finger_count == 1:
                gesture = 'f'
            elif finger_count == 2:
                gesture = 'b'
            elif finger_count == 3:
                gesture = 'r'
            elif finger_count == 4:
                gesture = 'l'
            else:
                gesture = 'Unknown'

            
            current_time = time.time()

           
            if gesture != previous_gesture:
                arduino.write(gesture.encode())  # Send the character to the Arduino
                previous_gesture = gesture

                # Add a delay to ensure Arduino has time to process the input
                time.sleep(1)

            # Display the previous gesture during the meantime
            if current_time - previous_time < 5:
                cv2.putText(frame, f'Gesture: {previous_gesture}', (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

            # Store the current time
            previous_time = current_time

            mp_drawing = mp.solutions.drawing_utils
            mp_drawing.draw_landmarks(frame, landmarks, mp_hands.HAND_CONNECTIONS)

    
    cv2.imshow('Finger Count', frame)

    # Exit the program when 'q' is pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the camera, close the OpenCV window, and close the serial connection
cap.release()
cv2.destroyAllWindows()
arduino.close()
