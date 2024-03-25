import cv2

#Source: ChatGPT
def detect_human(image_path):
    human_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_fullbody.xml')
    image = cv2.imread(image_path)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    humans = human_cascade.detectMultiScale(gray, 1.1, 4)

    if len(humans) > 0:
        x, y, w, h = humans[0]
        human_center_x = x + w // 2
        return human_center_x
    else:
        #print("No human detected in the image.")
        return None

def get_gap_from_center(human_center_x, image_path):
    image = cv2.imread(image_path)
    image_width = image.shape[1]
    center = image_width / 2
    gap = human_center_x - center

    return gap

def identify_human_position(image_path):
    human_center_x = detect_human(image_path)

    #If found human
    if human_center_x is not None:
        #Return position: left -> negative; center -> 0; right -> positive
        return get_gap_from_center(human_center_x, image_path)
    
    else:
        #No human found OR unable to identify human
        return None

def main():
    # Example usage
    image_path = './pictures/test2.JPG'
    human_position = identify_human_position(image_path)
    print(human_position)

if __name__ == "__main__":
    main()