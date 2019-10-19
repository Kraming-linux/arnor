import cv2
import numpy as np


'''
直方图均衡化 : opencv的这个是基于灰度图像的 用的话要把图像灰度处理
直方图比较
'''


'''
直方图比较：
两个相似度高的图像之间的差异计算
'''


def equalHist(image):  # 均衡化案例1  灰色图像加亮度版（对比度增强）
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    dst = cv2.equalizeHist(gray)
    cv2.imshow('equla1', dst)


def clahe_demo(image):  # 均衡化案例2  局部对比度增强（没那么亮）
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    clahe = cv2.createCLAHE(clipLimit=2, tileGridSize=(8, 8))  # 第一个参数调大点也是可以增强对比度的
    dst = clahe.apply(gray)
    cv2.imshow("clahe_demo", dst)


def create_RGB(image):  # 创建一个RGB三通道的图像 ： 拿去做直方图比较
    h, w, c = image.shape
    rgbHist = np.zeros([16*16*16, 1], np.float32)  # 三通道的bsize为16
    bsize = 256/16
    for row in range(h):
        for col in range(w):
            b = image[row, col, 0]
            g = image[row, col, 1]
            r = image[row, col, 2]
            index = np.int(b/bsize)*16*16 + np.int(g/bsize)*16 + np.int(r/bsize)  # 一定要整数
            rgbHist[np.int(index), 0] = rgbHist[np.int(index), 0] + 1
    return rgbHist


def hist_compare(image1, image2):
    hist1 = create_RGB(image1)
    hist2 = create_RGB(image2)
    match1 = cv2.compareHist(hist1, hist2, cv2.HISTCMP_BHATTACHARYYA)  # 巴氏距离比较，越小越相似
    match2 = cv2.compareHist(hist1, hist2, cv2.HISTCMP_CORREL)  # 相关性比较，越大越相似
    match3 = cv2.compareHist(hist1, hist2, cv2.HISTCMP_CHISQR)  # 卡方比较，越小越相似
    print("巴氏距离比较", match1)
    print("相关性比较", match2)
    print("卡方比较", match3)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
# equalHist(src)
# clahe_demo(src)
image1 = cv2.imread("D:/pictures/dd.jpg")
image2 = cv2.imread("D:/pictures/devil.jpg")
# cv2.imshow('image1', image1)
# cv2.imshow('image2', image2)
hist_compare(image1, image2)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
