import cv2
import numpy as np
from matplotlib import pyplot as plt


# 图像的直方图（histogram）


def plot_demo(image):  # 这个打印的是三通道全部的像素值  这和我们想要的有点差距
    plt.hist(image.ravel(), 256, [0, 256])  # 参数：图片的数组形式，256个bin，范围（就是像素的范围）
    plt.show()


def image_hist(image):  # 这个是把三通道的像素值分别show出来
    color = ('blue', 'green', 'red')  # 创建三个色彩通道的容器
    for i, color in enumerate(color):   # 在容器里进行迭代
        hist = cv2.calcHist([image], [i], None, [256], [0, 256])  # 每次打印出一个通道的像素值
        plt.plot(hist, color)
        plt.xlim([0, 256])   # 范围
    plt.show()


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
# plot_demo(src)  # 卡的一批
image_hist(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口