import cv2
import numpy as np


def assess_pictutre(iamge):      # 遍历数组的每个像素点
    print(iamge.shape)
    heigh = iamge.shape[0]  # 形状的第一维度（高度）
    width = iamge.shape[1]  # 第二维度（宽度）
    channel = iamge.shape[2]  # 通道数
    print("heigh", heigh)
    print("width", width)
    print("channel", channel)


def inverse(img):  # 像素取反
    dst = cv2.bitwise_not(img)
    cv2.imshow("img", dst)


def create_Image():
    img = np.zeros([400, 400, 3], np.uint8)  # 生成一张全0三通道的400*400大小的图（全黑）
    img[:, :, 0] = np.ones([400, 400])*255  # 修改原图生成全蓝色的新图 0 1 2 （三通道顺序）
    cv2.imshow("newpicture", img)


   # img = np.ones([400,400,1],np.uint8)
   # img = img * 127
   # cv2.imshow("new2", img)               这些是单通道生成灰色图像的代码
     # 早期黑白电视的值是0到255（0是黑色，255是白色）


src = cv2.imread("D:/pictures/sunny.jpg")  # 引用一下class one的图
cv2.imshow('src', src)        # blue，green，red 三通道的顺序（0，1，2）

assess_pictutre(src)

create_Image()

t1 = cv2.getTickCount()  # 计算上面所需时间
inverse(src)
t2 = cv2.getTickCount()  # 同上
time = (t2-t1)/cv2.getTickFrequency()  # 两时间差就是inverse所用的时间
print("time is ", time*1000)     # 乘以1000转化为毫秒的计数单位
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口

