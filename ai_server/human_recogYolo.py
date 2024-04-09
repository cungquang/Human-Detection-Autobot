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

    # Check if 'Person' label is detected
    for label in response['Labels']:
        if label['Name'] == 'Person':
            return True
    
    return False

# Example usage
if __name__ == "__main__":
    image_path = 'path/to/your/image.jpg'
    if detect_humans(image_path):
        print("Human detected in the image.")
    else:
        print("No humans detected in the image.")
