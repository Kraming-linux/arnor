import cv2


# 边缘保留滤波 （EPF） 美颜的滤镜的关键步骤之一


def bi_demo(image):  # 高斯双边模糊实现EPF（死亡滤镜）
    dst = cv2.bilateralFilter(image, 0, 100, 15)  # 参数：图片，d=0，差值，sigame
    cv2.imshow("bi_demo", dst)


def shift_demo(image):  # 均值迁移实现EPF（油画一般的效果）
    dst = cv2.pyrMeanShiftFiltering(image, 10, 15)  # 参数：图片，space，均值漂移的半径范围
    cv2.imshow("shift_demo", dst)  # 有时均值迁移会过分模糊，效果可能不大好


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
bi_demo(src)  # 妈耶这滤镜效果有点恐怖啊。。
# shift_demo(src)  # 油画图片
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口