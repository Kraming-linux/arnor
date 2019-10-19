"""
卡尔曼滤波器：
鼠标追踪案例
"""


import cv2
import numpy as np

'''------创建一个800X800的空帧------'''
frame = np.zeros((800, 800, 3), np.uint8)
'''---------初始化测量坐标和鼠标运动预测的数组--------------------------'''
last_measurement = current_measurement = np.array((2, 1), np.float32)
last_prediction = current_prediction = np.zeros((2, 1), np.float32)
'''--------------------------------------------------------------------'''


'''-------------------------------------------------------------------------'''
"""
鼠标移动的回调函数，绘制跟踪效果：
存储上一次的测量和预测，用当前测量来校正卡尔曼滤波器
计算卡尔曼滤波的值
然后绘制两条线
上一次测量到当前测量以及上一次预测到当前预测的两条线
"""


def mouse_move(event, x, y, s, p):
    global frame, current_measurement, measurements
    global last_measurement, current_prediction, last_prediction
    last_prediction = current_prediction
    last_measurement = current_measurement
    current_measurement = np.array([[np.float32(x)], [np.float32(y)]])
    kalman.correct(current_measurement)  # 修正卡尔曼滤波的预测结果
    current_prediction = kalman.predict()  # 估算目标位置
    lmx, lmy = last_measurement[0], last_measurement[1]  # 上次测量值
    cmx, cmy = current_measurement[0], current_measurement[1]  # 当前测量值
    lpx, lpy = last_prediction[0], last_prediction[1]  # 上次预测值
    cpx, cpy = current_prediction[0], current_prediction[1]  # 当前预测值
    cv2.line(frame, (lmx, lmy), (cmx, cmy), (0, 255, 255))  # 上次测量
    cv2.line(frame, (lpx, lpy), (cpx, cpy), (255, 0, 0))  # 这次测量


'''---------------------------------------------------------------------------'''

# 确定触发事件 #
cv2.namedWindow("kalman_tracker")
cv2.setMouseCallback('kalman_tracker', mouse_move)

'''-------创建卡尔曼滤波器--------'''

kalman = cv2.KalmanFilter(4, 2)  # 在此cv2.KalmanFilter(4,2)表示转移矩阵维度为4，测量矩阵维度为2

# 设置测量矩阵
kalman.measurementMatrix = np.array([[1, 0, 0, 0], [0, 1, 0, 0]], np.float32)
# 设置转移矩阵
kalman.transitionMatrix = np.array([[1, 0, 1, 0], [0, 1, 0, 1], [0, 0, 1, 0], [0, 0, 0, 1]], np.float32)
# 设置过程噪声协方差矩阵
kalman.processNoiseCov = np.array([[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]], np.float32) * 0.03

'''------------------------------'''


while True:
    cv2.imshow("kalman_tracker", frame)
    if cv2.waitKey(30) & 0xff == 27:
        break

cv2.destroyAllWindows()