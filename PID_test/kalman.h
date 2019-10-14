#ifndef KALMAN_H
#define KALMAN_H

#include "settings.h"


/**
ANTI_RANGE:  指数函数的底数
DEAD_BAND:   死线
**/


#define ANTI_RANGE 1.01
#define DEAD_BAND 0
#define SIZE_X 640
#define SIZE_Y 480

class RM_kalmanfilter
{
public:
    RM_kalmanfilter();
    ~RM_kalmanfilter();
    Point2f point_Predict(double g_runtime,Point2d current_point);  // 预测后的点

private:
    Mat measurement_img;  //测量矩阵
    cv::KalmanFilter kalman_img;  // 卡尔曼矩阵
    Point2f last_point;  // 上一个点
    Point2f last_predic_point;  // 上一个预测点
    double runtime=(1e-2) + 0.005666666f;
    double last_v=0;
    double last_a=0;
    double feedback_v = 0;
    int n=1;

    int lead_max_value = 0;
    float last_angular_v_change_rate = 0;
    float last_line_v_change_rate = 0;
};






#endif // KALMAN_H
