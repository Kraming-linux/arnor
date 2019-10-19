#include "kalman.h"


RM_kalmanfilter::~RM_kalmanfilter() {

}

RM_kalmanfilter::RM_kalmanfilter(): kalman_img(3, 3) //状态向量和测量向量都是３维。板不受控制，无控制向量，为默认0维
{
    measurement_img = Mat::zeros(3, 1, CV_64F);
    kalman_img.transitionMatrix = (Mat_ < double > (3, 3) <<
        1, runtime, runtime * runtime / 2, //下一步长的计算值为当前坐标加上当前速度乘以步长
        0, 1, runtime,
        0, 0, 1);
    kalman_img.processNoiseCov = (Mat_ < double > (3, 3) <<
        pow(runtime, 5) / 20, pow(runtime, 4) / 8, pow(runtime, 3) / 6, //下一步长的计算值为当前坐标加上当前速度乘以步长
        pow(runtime, 4) / 8, pow(runtime, 3) / 3, pow(runtime, 2) / 2,
        pow(runtime, 3) / 6, pow(runtime, 2) / 2, runtime);

    kalman_img.measurementMatrix = Mat_ < double > (3, 3);
    kalman_img.measurementNoiseCov = Mat_ < double > (3, 3);
    kalman_img.errorCovPost = Mat_ < double > (3, 3);

    setIdentity(kalman_img.measurementMatrix, Scalar::all(1)); //测量值也是坐标，不需要线性的变化
    setIdentity(kalman_img.measurementNoiseCov, Scalar::all(1e-10)); //测量协方差矩阵R，更大会有更慢的回归
    setIdentity(kalman_img.errorCovPost, Scalar::all(1)); //后验协方差矩阵P，方便迭代。设为无。

    kalman_img.statePost = (Mat_ < double > (3, 1) << 0, 0, 0); //后验更新值(迭代起点)
    last_point.x = 320;
    last_point.y = 240;
}

Point2f RM_kalmanfilter::point_Predict(double init_runtime, Point2d current_point)
{
    runtime = init_runtime;
    double v = (current_point.x - last_point.x) / runtime;  // 速度
    double a = (v - last_v) / runtime;    // 加速度
//    double b;
    cout<<"["<<runtime<<","<<v<<","<<a<<"]"<<endl;
//    if(a<-6.7906e-6){  // -6.7906 * 10^-6 ???
//        b=a*-100;
//    }
//    else if(a>6.7906e-6){b=a*100;}
//    else{b=a;}

//    int expectation_distance = current_point.x - 320;
//    float angular_v_change_rate = (float)expectation_distance / (float)feedback_v;
//    if(angular_v_change_rate > last_angular_v_change_rate){
//        //加上限
//        lead_max_value = abs(expectation_distance)+ abs(feedback_v);
//    }else{
//        //减上限
//        lead_max_value = abs(expectation_distance) - abs(feedback_v);
//        if(lead_max_value < 0){
//            lead_max_value = 0;
//        }
//    }
//    last_angular_v_change_rate = angular_v_change_rate; //更新

//    float line_v_change_rate = v / last_v;
//    float coefficient = 1.f;
//    if(line_v_change_rate < last_line_v_change_rate){
//        coefficient = coefficient * 2.f;
//        //减速
//    }else{
//        coefficient = coefficient * 0.5;
//        //加速
//    }
//    last_line_v_change_rate = line_v_change_rate; //更新

    measurement_img.at < double > (0, 0) = current_point.x;  // 现在的点
    measurement_img.at < double > (1, 0) = v;
    measurement_img.at < double > (2, 0) = a;

    Mat prediction2 = kalman_img.predict(); //至此完成了对下一帧单纯计算的预测，得出的点更加平稳。如果保证测量值够准，可以直接取这里算出的点
    Mat prediction = kalman_img.correct(measurement_img); //至此完成了对下一帧的最优估计，得出的点理论上更接近真实值。同时用于迭代，得出statePost供下一帧predict计算
    Point2f temp_point = Point2f(prediction.at < double > (0, 0), current_point.y);

    double temp_x = current_point.x + pow(ANTI_RANGE,n) * (current_point.x - temp_point.x);
    Point2f anti_kalman_point;

    if(a<-6.7906e-6||a>6.7906e-6){
        n+=n;
    }
    else{n=1;}
//    if(fabs(ANTI_RANGE*(current_point.x-temp_point.x))>10.0 )
//    {
//        temp_x=current_point.x+2*(current_point.x-temp_point.x);
//    }
    if(temp_x <= SIZE_X && temp_x >= 0)
    {
        if (abs(current_point.x - temp_point.x) > DEAD_BAND)
        {
            anti_kalman_point.x = temp_x;
        }
        else
        {
            anti_kalman_point.x = current_point.x;
        }
    }
    else
    {
        anti_kalman_point.x = current_point.x;
    }

//    if(a<-6.7906e-8||a>6.7906e-8){
//        anti_kalman_point.x=current_point.x;
//    }
//    if(fabs(temp_x - current_point.x) >= DEAD_BAND){
//        if(fabs(temp_x - current_point.x) >= lead_max_value){
//            if(temp_x < current_point.x){
//                anti_kalman_point.x = current_point.x - lead_max_value;
//            }else{
//                anti_kalman_point.x = current_point.x + lead_max_value;
//            }
//        }else{
//            if(temp_x < current_point.x){
//                anti_kalman_point.x = temp_x;
//            }else{
//                anti_kalman_point.x = temp_x;
//            }
//        }
//    }else{
//        anti_kalman_point.x = current_point.x;
//    }
    anti_kalman_point.y = temp_point.y;

    last_v = v;
    last_point = current_point;
    return anti_kalman_point;
}
