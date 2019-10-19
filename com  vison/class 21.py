import cv2
import numpy as np

'''
圆的检测：还是霍夫变换
'''

'''
先上个边缘保留滤波（EPF）
然后再搞：霍夫圆检测对噪声很敏感
'''


def hough_circle(image):
    dst = cv2.GaussianBlur(image, (3, 3), 0)  # 效果更好，但是很灵敏
    # dst = cv2.pyrMeanShiftFiltering(image, 10, 100)  # EPF降噪
    cimage = cv2.cvtColor(dst, cv2.COLOR_BGR2GRAY)
    circles = cv2.HoughCircles(cimage, cv2.HOUGH_GRADIENT, 1, 20, param1=50, param2=30, minRadius=0, maxRadius=0)  # 参数：图片，方法（唯一，图像梯度），步长，最小距离（处理同心圆）
    cricles = np.uint16(np.array(circles))                                            # circle（先暂定不填）param1，param2，最小圆半径，最大圆半径
    for i in circles[0, :]:
        cv2.circle(image, (i[0], i[1]), i[2], (0, 0, 255), 2)  # 画圆
        cv2.circle(image, (i[0], i[1]), 2, (0, 255, 255), 2)  # 画圆心 r为2个像素点
    cv2.imshow("circle", image)


src = cv2.imread("D:/pictures/stuff.jpg")
cv2.imshow('src', src)
hough_circle(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
