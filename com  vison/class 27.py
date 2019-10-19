import cv2
import numpy as np


'''
分水岭算法：
距离变换
分水岭案例
'''


def watershed_demo(image):
    # 降噪操作
    blur = cv2.GaussianBlur(image, (5, 5), 0)  # 先去下噪声
    # 灰度和二值化处理
    gray = cv2.cvtColor(blur, cv2.COLOR_BGR2GRAY)  # 灰度
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)  # 二值化
    cv2.imshow("binary", binary)

    # 形态学操作
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))  # 矩形结构元素
    mb = cv2.morphologyEx(binary, cv2.MORPH_OPEN, kernel, iterations=2)  # 两次开操作
    sure_op = cv2.dilate(mb, kernel, iterations=3)  # 图像膨胀 后面会用到的
    cv2.imshow("sure_op", sure_op)

    # 距离变换
    dist = cv2.distanceTransform(mb, cv2.DIST_L2, 3)  # L2的距离， 卷积大小3
    dis_op = cv2.normalize(dist, 0, 1.0, cv2.NORM_MINMAX)  # 更好展示 注意参数在0到1之间，最后选MINMAX
    cv2.imshow("dict_op", dis_op*50)  # 它就是那么干的我也不知道

    ret, surface = cv2.threshold(dist, dis_op.max()*0.6, 255, cv2.THRESH_BINARY)

    surface_fg = np.uint8(surface)  # 转成整型
    unknow = cv2.subtract(sure_op, surface_fg)  # 找出图像的未知区域
    ret, markers = cv2.connectedComponents(surface_fg)  # 找出markers

    # 分水岭操作
    markers = markers + 1
    markers[unknow == 255] = 0  # 像素操作
    markers = cv2.watershed(image, markers=markers)
    image[markers == -1] = [0, 255, 255]  # 标记区域的值为-1
    cv2.imshow("result", image)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
watershed_demo(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口