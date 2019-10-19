#ifndef KALMAN_H
#define KALMAN_H

#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include<iostream>
using namespace cv;
using namespace std;
#define ANTI_RANGE 2
#define DEAD_BAND 0
#define SIZE_X 640
#define SIZE_Y 480
class RM_kalmanfilter
{
public:
    RM_kalmanfilter();
    ~RM_kalmanfilter();
    Point2f point_Predict(double g_runtime,Point2d current_point);

private:
    Mat measurement_img;//测量矩阵
    cv::KalmanFilter kalman_img;
    Point2f last_point;
    Point2f last_predic_point;
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
