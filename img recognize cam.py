import cv2
import requests
import time

#NodeMCU IP
McuIp = "192.168.8.109:8080"

#EPS32 URL
EspURL="http://192.168.1.184:81/stream"

def imgRec(EspURL):
    # Create a VideoCapture object
    video_capture = cv2.VideoCapture(EspURL)

    # Check if the camera is opened successfully
    if not video_capture.isOpened():
        print("Error opening video stream")
        exit()

    while True:
        # Capture frame-by-frame
        ret, frame = video_capture.read()

        # If the frame is not read correctly, break
        if not ret:
            break

        # Get the width and height of the frame
        width = frame.shape[1]
        height = frame.shape[0]
        #print(width)
        #print(height)

        # Image item recognition
        from ultralytics import YOLO
        model = YOLO("yolov8m.pt")
        results = model.predict(frame)
        result = results[0]

        # Iterate over the boxes
        for box in result.boxes:
            # Get the class ID
            class_id = result.names[box.cls[0].item()]

            # Get the coordinates of the box
            cords = box.xyxy[0].tolist()
            cords = [round(x) for x in cords]
            itemList=["bottle"]

            if class_id in itemList:

                # Get the confidence score
                #conf = round(box.conf[0].item(), 2)

                # center Dot
                centerX1=width/2
                centerY2=height/2

                #width and height of the item
                widthItem= cords[2]-cords[0]
                heightItem=cords[3]-cords[1]
                #print("Hieght of the item = ",heightItem)
                #print("Width of the item = ",widthItem)
                #print("-------")


                #catching the distance between center dot and item center
                itemCenter=((widthItem/2) +cords[0]) - centerX1;   
                print("Distnace Between Center and the Item Cnter: ",itemCenter)
                #print("-------")

                # Draw the box around the object
                cv2.rectangle(frame, (cords[0], cords[1]), (cords[2], cords[3]), (0, 255, 0), 2)

        # Display the frame
        cv2.imshow("Frame", frame)
        
        #Start Time
        start_time = time.time()
        #closing the view after 5 seconds
        if (time.time() - start_time >= 5):
            break
    
    # When everything is done, release the capture
    video_capture.release()
    cv2.destroyAllWindows()
    return itemCenter


def send_value(McuIp, variable_name, variable_value):
    url = f"http://{McuIp}/?{variable_name}={variable_value}"

#nodeMCU URl        
url = f"http://{McuIp}/"
while True:
    try:
        response = requests.get(url)
        #checking laptop gets any request from nodeMCU
        while (True):
            if (response.status_code != 200):
                print("Not Yet Requested-----!")
            else:
                distance=imgRec(EspURL)
                send_value(McuIp,"dist",distance)
                break

    except requests.exceptions.RequestException as e:
        print("Waiting For Signal-----! \n")