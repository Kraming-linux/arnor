import cv2
import numpy as np

'''
图像梯度： 
一阶导与soble算子 : 求导算子
二阶导与拉普拉斯算子
'''

'''
一阶导数 ： soble算子
水平梯度：x=[-1 0 1      垂直梯度：y=[-1 -2 -1
            -2 0 2                    0  0  0
            -1 0 1]                   1  2  1    
           
     最终图像梯度 = sqrt（x*x + y*y）       
'''


'''
二阶导数： 拉普拉斯算子 最大变化处的值为0

[0  1  0        [1  1  1
 1 -4  1         1 -8  1
 0  1  0]        1  1  1
                
'''


def sobel_demo(image):
    grad_x = cv2.Sobel(image, cv2.CV_32F, 1, 0)  # x的梯度  ddeepth就填这个 ：Scharr是增强版
    grad_y = cv2.Sobel(image, cv2.CV_32F, 0, 1)  # y的梯度  注意参数 1就是一阶导
    gradx = cv2.convertScaleAbs(grad_x)  # 把上面的负数全部取绝对值
    grady = cv2.convertScaleAbs(grad_y)  # 把上面的负数全部取绝对值
    cv2.imshow("grad-x", gradx)  # 左右差异显示
    cv2.imshow("grad-y", grady)  # 上下差异显示
    gradxy = cv2.addWeighted(gradx, 0.5, grady, 0.5, 0)
    cv2.imshow("grad-xy", gradxy)


def lapalian_demo(image):
    # dst = cv2.Laplacian(image, cv2.CV_32F)
    # lpls = cv2.convertScaleAbs(dst)
    kernel = np.array([[0, 1, 0], [1, -4, 1], [0, 1, 0]])  # 拉普拉斯算子（自定义卷积核）
    dst = cv2.filter2D(image, cv2.CV_32F, kernel)
    lpls = cv2.convertScaleAbs(dst)
    cv2.imshow("lapalian", lpls)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
sobel_demo(src)
lapalian_demo(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
