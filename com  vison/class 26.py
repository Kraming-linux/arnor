import cv2
import numpy as np

'''
其他形态学操作：
顶帽：原图与开操作的差值图像
黑帽：闭操作与原图的差值图像
形态学梯度
'''


def tophat_demo(image):  # 顶帽demo
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    dst = cv2.morphologyEx(gray, cv2.MORPH_TOPHAT, kernel=kernel)
    brint = np.array([gray.shape], np.uint8)  # 把原图变的更亮一点
    brint = 100
    src = cv2.add(dst, brint)  # 别加权重
    cv2.imshow("TOPHAT", src)


def blackhat_demo(image):  # 黑帽demo
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    dst = cv2.morphologyEx(gray, cv2.MORPH_BLACKHAT, kernel=kernel)
    brint = np.array([gray.shape], np.uint8)  # 把原图变的更亮一点
    brint = 100
    src = cv2.add(dst, brint)  # 别加权重
    cv2.imshow("BLACK", src)


def basic_grad_demo(image):  # 基本梯度
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, binary = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    dst = cv2.morphologyEx(binary, cv2.MORPH_GRADIENT, kernel=kernel)
    cv2.imshow("basic_grad", dst)


def grad_item(image):
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
    dm = cv2.dilate(image, kernel)
    em = cv2.erode(image, kernel)
    dst1 = cv2.subtract(image, em)  # 图像内梯度（别把参数填反了）
    dst2 = cv2.subtract(dm, image)  # 图像外梯度
    cv2.imshow("inter", dst1)
    cv2.imshow("exter", dst2)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
# blackhat_demo(src)
# basic_grad_demo(src)
grad_item(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
