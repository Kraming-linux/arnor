import cv2
import numpy as np


'''
图像金字塔：高斯金字塔和拉普拉斯金字塔
'''


'''
高斯金字塔 ： 
reduce = 高斯模糊+降采样 ： 先高斯模糊后取2的n次方+1缩小图片
就是图像递归像金字塔形状一般

expand = 扩大+卷积 ： 与上面的reduce的形状相反 倒金字塔
'''


'''
拉普拉斯金字塔 ： 
基于高斯金字塔获取
'''


def pyramid_demo(image):   # reduce层金字塔
    level = 3      # 定义一个三层金字塔
    temp = image.copy()  # 拷贝对象
    ramid = []  # 返回一个list
    for i in range(level):  # 遍历金字塔
        dst = cv2.pyrDown(temp)   # reduce金字塔
        ramid.append(dst)     # 把每次迭代的结果放进列表里
        cv2.imshow("pyramid_demo_level:"+str(i), dst)
        temp = dst.copy()  # 还是把对象拷贝进去
    return ramid     # 返回最终结果


def lapalian_demo(image):  # 拉普拉斯金字塔
    ramid = pyramid_demo(image)  # 要用上面变量的返回值建金字塔
    level = len(ramid)  # level基于原来高斯金字塔
    for i in range(level-1, -1, -1):  # 从高位到低位，步长为1 ： 就下楼一样
        if (i-1) < 0:    # 最后一层的处理： 放原图
            expand = cv2.pyrUp(ramid[i], dstsize=image.shape[:2])  # 拿原图进行计算
            lpls = cv2.subtract(image, expand)   # lpls = g1 - expand
            cv2.imshow("lpls_down"+str(i), lpls)
        else:
            expand = cv2.pyrUp(ramid[i], dstsize=ramid[i - 1].shape[:2])
            lpls = cv2.subtract(ramid[i - 1], expand)  # lpls = g1 - expand（g2）
            cv2.imshow("lpls_down" + str(i), lpls)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
# pyramid_demo(src)
lapalian_demo(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
