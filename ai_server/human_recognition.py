import cv2
import numpy as np

def get_human_position(distance_pixels, image_width):
    center = image_width / 2
    return distance_pixels - center

def get_image_width(image_path):
    # Load the image
    image = cv2.imread(image_path)
    
    # Get the width of the image
    image_width = image.shape[1]  # index 1 corresponds to the width
    return image_width


def detect_human(image_path):
    # Load pre-trained human detector
    human_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_fullbody.xml')

    # Load image
    image = cv2.imread(image_path)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # Detect humans in the image
    humans = human_cascade.detectMultiScale(gray, 1.1, 4)

    if len(humans) > 0:
        # Get the coordinates of the first detected human
        x, y, w, h = humans[0]

        # Calculate the center of the detected human
        human_center_x = x + w // 2
        return human_center_x
    else:
        print("No human detected in the image.")
        return None

def main():
    # Example usage
    image_path = './pictures/test1.JPG'
    image_width = get_image_width(image_path)
    human_position = detect_human(image_path)
    print("humn on x axis: " + str(human_position))
    #print(get_human_position(distance_inPixel, image_width))

main()