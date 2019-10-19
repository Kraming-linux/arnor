import cv2
import numpy as np

'''
直线检测： 霍夫变换
前提：边缘检测已完成
直角坐标换成极坐标
'''


def line_space(image):  # 算法实现，还是不要用为好
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 转成灰度
    edge = cv2.Canny(gray, 50, 150, apertureSize=3)  # 边缘检测
    lines = cv2.HoughLines(edge, 1, np.pi/180, 150)  # roh = r, theta = 角度
    for line in lines:  # 极坐标
        print(type(lines))  # <class 'numpy.ndarray'> 这是个多维数组 第0个元素就是r，theta
        rho, theta = line[0]
        a = np.sin(theta)
        b = np.cos(theta)
        x0 = a*rho
        y0 = b*rho  # 下面那堆鬼东西我也不知道opencv干嘛要这么干
        x1 = int(x0+1000*(-b))
        y1 = int(y0+1000*(a))
        x2 = int(x0+1000*(-b))
        y2 = int(y0+1000*(a))
        cv2.line(image, (x1, y1), (x2, y2), (0, 0, 255), 2)
    cv2.imshow("line_demo", image)


def line_possible(image):  # 尽量用这个
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # 转成灰度
    edge = cv2.Canny(gray, 50, 150, apertureSize=3)  # 边缘检测
    lines = cv2.HoughLinesP(edge, 1, np.pi/180, 100, minLineLength=50, maxLineGap=10)
    for line in lines:
        print(type(line))  # <class 'numpy.ndarray'> 这是个多维数组 第0个元素就是r，theta
        x1, y1, x2, y2 = line[0]
        cv2.line(image, (x1, y1), (x2, y2), (0, 0, 255), 2)
    cv2.imshow("line_demo", image)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
line_possible(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口