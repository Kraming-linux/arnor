import cv2
import numpy as np
from matplotlib import pyplot as plt

'''
canny 边缘提取算法:
1,高斯模糊降噪
灰度转化
求取图像的梯度
非最大信号抑制 
高低阈值输出二值图像  T1低阈值，T2高阈值，比T1低的舍去，T2高的保留，1与2直接连接的保留
'''


def canny_demo(image):
    bure = cv2.GaussianBlur(image, (3, 3), 0)
    gray = cv2.cvtColor(bure, cv2.COLOR_BGR2GRAY)
    gradx = cv2.Sobel(gray, cv2.CV_16SC1, 1, 0)
    grady = cv2.Sobel(gray, cv2.CV_16SC1, 0, 1)
    edge = cv2.Canny(gradx, grady, 50, 150)
    cv2.imshow("edge", edge)
    dst = cv2.bitwise_and(image, image, mask=edge)
    cv2.imshow("color_edge", dst)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
canny_demo(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口