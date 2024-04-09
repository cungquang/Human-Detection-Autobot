import boto3

def detect_humans(image_path):
    # Initialize the Rekognition client
    client = boto3.client('rekognition')

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
    humans = []
    for label in response['Labels']:
        if label['Name'] == 'Person':
            for instance in label['Instances']:
                bounding_box = instance['BoundingBox']
                humans.append({
                    'left': bounding_box['Left'],
                    'top': bounding_box['Top'],
                    'width': bounding_box['Width'],
                    'height': bounding_box['Height']
                })

    return humans

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

# Example usage
if __name__ == "__main__":
    image_path = 'path/to/your/image.jpg'
    humans = detect_humans(image_path)
    if humans:
        # Assume the image dimensions are known, replace these values with actual dimensions
        image_width = 1000
        image_height = 800
        
        min_distance = float('inf')
        for human in humans:
            distance = calculate_distance_to_center(image_width, image_height, human)
            min_distance = min(min_distance, distance)
        
        print("Distance from center to nearest human:", min_distance)
    else:
        print("No humans detected in the image.")
