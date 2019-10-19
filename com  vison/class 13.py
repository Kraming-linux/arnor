import cv2
from matplotlib import pyplot as plt

'''
直方图反向投影
把RGB要转成HSV色彩空间

'''


def back_projection_demo():
    sample = cv2.imread("D:/pictures/box_in_scene.png")
    target = cv2.imread("D:/pictures/box.png")
    sam_hsv = cv2.cvtColor(sample, cv2.COLOR_BGR2HSV)
    tar_hsv = cv2.cvtColor(target, cv2.COLOR_BGR2HSV)
       # show the picture
    cv2.imshow("sample", sample)
    cv2.imshow("target", target)
     # calculate the hist
    samhist = cv2.calcHist([sam_hsv], [0, 1], None, [32, 32], [0, 180, 0, 256])
    cv2.normalize(samhist, samhist, 0, 255, cv2.NORM_MINMAX)  # 要转个尺寸
    dst = cv2.calcBackProject([tar_hsv], [0, 1], samhist, [0, 180, 0, 256], 1)  # 计算反向投影，最后一个参数选择的是要不要放缩：1就是原比例
    cv2.imshow("backpro", dst)


def hist2D(image):  # 2D直方图的建立  计算H通道和S通道的范围
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    hist = cv2.calcHist([image], [0, 1], None, [32, 32], [0, 180, 0, 256])  # H通道的range是0到180,S的是0到256
    plt.imshow(hist, interpolation="nearest")  # 酷炫出图
    plt.title("2D_hist")
    plt.show()


src = cv2.imread("D:/pictures/sunny.jpg")
# cv2.imshow('src', src)
hist2D(src)
# back_projection_demo()
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口