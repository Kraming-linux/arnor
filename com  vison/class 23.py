import cv2
import numpy as np

'''
对象测量：
轮廓检测
计算每个轮廓的弧长与面积，像素单位上的
'''


def messure_object(image):
    gray =cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    print('图像阈值', ret)
    cv2.imshow('thresh', thresh)
    contours, herichy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)  # 新版本只有两个返回值,测外轮廓
    print("层次", herichy)
    for i, contour in enumerate(contours):
        area = cv2.contourArea(contour)  # 轮廓面积
        x, y, w, h = cv2.boundingRect(contour)  # 轮廓的外接矩形
        present = min(w, h)/max(w, h)
        print("图像宽高比", present)
        mon = cv2.moments(contour)  # 轮廓的几何矩
        print("几何矩类型", type(mon))
        cx = mon['m10']/mon['m00']  # 轮廓的中心位置  : 有时候会除以0， 注意图像
        cy = mon['m01'] / mon['m00']  # 同上
        cv2.circle(image, (np.int(cx), np.int(cy)), 2, (255, 0, 0), 2)
        cv2.rectangle(image, (x, y), (x+w, y+h), (0, 255, 0), 1)
        print("轮廓面积", area)
    cv2.imshow("detect_countour", image)


def special_messure(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
    cv2.imshow('thresh', thresh)
    contours, herichy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)  # 新版本只有两个返回值,测外轮廓
    dst = cv2.cvtColor(thresh, cv2.COLOR_GRAY2BGR)  # 换个图像来突显特征
    for i, contour in enumerate(contours):
        x, y, w, h = cv2.boundingRect(contour)  # 轮廓的外接矩形
        present = min(w, h) / max(w, h)
        print("图像宽高比", present)
        mon = cv2.moments(contour)  # 轮廓的几何矩
        print("几何矩类型", type(mon))
        cx = mon['m10'] / mon['m00']  # 轮廓的中心位置  : 有时候会除以0， 注意图像
        cy = mon['m01'] / mon['m00']  # 同上
        cv2.circle(dst, (np.int(cx), np.int(cy)), 2, (255, 0, 0), 2)
        # cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 1)
        approxCurve = cv2.approxPolyDP(contour, 4, True)
        print(approxCurve.shape)
        if approxCurve.shape[0] > 6:  # 画出大于6条线的圆的轮廓，积分
            cv2.drawContours(dst, contours, i, (0, 255, 0), 2)
        if approxCurve.shape[0] == 4:  # 画出四条线的轮廓（矩形嘛）
                    cv2.drawContours(dst, contours, i, (0, 255, 255), 2)
        if approxCurve.shape[0] == 3:  # 画出三条线的轮廓（三角形嘛）
            cv2.drawContours(dst, contours, i, (255, 0, 0), 2)
    cv2.imshow("special", dst)


src = cv2.imread("D:/pictures/box.png")
cv2.imshow('src', src)
# messure_object(src)
special_messure(src)
cv2.waitKey(0)            # 等待下一个按键触发
cv2.destroyAllWindows()    # 关闭窗口
