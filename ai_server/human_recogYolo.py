import boto3
import cv2
from PIL import Image, ImageEnhance

def detect_humans(image_path):
    ACCESS_ID = 'AKIAV6ZBKG62AUZFQM6Z'
    SECRET_KEY = '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' 
    
    # Initialize the Rekognition client
    client = boto3.client(
        'rekognition',
        aws_access_key_id=ACCESS_ID,
        aws_secret_access_key=SECRET_KEY
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
    human = []
    for label in response['Labels']:
        if label['Name'] == 'Person':
            for instance in label['Instances']:
                bounding_box = instance['BoundingBox']
                human.append({
                    'left': bounding_box['Left'],
                    'top': bounding_box['Top'],
                    'width': bounding_box['Width'],
                    'height': bounding_box['Height']
                })
                return human

    return None

def calculate_distance_to_center(image_width, image_height, bounding_box):
    # Calculate center of the image
    image_center_x = image_width / 2
    image_center_y = image_height / 2

    # Calculate center of the bounding box
    bounding_box_center_x = bounding_box['left'] + (bounding_box['width'] / 2)
    bounding_box_center_y = bounding_box['top'] + (bounding_box['height'] / 2)

    # Calculate distance between image center and bounding box center
    distance = ((image_center_x - bounding_box_center_x) ** 2 + (image_center_y - bounding_box_center_y) ** 2) ** 0.5
    return distance

def enhance_image(image_path, output_path, brightness=1.2, contrast=1.2, sharpness=1.2):
    # Open the image file
    image = Image.open(image_path)

    # Enhance brightness, contrast, and sharpness
    enhancer = ImageEnhance.Brightness(image)
    image = enhancer.enhance(brightness)
    enhancer = ImageEnhance.Contrast(image)
    image = enhancer.enhance(contrast)
    enhancer = ImageEnhance.Sharpness(image)
    image = enhancer.enhance(sharpness)

    # Save the enhanced image
    image.save(output_path)


def enhance_image_2(input_image_path, output_image_path):
    # Read the image
    image = cv2.imread(input_image_path)

    # Convert the image to grayscale
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Reduce noise using a Gaussian blur
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Enhance contrast using histogram equalization
    enhanced = cv2.equalizeHist(blurred)

    # Increase sharpness using unsharp masking
    unsharp = cv2.addWeighted(enhanced, 1.5, blurred, -0.5, 0)

    # Save the enhanced image
    cv2.imwrite(output_image_path, unsharp)


# Example usage
if __name__ == "__main__":
    raw_data_path = './received_image.JPG'
    # image_path = './enhanced_image.jpg'

    # enhance_image_2(raw_data_path, image_path)

    
    human = detect_humans(raw_data_path)
    if human:
        # Assume the image dimensions are known, replace these values with actual dimensions
        image_width = 1000
        image_height = 800
        
        min_distance = float('inf')
        distance = calculate_distance_to_center(image_width, image_height, human)
        min_distance = min(min_distance, distance)
        
        print("Distance from center to nearest human:", min_distance)
    else:
        print("No humans detected in the image.")
