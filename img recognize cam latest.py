import cv2
from ultralytics import YOLO
import time
from flask import Flask, Response


# NodeMCU IP
#McuIp = "192.168.1.5"

# ESP32 URL
EspURL = "http://192.168.1.184:81/stream"

# PythonCode IP
pcIP = '192.168.8.5'

app = Flask(__name__)

# Item list for recognition
itemList = ["bottle"]

    
def process_request():
    # Create a VideoCapture object
    video_capture = cv2.VideoCapture(EspURL)

    # Check if the camera is opened successfully
    if not video_capture.isOpened():
        return "Error opening video stream"
    start=time.time()
    while (time.time()-start)<5:
        # Capture frame-by-frame
        ret, frame = video_capture.read()

        # If the frame is not read correctly, break
        if not ret:
            break

        # Get the width and height of the frame
        width = frame.shape[1]
        height = frame.shape[0]

        # Image item recognition
        model = YOLO("yolov8m.pt")
        results = model.predict(frame)
        result = results[0]

        itemCenter = None

        # Iterate over the boxes
        for box in result.boxes:
            # Get the class ID
            class_id = result.names[box.cls[0].item()]

            # Get the coordinates of the box
            cords = box.xyxy[0].tolist()
            cords = [round(x) for x in cords]

            if class_id in itemList:
                # center Dot
                centerX1 = width / 2
                centerY2 = height / 2

                # Width and height of the item
                widthItem = cords[2] - cords[0]
                heightItem = cords[3] - cords[1]

                # Catching the distance between center dot and item center
                itemCenter = ((widthItem / 2) + cords[0]) - centerX1
                print("Distance Between Center and the Item Center:", itemCenter)
                

                # Draw the box around the object
                cv2.rectangle(frame, (cords[0], cords[1]), (cords[2], cords[3]), (0, 255, 0), 2)
        # Display the frame
        #start_atime=time.time()
        #while(time.time()-start_time<5):
        #    cv2.imshow("Frame", frame)
        #    cv2.waitKey(1)

    # When everything is done, release the capture
        #video_capture.release()
        #cv2.destroyAllWindows()
                return str(itemCenter)

@app.route('/')
def stream_video():
    return Response(process_request(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='192.168.8.5', port=5000)
