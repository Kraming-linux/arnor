import cv2
import numpy as np

'''
# D:/opencv-4.1.0/samples/data   # opencv 激情提供一些大小和规格相同的图片
# 这个案例要规格相同（既长，宽，通道数相同）的图片才能玩
# 还是把这两张图片放到 D:/pictures下
'''


def logic_demo(m1, m2):  # 像素逻辑运算(逻辑与运算）1与0=0，1与1=1，0与0=0
    dst = cv2.bitwise_and(m1, m2)
    cv2.imshow("logic", dst)


def logic_demo2(m1, m2):  # 像素或运算 1或0=1，0或0=0，1或1=0
    dst = cv2.bitwise_or(m1, m2)
    cv2.imshow("logic2", dst)


def logic_not(m1):  # 像素非运算 0和1互换  只针对一张图片
    m = cv2.imread("D:/pictures/LinuxLogo.jpg")
    cv2.imshow("yes", m)
    dst = cv2.bitwise_not(m1)
    cv2.imshow("not", dst)


def brightness(image, c, b):  # 调节亮度 参数为图片，对比度，亮度
    h, w, ch = image.shape
    blank = np.zeros([h, w, ch], image.dtype)  # 合成一张与参数图片相同的全黑图像
    ds = cv2.addWeighted(image, c, blank, 1-c, b)   # 用这个函数 图像加权
    cv2.imshow("bright", ds)


src1 = cv2.imread("D:/pictures/LinuxLogo.jpg")  # 遮罩层图片 这张只有黑白两色
src2 = cv2.imread("D:/pictures/WindowsLogo.jpg")
src = cv2.imread("D:/pictures/sunny.jpg")
brightness(src, 1.5, 10)
cv2.imshow("src", src)
cv2.imshow('src1', src1)
cv2.imshow('src2', src2)
logic_demo(src1, src2)
logic_demo2(src1, src2)
logic_not(src1)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()