#include <iostream>
#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "kalman.h"


using namespace std;
using namespace cv;

/*fxxk,在循环里才能开图片*/

#define    POINT_DIST(p1,p2) std::sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y)) // useless
// #define    min_light_height  8  // 暂弃


float centerDistance(Point p1, Point p2)
{
    float Dis = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    return Dis;
}


cv::RotatedRect boundingRRect(const cv::RotatedRect & left, const cv::RotatedRect & right)
{
    // 这个函数是用来将左右边的灯条拟合成一个目标旋转矩形，没有考虑角度
    const Point & pl = left.center, & pr = right.center;
    Point2f center = (pl + pr) / 2.0;
   //    cv::Size2f wh_l = left.size;
  //    cv::Size2f wh_r = right.size;
    // 这里的目标矩形的height是之前灯柱的width
    double width_l = MIN(left.size.width, left.size.height);
    double width_r = MIN(right.size.width, right.size.height);
    double height_l = MAX(left.size.width, left.size.height);
    double height_r = MAX(right.size.width, right.size.height);
    float width = centerDistance(pl, pr) - (width_l + width_r) / 2.0;
    float height = std::max(height_l, height_r);
    //float height = (wh_l.height + wh_r.height) / 2.0;
    float angle = std::atan2(right.center.y - left.center.y, right.center.x - left.center.x);
    return RotatedRect(center, Size2f(width, height), angle * 180 / CV_PI);

}


