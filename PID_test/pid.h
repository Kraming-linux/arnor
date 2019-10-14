#include "settings.h"


#define WIDTH 640  // 假装这两个是最大像素
#define HEIGHT 480

#define kP 0.6
#define kI 0.02
#define kD 0.1


/**
PID的参数: kP，kD，kI
这里打算用位置式的PID控制
要看预测的效果
kP:比例系数
kI:积分系数
kD:微分系数
**/


#ifndef PID_H
#define PID_H

class PIDcontrolor
{

public:
    PIDcontrolor();
    ~PIDcontrolor();

private:
    int predict_x;  // 预测位x
    int predict_y;  // 预测位y

    int last_x;  // 当前装甲板x
    int last_y;  // 当前装甲板y

    int true_x;  // 修正后的x
    int true_y;  // 修正后的y

public:
    float point_dis(Point p1, Point p2);  // 两点距离

    Point pid_Control_predictor(Point predict, Point local);  // PID控制,返回的是一个修正后的点

};
#endif // PID_H
