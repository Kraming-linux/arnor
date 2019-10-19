import cv2
import numpy as np

'''
超大图像二值化: 分成小块进行处理
'''


def big_image_binary(image):  # 还是全局阈值化，效果会有图像噪声，OTSU全局明显不适合
    print(image.shape)
    cw, ch = 256, 256   # 小块的宽和高
    h, w = image.shape[:2]  # 整个图像的宽和高
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    for row in range(0, h, ch):  # 步长为ch 从0开始一直到h的高度
        for col in range(0, 256, cw):
            roi = gray[row:row+ch, col:col+cw]  # 获取图像的ROI
            ret, dst = cv2.threshold(roi, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
            gray[row:row + ch, col:col + cw] = dst
            print(np.std(dst), np.mean(dst))
    cv2.imwrite("D:/pictures/result_big.jpg", gray)  # 别用imshow了如果图超大你是看不到的，所以把二值化的图放在了pictures下


def big_image_binary1(image):  # 推荐用这种方法：  自适应图像阈值法
    print(image.shape)
    cw, ch = 256, 256   # 小块的宽和高
    h, w = image.shape[:2]  # 整个图像的宽和高
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    for row in range(0, h, ch):  # 步长为ch 从0开始一直到h的高度
        for col in range(0, 256, cw):
            roi = gray[row:row+ch, col:col+cw]  # 获取图像的ROI
            dst = cv2.adaptiveThreshold(roi, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 127, 20)
            gray[row:row + ch, col:col + cw] = dst
            print(np.std(dst), np.mean(dst))
    cv2.imwrite("D:/pictures/result1_big.jpg", gray)  # 别用imshow了如果图超大你是看不到的，所以把二值化的图放在了pictures下


def big_image_binary2(image):  # 还是全局阈值化(过滤加强版）
    print(image.shape)
    cw, ch = 256, 256   # 小块的宽和高
    h, w = image.shape[:2]  # 整个图像的宽和高
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    for row in range(0, h, ch):  # 步长为ch 从0开始一直到h的高度
        for col in range(0, 256, cw):
            roi = gray[row:row+ch, col:col+cw]  # 获取图像的ROI
            print(np.std(roi), np.mean(roi))
            dev = np.std(roi)
            if dev < 10:
                gray[row:row + ch, col:col + cw] = 255
            else:
                ret, dst = cv2.threshold(roi, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
                gray[row:row + ch, col:col + cw] = dst
    cv2.imwrite("D:/pictures/result_big3.jpg", gray)  # 别用imshow了如果图超大你是看不到的，所以把二值化的图放在了pictures下


src = cv2.imread("D:/pictures/fxxk_big.jpg")  # 一张大图
# cv2.imshow('src', src)
big_image_binary1(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
