import cv2
import numpy as np


# ROI与泛洪填充
# ROI区域是指对图像感兴趣的区域
#  FLOODFILL_FIXED_RANGE- 改变图像
#  FLOODFILL_MASK_ONLY- 不改变图像，只填充遮罩层本身，忽略新的颜色值参数（遮罩层的值必须为0）


def fill_color(image):  # 泛洪填充  # floodFill函数的参数：图片，遮罩层，填充色，像素低值，像素高值，填充模式
    copyimg = image.copy()
    h, w = image.shape[:2]   # 选择长，宽
    mask = np.zeros([h+2, w+2], np.uint8)  # mask只能那么干
    cv2.floodFill(copyimg, mask, (30, 30), (0, 255, 255), (100, 100, 100), (50, 50, 50), cv2.FLOODFILL_FIXED_RANGE)
    cv2.imshow("copying", copyimg)


def fill_briny():  # 二值图片的处理  填充ROI区域
    image = np.zeros([400, 400, 3], np.uint8)
    image[100:300, 100:300, :] = 255
    cv2.imshow("fill_briny", image)
    mask = np.ones([402, 402], np.uint8)  # 还是要个遮罩层
    mask[101:301, 101:301] = 0
    cv2.floodFill(image, mask, (200, 200), (0, 0, 255), cv2.FLOODFILL_MASK_ONLY)  # 第二种填充方式
    cv2.imshow("fillnews", image)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)    # 原始图像
face = src[50:500, 100:500]  # 选择你的ROI区域  数组切片
cv2.imshow("face", face)
gray = cv2.cvtColor(face, cv2.COLOR_BGR2GRAY)
cv2.imshow("gray", gray)
fill_color(src)
fill_briny()
grayface = cv2.cvtColor(gray, cv2.COLOR_GRAY2BGR)   # 图像的反操作 对原来切出来的灰图合并到原图
src[50:500, 100:500] = grayface   # 对选中的ROI区域赋值变灰
cv2.imshow("load", src)     # 展现变灰后得到的原图
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口