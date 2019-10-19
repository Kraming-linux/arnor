import cv2
import numpy as np


'''
轮廓发现：
基于边缘提取 : 二值图像
选取的阈值会影响最终的结果
'''


def countour_demo(image):
    dst = cv2.GaussianBlur(image, (3, 3), 0)  # 高斯去噪
    gray = cv2.cvtColor(dst, cv2.COLOR_BGR2GRAY)  # 灰度处理
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)  # 二值化
    cv2.imshow("binary", binary)
    contours, herichy = cv2.findContours(binary, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)  # 新版本只有两个返回值
    for i, contour in enumerate(contours):
        cv2.drawContours(image, contours, i, (127, 127, 127), 2)  # 对的，就是这样 :pink颜色
        print(i)
    cv2.imshow("detect_countour", image)


def canny_contour(image):  # canny边缘检测法
    blur = cv2.GaussianBlur(image, (3, 3), 0)
    gray = cv2.cvtColor(blur, cv2.COLOR_BGR2GRAY)
    # xgrad = cv2.Sobel(gray, cv2.CV_16SC1, 1, 0)
    # ygrad = cv2.Sobel(gray, cv2.CV_16SC1, 0, 1)
    edge = cv2.Canny(gray, 20, 180)
    cv2.imshow("edge", edge)
    binary = edge
    contours, herichy = cv2.findContours(binary, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    for i, contour in enumerate(contours):
        cv2.drawContours(image, contours, i, (0, 0, 255), 2)
    cv2.imshow('_contour_', image)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
# countour_demo(src)
canny_contour(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口

