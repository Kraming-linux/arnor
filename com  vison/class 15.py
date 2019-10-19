import cv2
import numpy as np


'''
图像二值化： 全局阈值；局部阈值
二值图像：只有黑和白
'''


'''
threshold_demo~`demo5 都是全局阈值
'''


def threshold_demo(image):   # OTSU 方法
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 二值化要先转灰度
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)  # 你没有看错
    print("threshoud:", ret)  # 图像的阈值    那条 | 后面是自动计算阈值 如果你改动0或255的值要把 | 后面删掉
    cv2.imshow('binary', binary)  # 图像


def threshold_demo1(image):   # TRIANGLE 方法： 直方图只有一个波峰的时候这个特别好
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 二值化要先转灰度
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_TRIANGLE)  # 你没有看错
    print("threshoud1:", ret)  # 图像的阈值
    cv2.imshow('binary', binary)  # 图像


def threshold_demo2(image):   # 自定义阈值方法
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 二值化要先转灰度
    ret, binary = cv2.threshold(gray, 127, 255, cv2.THRESH_BINARY)  # 效果没那么好
    print("threshoud2:", ret)  # 图像的阈值
    cv2.imshow('binary', binary)  # 图像


def threshold_demo3(image):  # INV 方法
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 二值化要先转灰度
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV)  # 黑白像素值对调
    print("threshoud1:", ret)  # 图像的阈值
    cv2.imshow('binary', binary)  # 图像


def threshold_demo4(image):  # TRUNC 方法
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 二值化要先转灰度
    ret, binary = cv2.threshold(gray, 127, 255, cv2.THRESH_TRUNC)  # 大于阈值127的为127，小于的变黑
    print("threshoud1:", ret)  # 图像的阈值
    cv2.imshow('binary', binary)  # 图像


def threshold_demo5(image):  # ZERO 方法
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 二值化要先转灰度
    ret, binary = cv2.threshold(gray, 127, 255, cv2.THRESH_TOZERO)  # 小于127的全部变0  这个也可以用THRESH_TOZERO_INV
    print("threshoud1:", ret)  # 图像的阈值
    cv2.imshow('binary', binary)  # 图像


def local_threshold(image):  # 普通版局部阈值处理
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    DST = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 25, 10)  # 自适应阈值
    cv2.imshow('local_1', DST)      # 后面两个参数为bloksize（必须奇数）， 常量C（就是大于C的变白）


def gassusia_threshold():  # 精确版局部阈值处理
    dst = cv2.imread("D:/pictures/text_defocus.jpg")  # 换张图
    cv2.imshow("text_defocus", dst)
    gray = cv2.cvtColor(dst, cv2.COLOR_BGR2GRAY)
    binary = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 25, 0.3)
    cv2.imshow('brightness', binary)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
threshold_demo(src)
local_threshold(image=src)
gassusia_threshold()
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
