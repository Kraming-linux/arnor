'''
模板匹配：在复杂的场合没那么有效
特定场合用模板匹配比较好
'''


'''
就是在整个图像区域发现与给定子图像匹配的小块区域
所以需要一个模板图像T，和待检测的源图像S
'''

'''
工作方法：在待检测图像上从左到右，从上到下计算模板图像与
重叠子图像的匹配度，匹配程度越大两者相同的可能性越大
'''


import cv2
import numpy as np


def tam_demo():
    tpl = cv2.imread("D:/pictures/box.png")  # 模板图像T
    target = cv2.imread("D:/pictures/box_in_scene.png")  # 当然是源图像S啦
    cv2.imshow('tpl', tpl)
    cv2.imshow('target', target)
    methons = [cv2.TM_SQDIFF_NORMED, cv2.TM_CCORR_NORMED, cv2.TM_CCOEFF_NORMED]
    th, tw = tpl.shape[:2]
    for md in methons:
        print(md)
        result = cv2.matchTemplate(target, tpl, md)  # 返回的是匹配算法的值（也是图片）
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(result)  # 在匹配位置上生成一个矩形
        if md == cv2.TM_SQDIFF_NORMED:
            tl = min_loc
        else:
            tl = max_loc
        br = (tl[0]+tw, tl[1]+th)  # tl[0] is x, tl[1] is y
        cv2.rectangle(target, tl, br, (0, 0, 255), 2)
        cv2.imshow("match"+np.str(md), target)
        # cv2.imshow("match"+np.str(md), result)


src = cv2.imread("D:/pictures/sunny.jpg")
cv2.imshow('src', src)
tam_demo()
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口


'''
1 平方
3 相关性
5 相关因子
'''