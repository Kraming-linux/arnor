import cv2
import numpy as np


'''
 高斯模糊 ： 效果比之前的更好
 普及一下高斯的概率分布（看起来像正态分布的图片）
 G（x）= 1 / （sqrt(2*pi)*sigam) * e^(-pi^2/2*sigam^2)  : 一维方面的高斯概率分布
'''


def clarm(pv):  # 这个函数确保像素值在0到255之内
    if pv > 255:
        return 255
    if pv < 0:
        return 0
    else:
        return pv


def gaussian_noise(image):
    h, w, c = image.shape
    for row in range(h):
        for col in range(w):
            s = np.random.normal(0, 20, 3)   # 0到20，每次产生三个随机数
            b = image[row, col, 0]  # blue
            g = image[row, col, 1]  # green
            r = image[row, col, 2]  # red
            image[row, col, 0] = clarm(b + s[0])
            image[row, col, 1] = clarm(g + s[1])
            image[row, col, 2] = clarm(r + s[2])
    cv2.imshow("noise", image)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
gaussian_noise(src)   # 自定义的高斯模糊
dst = cv2.GaussianBlur(src, (5, 5), 0)  # 如何处理这种高斯模糊（此时的src已经被变成高糊图片了） 参数： 图片，（x，y），sigam值
cv2.imshow("new_gaussian", dst)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口