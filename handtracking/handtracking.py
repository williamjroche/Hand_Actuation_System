# Hand Tracking and gesture recognition by William Roche
# GitHub: williamjroche
# if anything is unclear, reach out to me on linkedin (link on GitHub profile)

#if you don't want to scroll:
# Current recognized gestures: rock n roll (with thumb extended), point index, peace, open hand, fist, pinch (index and thumb only), F.U. (middle finger)
# make sure hand_landmarker.task is in the same folder as this file and the path is correct in line 44
# press q to exit

import cv2
import mediapipe as mp
import time
import serial

#connect to arduino
try:
    arduino = serial.Serial('COM7', 9600, timeout=1) #COMx needs to be changed to your com port arduino is connected to
    time.sleep(2)
    print("Arduino Connected!")

    if arduino.in_waiting:
        print(arduino.readline().decode().strip())
except Exception as e:
    arduino = None
    print(f"Arduino not connected: {e}") #error if arduino cant connect

#global value used for HandLandmarkerResult
latest_result = None

#use default webcam
capture = cv2.VideoCapture(0)

BaseOptions = mp.tasks.BaseOptions
HandLandmarker = mp.tasks.vision.HandLandmarker
HandLandmarkerOptions = mp.tasks.vision.HandLandmarkerOptions
HandLandmarkerResult = mp.tasks.vision.HandLandmarkerResult
VisionRunningMode = mp.tasks.vision.RunningMode

def print_result(result: HandLandmarkerResult, output_image: mp.Image, timestamp_ms: int):
    global latest_result
    latest_result = result
    print('hand landmarker result: {}'.format(result))

options = HandLandmarkerOptions(base_options=BaseOptions(model_asset_path = "D:/personal/robotic hand project/hand_landmarker.task"), running_mode=VisionRunningMode.LIVE_STREAM, result_callback=print_result)

HAND_CONNECTIONS = [
    (0, 1), (1, 2), (2, 3), (3, 4),        #thumb
    (0, 5), (5, 6), (6, 7), (7, 8),        #index finger
    (0, 9), (9,10), (10,11), (11,12),      #middle finger
    (0,13), (13,14), (14,15), (15,16),     #ring finger
    (0,17), (17,18), (18,19), (19,20)      #pinky finger
]

#Note: I am using python 3.13 so Im using the latest mediapipe, which has changed how hand landmarks are drawn, so I am using cv2 to draw the landmarks and connecting lines (easier)
def draw_landmarks(image, result):
    if result is None or not result.hand_landmarks: #if mediapipe hasn't generated results yet
        return image

    h, w, _ = image.shape

    for hand in result.hand_landmarks:
        points = []

        #draw landmarks (joints)
        for lm in hand:
            x = int(lm.x * w)
            y = int(lm.y * h)
            points.append((x, y))
            cv2.circle(image, (x, y), 5, (0, 255, 0), -1)

        #draw connections (bones)
        for start, end in HAND_CONNECTIONS:
            cv2.line(image, points[start], points[end], (255, 0, 0), 2)

    return image

def get_hand_bounding_box(hand_landmarks, image_shape, padding=20):
    h, w, _ = image_shape
    #convert landmarks to pixle coords
    xs = [int(lm.x*w) for lm in hand_landmarks]
    ys = [int(lm.y*h) for lm in hand_landmarks]

    #find min and max for x and y
    min_x = max(min(xs)-padding, 0)
    min_y = max(min(ys)-padding, 0)
    max_x = min(max(xs) + padding, w)
    max_y = min(max(ys) + padding, h)

    return min_x, min_y, max_x, max_y

def draw_hand_bounding_box(image, hand_landmarks, gesture_label):
    x1, y1, x2, y2 = get_hand_bounding_box(hand_landmarks, image.shape)
    #bounding box
    cv2.rectangle(image, (x1,y1), (x2,y2), (255, 255, 255), 2)

    gesture_label = classify_gesture(hand_landmarks)

    if gesture_label: #if gesture detected
        text = gesture_label
        font = cv2.FONT_HERSHEY_SIMPLEX
        scale = 0.7
        thickness = 2

        (tw, th), _ = cv2.getTextSize(text, font, scale, thickness)
        #background rectangle for text
        cv2.rectangle(image, (x1, y1-th-10), (x1+tw+6, y1), (255,255,255), -1)
        #text
        cv2.putText(image, text, (x1+3, y1-5), font, scale, (0,0,0), thickness, cv2.LINE_AA)


GESTURE_STABILITY_FRAMES = 10

#detects if finger is extended
def finger_extended(hand, tip, pip):
    if tip == 4:
        #abs for left and right hand
        wrist_x = hand[0].x
        thumb_x = hand[tip].x
        #thumb extended horizontally
        return abs(thumb_x-wrist_x) > abs(hand[pip].x - wrist_x)
    else:
        #fingers extended vertically
        return hand[tip].y < hand[pip].y

def classify_gesture(hand):
    thumb = finger_extended(hand, 4, 3)
    index = finger_extended(hand, 8, 6)
    middle = finger_extended(hand, 12, 10)
    ring = finger_extended(hand, 16, 14)
    pinky = finger_extended(hand, 20, 18)

    #Relaxed open hand
    if thumb and index and middle and ring and pinky:
        return "OPEN"
    #closed fist
    elif not thumb and not index and not middle and not ring and not pinky:
        return "CLOSED"
    #point index
    elif not thumb and index and not middle and not ring and not pinky:
        return "POINT"
    #pinch (thumb and index)
    elif not thumb and not index and middle and ring and pinky:
        return "PINCH"
    #peace sign
    elif not thumb and index and middle and not ring and not pinky:
        return "PEACE"
    #rock n roll
    elif thumb and index and not middle and not ring and pinky:
        return "ROCK"
    #F.U.
    elif not thumb and not index and middle and not ring and not pinky:
        return "F.U."
    else:
        return "Unknown"

gesture_buffer = []
current_gesture = None

def update_gesture(new_gesture):
    global gesture_buffer, current_gesture

    gesture_buffer.append(new_gesture)
    if len(gesture_buffer) > GESTURE_STABILITY_FRAMES:
        gesture_buffer.pop(0) #if length of gesture buffer is greater than stability, remove first in buffer

    if gesture_buffer.count(new_gesture) == GESTURE_STABILITY_FRAMES:
        if new_gesture != current_gesture:
            current_gesture = new_gesture
            return new_gesture

    return None

with HandLandmarker.create_from_options(options) as landmarker:
    while True:
        #capture webcame image
        success, img = capture.read()
        img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=img_rgb)

        timestamp_ms = int(time.time()*1000)
        landmarker.detect_async(mp_image, timestamp_ms)

        #show webcam image with overlay
        
        img = draw_landmarks(img, latest_result)
        if latest_result and latest_result.hand_landmarks:
            for hand_landmarks in latest_result.hand_landmarks:
                gesture_label = classify_gesture(hand_landmarks)

                #update gesture with stability check
                stable_gesture = update_gesture(gesture_label)
                
                if stable_gesture and arduino: #if stable gesture is updated
                    try:
                        arduino.write(f"{stable_gesture}\n".encode()) #send gesture to arduino
                        print(f"Sent to arduino: {stable_gesture}") #print so user knows
                    except Exception as e:
                        print(f"Error sending gesture: {e}") #print exception if there is error

                draw_hand_bounding_box(img, hand_landmarks, current_gesture)

        cv2.imshow("Hand recognition", img)
        if cv2.waitKey(1) & 0xFF == ord('q'): #q key to exit
            break

capture.release()
cv2.destroyAllWindows()