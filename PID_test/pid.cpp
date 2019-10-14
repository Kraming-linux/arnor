#include "pid.h"


PIDcontrolor::PIDcontrolor()
{
    cout << "I am ready" << endl;
}


PIDcontrolor::~PIDcontrolor()
{
    cout << "release" << endl;
}


float PIDcontrolor::point_dis(Point p1, Point p2)
{
    float D = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    return D;
}


Point PIDcontrolor::pid_Control_predictor(Point predict, Point local)
{
    cv::Point true_location;  // 返回的坐标点，后续可能会改成角度，暂留

    int error_x;  // x坐标之间的差值
    int error_y;  // y坐标之间的差值
    int dis;

    predict_x = predict.x;  /*预测位置*/
    predict_y = predict.y;

    last_x = local.x;  /*上一次的实际位置*/
    last_y = local.y;

    error_x = abs(predict_x - last_x);
    error_y = abs(predict_y - last_y);

    dis = point_dis(predict, local);


    if(dis < 3)
    { // 差值过小的不进入PID处理
        true_location.x = last_x;  // 差值过小就不用预测位，防止云台抖动
        true_location.y = last_y;
    }


    if(dis > 5 && dis < 10)
    {  // 合理范围内就用预测点，具体参数要依实际调整
        true_location.x = predict_x;
        true_location.y = predict_y;
    }


    if(dis > 10)  // 依实际调整
    {
        // 调用PID //
        error_x = error_x*kP + error_x*kI + error_x*kD;  // PID修正误差
        error_y = error_y*kP + error_y*kI + error_y*kD;

        /*下面有四种情况，直接看代码理解会比较好，注意图像坐标是左上角为0点*/
        if(predict_x >= last_x && predict_y >= last_y)
        {
            true_location.x = last_x + error_x;
            true_location.y = last_y + error_y;
        }
        else if (predict_x >= last_x && predict_y <= last_y)
        {
            true_location.x = last_x + error_x;
            true_location.y = last_y - error_y;
        }
        else if (predict_x <= last_x && predict_y <= last_y)
        {
            true_location.x = last_x - error_x;
            true_location.y = last_y - error_y;
        }
        else if (predict_x <= last_x && predict_y >= last_y)
        {
            true_location.x = last_x - error_x;
            true_location.y = last_y + error_y;
        }
        else {

        }

     }
    return true_location;  // 返回最终修复的坐标
}

