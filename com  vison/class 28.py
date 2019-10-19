import cv2


'''
HARR and LBP 人脸检测
'''


def face_detect(image):
    # 灰度处理
    gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    # 加载人脸检测数据集
    faceData = cv2.CascadeClassifier("D:\opencv-4.1.0/data/lbpcascades/lbpcascade_frontalface.xml")
    # 在多尺度空间进行人脸检测：图片，尺度变化，检测低值（写小点，2,3,4）
    face = faceData.detectMultiScale(gray, 1.02, 15)
    for x, y, w, h in face:  # 把每个脸都迭代生成矩形框,线宽一定要整数
        cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 255), thickness=1)
    cv2.imshow("result", image)


def video_detect():
    while True:
        capture = cv2.VideoCapture(0)
        ret, frame = capture.read()
        frame = cv2.flip(frame, 1)  # 隔帧处理
        face_detect(frame)
        c = cv2.waitKey(0)
        if c == 27:  # 等待用户触发按键
            break


def face1_detect(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    faceData = cv2.CascadeClassifier("D:/opencv-4.1.0/data/haarcascades/haarcascade_frontalface_alt_tree.xml")
    face = faceData.detectMultiScale(gray, 1.02, 2)
    for x, y, w, h in face:
        cv2.rectangle(image, (x, y), (x + w, y + h), (255, 0, 0), thickness=1)
    cv2.imshow("face2", image)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
cv2.namedWindow("result", cv2.WINDOW_AUTOSIZE)
face_detect(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口


