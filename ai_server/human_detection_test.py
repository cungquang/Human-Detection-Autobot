import boto3
import cv2
import os
from PIL import Image, ImageEnhance

AWS_ID = os.environ.get('AWS_ID')
AWS_ACCESS = os.environ.get('AWS_ACCESS')


def detect_humans(image_path):    
    # Initialize the Rekognition client
    client = boto3.client(
        'rekognition',
        aws_access_key_id=AWS_ID,
        aws_secret_access_key=AWS_ACCESS,
        region_name='us-east-1'
    )

    # Open the image file
    with open(image_path, 'rb') as image_file:
        image_bytes = image_file.read()

    # Detect labels in the image
    response = client.detect_labels(
        Image={
            'Bytes': image_bytes
        },
        MaxLabels=10,
        MinConfidence=50
    )

    # Extract bounding box coordinates of detected humans
    human = {}
    for label in response['Labels']:
        if label['Name'] == 'Person':
            for instance in label['Instances']:
                bounding_box = instance['BoundingBox']
                
                if bounding_box:
                    human['left'] = bounding_box['Left']
                    human['top'] = bounding_box['Top'] 
                    human['width'] = bounding_box['Width'] 
                    human['height'] = bounding_box['Height'] 
                
                    return human
    return None


def calculate_distance_to_center(image_path, bounding_box):
    image = cv2.imread(image_path)
    image_height, image_width, _ = image.shape
    print("image_height - " + str(image_height) + " image_width - " + str(image_width))

    # Calculate center of the image
    image_center_x = image_width / 2
    image_center_y = image_height / 2

    # Calculate center of the bounding box
    left = image_width*bounding_box['left']
    top = image_height*bounding_box['top']
    width = image_width*bounding_box['width']
    height = image_height*bounding_box['height']

    human_center_x = left + width/2
    human_center_y = top + height/2

    print("image_cetner_x ", image_center_x)
    print("image_center_y ", image_center_y)
    print("human_center_x ", human_center_x)
    print("human_center_y ", human_center_y)

    # gap on x_axis
    distance = image_center_x - human_center_x
    return distance



def test_human():
    import cv2

    # Load the pre-trained Haar Cascade classifier for human detection
    human_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_fullbody.xml')

    # Load the image
    image_path = 'temp2.jpg'
    image = cv2.imread(image_path)

    # # Convert the image to grayscale
    # gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # # Detect humans in the image
    # humans = human_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    # Detect humans in the color image
    humans = human_cascade.detectMultiScale(image, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    # Draw rectangles around the detected humans
    for (x, y, w, h) in humans:
        cv2.rectangle(image, (x, y), (x + w, y + h), (255, 0, 0), 2)

    # Display the image with detected humans
    cv2.imshow('Humans Detected', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()



def draw_bounding_box(human):
    # Draw rectangles around the detected humans
    for (x, y, w, h) in human:
        cv2.rectangle(image, (x, y), (x + w, y + h), (255, 0, 0), 2)

    # Display the image with detected humans
    cv2.imshow('Humans Detected', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

# Example usage
if __name__ == "__main__":
    raw_data_path = './received_image2.JPG'

    human = detect_humans(raw_data_path)
    if human:
        distance = calculate_distance_to_center(raw_data_path, human)
        print("distance from center on x_axis: ", distance)
    else:
        print("No human found")