int main()
{

    RM_kalmanfilter kalman;
    double g_runtime;
    Point2f kalman_point;
    VideoCapture video;
    video.open("/home/kraming/video/步兵自旋-蓝.avi");  // 读视频 步兵自旋-蓝.avi 基地步兵-蓝.avi
    //video.open("/home/kraming/video/armor_2.avi");  // 读视频 armor_4.avi 大小装甲-红.avi
    if (!video.isOpened()){
        cout << "game over" << endl;
        return -1;
    }


    // VideoCapture capture(1);
    // if (!capture.isOpened())
    // {
    // 	cout << "你他妈倒是开啊" << endl;
    // 	return -1;
    // }


    Mat src, dst,gray, binary,result;
    // src = imread("/home/kraming/video/color2.jpg");
    // resize(src, src, Size(800, 800));
    Mat element3 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    vector<Mat> splited; // 分离后储存图片的容器
    vector<Mat>channel;



 while(1)
{
    video >> src;
    //capture >> src;
    //resize(src, src, Size(800, 800));
    src.copyTo(result);
    g_runtime = ((double)getTickCount() - g_runtime) / getTickFrequency();
/*----------------------------------------RGB--------------蓝色-----------------------------*/
     split(src, splited);  // 分离
     cvtColor(src, gray, COLOR_BGR2GRAY);  // 灰度处理
     threshold(gray, binary, 120, 255, THRESH_BINARY);  // 二值化
     subtract(splited[0], splited[2], dst);  // 减法
     threshold(dst, dst, 20, 255, THRESH_BINARY);
     erode(dst, dst, element3);
     dst = dst & binary;   // 与运算  1 & 1 = 1 ； 0 & 1 = 0
     dilate(dst, dst, element3);
/*---------------------------------------------------------------------------------------*/


/*----------------------------------------RGB----------红色---------------------------------*/
//    split(src, splited);  // 分离
//    cvtColor(src, gray, COLOR_BGR2GRAY);  // 灰度处理
//    threshold(gray, binary, 20, 255, THRESH_BINARY);  // 二值化
//    subtract(splited[2], splited[0], dst);  // 减法
//    threshold(dst, dst, 20, 255, THRESH_BINARY);
//    erode(dst, dst, element3);
//    dilate(dst, dst, element3);
//    dst = dst & binary;   // 与运算  1 & 1 = 1 ； 0 & 1 = 0
//    dilate(dst, dst, element3);
/*-----------------------------------------------------------------------------------------*/


/*----------------------------------------HSV--------------------------------------------*/
    // Mat temp, temp1,temp2, red;     // 这测红色的
    // vector<cv::Mat> channels;
    // cvtColor(src, temp, COLOR_BGR2HSV);
    // split(temp, channels);
    // inRange(temp, Scalar(0,43,46),Scalar(10,255,255),temp1);
    // inRange(temp,Scalar(156,43,46),Scalar(180,255,255),temp2);
    // //red =  temp1 + temp2;
    // cv::add(temp1, temp2, red);  // 用add可能好一点
    // cvtColor(src,binary,COLOR_BGR2GRAY);
    // threshold(binary,binary,55,255,THRESH_BINARY);
    // //erode(binary, binary, element3);
    // //morphologyEx(binary, binary, MORPH_OPEN, element3);
    // //imshow("thresh_whole", binary);
    // //dilate(red, red, element3);
    // //imshow("red", red);
    // dst = red & binary;
    // dilate(dst, dst, element3);
/*-------------------------------------------------------------------------------------*/


    // /*--------------------------------------HSV--------------------------------------------*/
//     Mat temp, temp1, blue;     // 蓝色的检测，inRange有毒，后期自己想办法吧。。
//     vector<cv::Mat> channels;
//     cvtColor(src, temp, COLOR_BGR2HSV);
//     split(temp, channels);
//     inRange(temp, Scalar(70,43,46),Scalar(110,255,255),temp1);
//     blue = temp1;
//     cvtColor(src,binary,CV_BGR2GRAY);
//     threshold(binary,binary,25,255,THRESH_BINARY);
//     erode(binary, binary, element3);
//     //morphologyEx(binary, binary, MORPH_OPEN, element3);
//     //imshow("thresh_whole", binary);
//     //dilate(blue, blue, element3);
//     //imshow("blue", blue);
//     dst = blue & binary;
//     dilate(dst, dst, element3);
    /*--------------------------------------------------------------------------------------*/


    vector<vector<Point2i> > contours;  // 放轮廓的
    vector<Vec4i> hierarchy;
    vector < RotatedRect > rotateRect;
    vector < Rect > boundRect;
    findContours(dst, contours, hierarchy, RETR_EXTERNAL , CHAIN_APPROX_SIMPLE, Point(0, 0));
    // -------------初步寻找符合灯条的轮廓 -----------------//
    for (size_t i = 0; i < contours.size(); ++i)
    {
        if (contours.size() <= 1)
        {
            break;        // 这他妈等于没有
        }
        RotatedRect M_rect = minAreaRect(contours[i]); //
        Rect B_rect = boundingRect(contours[i]);
        double max_rrect_len = MAX(M_rect.size.width, M_rect.size.height); // 最长的边
        double min_rrect_len = MIN(M_rect.size.width, M_rect.size.height); // 最短的边
        bool condition_1 = (fabs(M_rect.angle) < 60.0 && M_rect.size.height > M_rect.size.width); // 筛一些角度和形状迷人的
        bool condition_2 = (fabs(M_rect.angle) > 60.0 && M_rect.size.width > M_rect.size.height);  // 感谢深大的激情提供
        bool condition_3 = max_rrect_len > 10; // 灯条最小长度
        bool condition4 = (max_rrect_len/min_rrect_len >= 0.98);  // 最长的比最短的
        bool basic_condition = M_rect.size.height > M_rect.size.width;  // 暂时不启用，那个无角度的矩形已卡位

        if(B_rect.size().height > B_rect.size().width){   // 用这块卡一些横的效果不错,感谢队里的激情提供
           if((condition_1 || condition_2) && condition_3 && condition4){   // 初步筛选灯条
            boundRect.push_back(B_rect);
            rotateRect.push_back(M_rect);
            //  Point2f vertice[4];
            //  rrect.points(vertice);
            //  for (int j = 0; j < 4; j++){
            //  	line(result, vertice[i], vertice[(i + 1) % 4], Scalar(0, 255, 255), 1);
            //  }
           }
        }
    }


    Point2f _pt[4],pt[4];     // 存放灯条的点
    vector <RotatedRect> object;  // 存放大矩形的容器

    vector<Point2f> mid_point(2);  // 两灯条中点坐标
    vector<vector<Point2f>> middle_point;


    // --------------------------开始匹配灯条----------------------------- //
    for (int k1 = 0; k1 < (int) rotateRect.size(); ++k1) {
        if (rotateRect.size() <= 1) {break;}
         RotatedRect  rect_k1 = rotateRect[k1];
         rect_k1.points(_pt);
         float anglek1 = fabs(rect_k1.angle);
         float angle_limited;
         /*pt
          * 1 2
          * 0 3
            * */
            // 往右斜的长灯条
           // rect.points有顺序的，y最小的点是0,顺时针1 2 3
           if(anglek1 > 45.0){
              pt[0] = _pt[3];   // 这些条件暂时用不上。。
              pt[1] = _pt[0];

            }
            else{  // left
               pt[0] = _pt[2];
               pt[1] = _pt[3];
            }

            for (int k2 = k1 + 1; k2 < (int) rotateRect.size(); ++k2) {
                RotatedRect  rect_k2 = rotateRect[k2];
                rect_k2.points(_pt);
                float anglek2 = fabs(rect_k2.angle);

                if(anglek2 > 45.0){   // to solve the angle
                    pt[2] = _pt[2];
                    pt[3] = _pt[1];
                }
                else{
                    pt[2] = _pt[1];
                    pt[3] = _pt[0];
                }


//                if(anglek1 > 45.0 && anglek2 < 45.0){ // 八字 / \   //
//                                    angle_limited = 90.0 - anglek1 + anglek2;
//                 }else if(anglek1 <= 45.0 && anglek2 >= 45.0){ // 倒八字 \ /
//                                    angle_limited = 90.0 - anglek2 + anglek1;
//                 }else{
//                 if(anglek1 > anglek2) angle_limited = anglek1 - anglek2; // 在同一边
//                 else angle_limited = anglek2 - anglek1;
//                     // 这个效果上好像没角度差那么好,已弃用
//                }


               // cout << "left_angle: " << rect_k1.angle << endl;
               // cout << "right_angle: " << rect_k2.angle << endl;


                float area_k1 = rect_k1.size.height * rect_k1.size.width;  // area_k1
                float area_k2 = rect_k2.size.height * rect_k2.size.width;
                //cout << "area_k1:" << area_k1 << endl;
                //cout << "area_k2:" << area_k2 << endl;

                /*****获取中点坐标和两根灯条中最长的边*****/
                float x1 = rect_k1.center.x;
                float y1 = rect_k1.center.y;
                float x2 = rect_k2.center.x;
                float y2 = rect_k2.center.y;
                float len_k1 = MAX(rect_k1.size.width, rect_k1.size.height);
                float len_k2 = MAX(rect_k2.size.width, rect_k2.size.height);

                float center_slope = fabs((y1-y2)/(x1-x2));  /*双灯条的中点斜率*/
                bool is_rect_slope = (center_slope < 0.31 || center_slope == 0); /*用斜率筛歪的过分的矩形*/


               float area_ratio1 = (area_k1 > area_k2 ? area_k1/area_k2 : area_k2/ area_k1);  // 面积之比


               float center_dis = centerDistance(rect_k1.center, rect_k2.center);
               float the_len = len_k1 > len_k2? len_k1 : len_k2;       // 选取两根中最长的
               float len_ratio = center_dis / the_len;
               // cout << "len_ratio :" << len_ratio << endl;
               bool len_condition = center_dis > the_len && len_ratio < 5.55 && len_ratio > 1.5;   // 适应性不高，待修改
               bool angle_limit = fabs(anglek1 - anglek2) < 12.25;  // 两灯条的角度差，大约差12.25
              // bool angle_limit = angle_limited > 15 && angle_limited < 55; // 此条已弃
                bool AREA_RATIO = center_dis * the_len > 180 && area_ratio1 < 1.86;  // 面积条件

            if (angle_limit && is_rect_slope && len_condition && AREA_RATIO )  // 角度极限待修改，因为假板好像角度差也不大
            {

                //rectangle(src, boundRect[k1].tl(), boundRect[k1].br(), Scalar(0, 255, 255), 2, 8, 0); //画灯条的
                //rectangle(src, boundRect[k2].tl(), boundRect[k2].br(), Scalar(0, 255, 255), 2, 8, 0); // 也是画灯条的

                RotatedRect obj_rect = boundingRRect(rect_k1, rect_k2);
                double w = obj_rect.size.width;
                double h = obj_rect.size.height;
                double wh_ratio = w / h;
                double rect_area = w *h;
                cout << "rect_area:" << rect_area << endl;
                        /* 长宽比不符的情况*/
                bool ratio_condition_1 = wh_ratio > 4.2;  // 大于大的
                bool ratio_condition_2 = wh_ratio < 1.25;  // 小于小的
                bool angle_condition = obj_rect.size.height > obj_rect.size.width;  // 高比宽大的
                bool rect_area_condition = rect_area < 160;  // 面积太小的不要

               if ( ratio_condition_1 && angle_condition && ratio_condition_2 && rect_area_condition ){continue;} // 不合条件的

                  Point2f vertice[4];
                  obj_rect.points(vertice);

                  /*取两灯条的中点坐标，进行第三次筛选*/

                  mid_point[0] = rect_k1.center;
                  mid_point[1] = rect_k2.center;

                  middle_point.push_back(mid_point);  // 把暂时满足的点推进去
                  kalman_point=kalman.point_Predict(g_runtime,obj_rect.center);
                  // object.push_back(obj_rect);  // 满足条件的进容器,暂弃
                  putText(src,to_string(obj_rect.angle), obj_rect.center, CV_FONT_NORMAL, 1, Scalar(0, 255, 0), 2);
                 // // cout << obj_rect.size.width << endl;
                 // 测试装甲板用，理论上可以框但会框假的以及真假一起框 //
                  for (int i = 0; i < 4; i++)  // 初代筛选结果
                  {
                     line(src, vertice[i], vertice[(i + 1) % 4], Scalar(255, 255, 255), 2);
                     circle(src, obj_rect.center, 2, Scalar(255, 0, 0));
                     circle(src, kalman_point, 8, Scalar(0, 255, 255), 3);
                  }

            }
        }
    }



//-- -----------------------第三次遍历整个矩形，这个要改进，效果奇差，改进方法准备用遍历两个灯条的点--------------------------------------------------//

//    for (size_t k3 = 0; k3 < (int) object.size(); k3++)  /*在框好的装甲中选可靠的,能识别但速度上奇慢,效果不佳，待修改*/
//    {
//        if (object.size() <= 1)
//        {
//            break;
//        }
//        if(object[k3].size.height > object[k3].size.width) break;

//        float the_W_H = object[k3].size.width / object[k3].size.height;
//        float object_area = object[k3].size.height * object[k3].size.height;
//        bool area_condition = object_area > 150;    // 面积筛选
//        bool the_len = object[k3].size.width > object[k3].size.height; // 宽比高长，待改进，因为上面好像筛过了
//        bool WHratio = (the_W_H < 4.1);  // 待改进，因为宽高比测的不准，在近距离会崩
//        bool rect_angle = fabs(object[k3].angle) < 13.51;  // 矩形角度不能太歪，用来筛一些飘的,已废，因为我搞错角度了
//        if( 1 )
//        {
//            Point2f vertice[4];
//            object[k3].points(vertice);
//            putText(result,to_string(object[k3].angle), object[k3].center, CV_FONT_NORMAL, 1, Scalar(0, 255, 0), 2);
//            // // cout << obj_rect.size.width << endl;
//            // 效果不佳，因为可以框的矩形少之又少,待后期改进，不过后期应该会再筛一遍灯条的点，然后找最优解 //
//             cout << the_W_H << endl;
//            for (int i = 0; i < 4; i++)
//            {
//                line(result, vertice[i], vertice[(i + 1) % 4], Scalar(255, 255, 255), 1);
//                circle(result, object[k3].center, 2, Scalar(255, 255, 255));
//            }


//        }

//    }



    // for(size_t k3 = 0; k3 <= (int) middle_point.size();k3++)
    // {
    //     float dis_point = centerDistance(middle_point[k3][0], middle_point[k3][1]);
    //     float the_point_slope = fabs(middle_point[k3][0].y - middle_point[k3][1].y) / fabs(middle_point[k3][0].x - middle_point[k3][1].x);

    //     bool true_slope = the_point_slope <= 0.31;
    //     bool dis_condition = dis_point < 5.8;
    //     if(true_slope && dis_condition)
    //     {
    //         line(result,middle_point[k3][0],middle_point[k3][1],Scalar(0,0,255),2,8);  // 最终矩形
    //     }

    // }



//------------------------------------------------------------------------------------------------------//




    imshow("src", src);
    imshow("result", result);
    //imshow("dst", dst);


        char c = waitKey(1);
         if(c == 27)
            break;



        int a = waitKey(0);
        if ((char)a == 32)   // 卡帧操作,让它停在一帧画面
            continue;


        if ((char)a == 27)
            break;

}
 return 0;

}
