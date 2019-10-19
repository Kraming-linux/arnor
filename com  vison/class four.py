import cv2
import numpy as np


# 色彩空间转换API
# 学会用inrange  inrange函数用于提取颜色
# 通道分离与合并
#   H: 0-180   S: 0-255   V: 0-255
#      黑  灰 白   红       橙   黄   绿  青  蓝  紫 
#hmin  0   0   0  0 ||156   11  26   35  78  100  125
#hmax 180 180 180 10||180   25  34； 77  99  124  155 
#smin  0   0   0    43       43  43   43  43  43    43 
#smax 255  43  30   255     255  255  255 255 255   255 
#vmin 0   46   221   46      46  46    46  46  46    46
#vmax 46  220 255   255    255  255  255  255 255  255


def extrace_object():  # 提取颜色对象
    capture = cv2.VideoCapture(" D:/pictures/movie.mp4")  # 捕获摄像头
    running = True
    while running:
        ret, frame = capture.read()  # 读取视频或图像
        if ret == False:
            break
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)          # 变成HSV
        lower_hsv = np.array([0, 0, 211])                    # 设置获取颜色的下限  这里抓取白色
        upper_hsv = np.array([180, 30, 255])                 # 设置颜色上限     （上面有vmin，vmax等）
        mark = cv2.inRange(hsv,lowerb=lower_hsv, upperb=upper_hsv)  # 三个参数 目标，下限，上限
        dst = cv2.bitwise_and(frame, frame, mask=mark)
        cv2.imshow("new_video", dst)
        cv2.imshow("video", frame)  # 显示在桌面上
        c = cv2.waitKey(50)  # 50ms
        if c == 27:
            break


src = cv2.imread("D:/pictures/sunny.jpg")  # 引用一下class one的图
cv2.imshow('src', src)        # blue，green，red 三通道的顺序（0，1，2)
b, g, r = cv2.split(src)  # 三通道分离 分成蓝，绿，红三个通道的图片，然后show出来
cv2.imshow("blue", b)
cv2.imshow("green", g)
cv2.imshow("red", r)
src[:, :, 2] = 0    # 可以任意修改通道的值（把red通道全部弄没了）
cv2.imshow('new_src', src)
src1 = cv2.merge([b, g, r])  # 三通道合并 注意要以数组的形式 merge最多填两个参数
cv2.imshow("new_src", src1)  # 合并后的新图
cv2.waitKey(0)
cv2.destroyAllWindows()