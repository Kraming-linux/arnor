import cv2
import numpy as np

# 色彩空间排名：RGB HSV HIS YCrCb YUV  这排名不是瞎排的 RGB肯定是老大
# RGB: 立方体色彩结构
# HSV： 如果你获取的特征对象颜色很突出（比如红色，绿色的物体对象）转到HSV肯定可以找到
# HIS: I就是色彩强度，S就是灰度级别
# YCrCb： 早期作肤色检查用的
# YUV：Linux系统色彩空间就是这个（安卓摄像机读取的原始格式也是YUV，只不过中间进行色彩转换了）


def color_space_demo(image):    # 色彩空间转换函数
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 还是灰度处理
    cv2.imshow("gray", gray)
    hsv = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)  # 转换色彩空间为HSV格式
    cv2.imshow("HSV", hsv)
    yuv = cv2.cvtColor(image, cv2.COLOR_RGB2YUV)  # 转换色彩空间为YUV格式
    cv2.imshow("YUV", yuv)
    ycrcb = cv2.cvtColor(image, cv2.COLOR_RGB2YCrCb)  # 转换色彩空间为YCRCB格式
    cv2.imshow("YCrCb", ycrcb)


img = cv2.imread("D:/pictures/sunny.jpg")  # 引用一下class one的图
cv2.imshow('RGB', img)
color_space_demo(img)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
