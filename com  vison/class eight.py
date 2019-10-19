import cv2
import numpy as np
# 模糊操作
# 图像的模糊实质就是图像受到平均或积分运算造成的，因此可以对图像进行逆运算如微分运算来使图像清晰化

# 卷积核：是一个把图像变清晰或变模糊的数组，多数为二维数组
# 如果卷积核内数相加为1，就是图像的锐化  # 如果为0，即为模糊


def blur_demo(image):  # 均值模糊（用与图像去噪方面）
    dst = cv2.blur(image, (5, 5))  # blur参数：图片，卷积核长与宽（x， y） 可改变括号内的值加深模糊
    cv2.imshow("blur", dst)     # x与y必须单数


def miden_blur_demo(image):  # 中值模糊 (去掉图像的椒盐噪声） : 中值模糊也行，但效果不好
    dst = cv2.medianBlur(image, 5)
    cv2.imshow("midden", dst)


def customer_blur_demo(image):  # 自定义模糊                 # 如果是3*3的卷积核，除以9
    kernel = np.ones([5, 5], np.float32)/25  # 自定义卷积核 # /25是因为5*5的核处理的化可能溢出像素，要除以25
    dst = cv2.filter2D(image, -1, kernel=kernel)   # 参数值：图片，-1，自定义卷积核
    cv2.imshow("self", dst)


def brigthen(image):  # 卷积锐化  卷积既可以模糊也可以立体化 弄个锐化的算子: 拉普拉斯锐化模板
    kernel = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]], np.float32)  # 不要除以9
    dst = cv2.filter2D(image, -1, kernel=kernel)
    cv2.imshow("3D", dst)


src = cv2.imread("D:/pictures/sunny.jpg")
src1 = cv2.imread("D:/pictures/lena.jpg")  # 案例 # 原图有一些像椒盐一样的点
cv2.imshow('src', src)
cv2.imshow('src1', src1)
blur_demo(src)
miden_blur_demo(src1)
customer_blur_demo(src)
brigthen(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
