import cv2
import numpy as np


'''
图像的开闭操作：
开操作：去除小的干扰块
闭操作：填充封闭区域
'''


def open_demo(image):  # 开操作：可以去直线，去干扰，屌的一批
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    cv2.imshow("binary", binary)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))  # 这个操作需要结构元素（这里用矩形）最后的参数可以调
    open = cv2.morphologyEx(binary, cv2.MORPH_OPEN, kernel)  # 图像开闭操作取决于你中间的参数
    cv2.imshow("open_demo", open)


def close_demo(image):  # 闭操作：感觉没啥屌用。。
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    cv2.imshow("binary", binary)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))  # 这个操作需要结构元素（这里用矩形）
    close = cv2.morphologyEx(binary, cv2.MORPH_CLOSE, kernel)
    cv2.imshow("close_demo", close)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
# open_demo(src)
close_demo(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
