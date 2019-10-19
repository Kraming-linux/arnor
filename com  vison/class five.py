import cv2
import numpy as np

# 对像素进行加减乘除 （合成一张有意思的图片）

# D:/opencv-4.1.0/samples/data # opencv 激情提供一些大小和规格相同的图片
# 这个案例要规格相同（既长，宽，通道数相同）的图片才能玩
# 还是把这两张图片放到 D:/pictures下


def add_demo(m1, m2):  # 加法
    dst = cv2.add(m1, m2)
    cv2.imshow("demo1", dst)


def subtract_demo(m1, m2):  # 减法
    dst = cv2.subtract(m1, m2)
    cv2.imshow("demo2", dst)


def multiply_demo(m1, m2):  # 乘法
    dst = cv2.multiply(m1, m2)
    cv2.imshow("demo3", dst)


def divide_demo(m1, m2):  # 除法
    dst = cv2.divide(m1, m2)
    cv2.imshow("demo4", dst)


def means(m1, m2):  # 求图像的均值（用mean方法）
    print(cv2.mean(m1))

    print(cv2.mean(m2))


def meanStdDev(m1, m2):  # 求图像的均值和方差 黑白照的方差值会偏大一点
    M1, dev1 = cv2.meanStdDev(m1)
    M2, dev2 = cv2.meanStdDev(m2)

    print("均值1：", M1)  # 均值 是个二维数组，三通道就是三行
    print("方差1：", dev1)  # 方差 数组 也是三行

    print("均值2：", M2)
    print("方差2：", dev2)


src1 = cv2.imread("D:/pictures/LinuxLogo.jpg")  # 遮罩层图片 这张只有黑白两色
src2 = cv2.imread("D:/pictures/WindowsLogo.jpg")
print(src1.shape)  # 证明这两张图片规格相同
print(src2.shape)

cv2.imshow('src1', src1)
cv2.imshow('src2', src2)
add_demo(src1, src2)
subtract_demo(src1, src2)
multiply_demo(src1,src2)
divide_demo(src1, src2)
means(src1, src2)
meanStdDev(src1, src2)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()