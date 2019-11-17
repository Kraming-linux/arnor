import cv2
import serial
import numpy as np
import time
import sys


src_img = cv2.imread("/home/kraming/color_3.jpg")
dst_img = src_img.copy()    # 保存一张源图来show

blue_sum = []   # 装轮廓面积的LIST
green_sum = []
red_sum = []


'''
还是色道相减法
'''
B = np.array(src_img[:, :, 0])  # 直接从源图分离
G = np.array(src_img[:, :, 1])
R = np.array(src_img[:, :, 2])


B_img_1 = cv2.subtract(B, G)
B_img_2 = cv2.subtract(B, R)
B_img = B_img_1 & B_img_2
ret, binary_blue = cv2.threshold(B_img, 60, 255, cv2.THRESH_BINARY)
contours_blue, ret = cv2.findContours(binary_blue, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
for i, contour_red in enumerate(contours_blue):
    area_b = cv2.contourArea(contour=contour_red)   # 收集每个轮廓的面积（变相找出有多少个轮廓）
    blue_sum.append(area_b)
    cv2.drawContours(dst_img, contours_blue, i, (0, 255, 0), 3)  # 对的，就是这样,其实不画也行


G_img_1 = cv2.subtract(G, B)
G_img_2 = cv2.subtract(G, R)
G_img = G_img_1 & G_img_2
ret, binary_green = cv2.threshold(G_img, 60, 255, cv2.THRESH_BINARY)
contours_green, ret = cv2.findContours(binary_green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
for j, contour_green in enumerate(contours_green):
    area_g = cv2.contourArea(contour=contour_green)
    green_sum.append(area_g)
    cv2.drawContours(dst_img, contours_green, j, (0, 0, 255), 3)  # 对的，就是这样


R_img_1 = cv2.subtract(R, G)
R_img_2 = cv2.subtract(R, B)
R_img = R_img_1 & R_img_2
ret, binary_red = cv2.threshold(R_img, 60, 255, cv2.THRESH_BINARY)
contours_red, ret = cv2.findContours(binary_red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
for k, contour_red in enumerate(contours_red):
    area_r = cv2.contourArea(contour=contour_red)
    red_sum.append(area_r)
    cv2.drawContours(dst_img, contours_red, k, (255, 0, 0), 3)


'''--------------------------逻辑判断------------------------------'''
if len(blue_sum) > len(green_sum) and len(blue_sum) > len(red_sum):
    print("blue")
elif len(green_sum) > len(blue_sum) and len(green_sum) > len(red_sum):
    print("green")
elif len(red_sum) > len(blue_sum) and len(red_sum) > len(green_sum):
    print("red")
else:
    print("lack of condition")
'''-----------------------------------------------------------------'''


cv2.imshow("color", dst_img)
cv2.waitKey(0)
cv2.destroyAllWindows()
sys.exit()
