import cv2
import numpy as np

'''
图像的形态学操作：
处理灰度和二值图像 彩色图像也是可以的
膨胀与腐蚀
注：数学要学好啊。。。
'''


def erode_demo(image):  # 腐蚀
    print(image.shape)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    cv2.imshow("thresh", thresh)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))  # 结构元素（矩形）3*3的kernel
    erodeImage = cv2.erode(thresh, kernel)  # 基于二值图像
    cv2.imshow("erode_demo", erodeImage)


def dilate_demo(image):  # 膨胀
    print(image.shape)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    cv2.imshow("thresh", thresh)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))  # 结构元素（矩形）3*3的kernel
    dilateImage = cv2.dilate(thresh, kernel)  # 基于二值图像
    cv2.imshow("dilateImage", dilateImage)


def color_demo(image):  # 彩图的腐蚀操作 膨胀的结构也是相同的
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))  # 结构元素（矩形）3*3的kernel
    erode = cv2.erode(image, kernel)
    cv2.imshow("color_erode", erode)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
erode_demo(src)
dilate_demo(src)
color_demo(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口

