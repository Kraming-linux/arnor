#include <iostream>
#include "rm_anglesolve.h"
using namespace std;

int main()
{
    RM_AngleSolve angel;
    /* 假装出一个旋转矩形 */
//    RotatedRect rect;
//    rect.size.width = 200;
//    rect.size.height = 60;
//    rect.angle = 0;
//    rect.center = Point(320, 240);
//    /*----------------*/

//    angel.solveAngle(rect, 2);

//    /* 方法一：调用pnp结算角度和距离，记得转成整数，也注意正负 */
//    float yaw = angel.yaw;
//    float pitch = angel.pitch;
//    cout << "yaw:" << yaw << endl; // 就是x轴角度
//    cout << "pitch:" << pitch << endl;  // y轴角度
//    float dist = angel.distance;   // 距离
//    cout << "dist:" << dist << endl;

    /* 方法二：不用pnp，直接拟合来计算，现在还不能用 */
    //float dist_2 = angel.solveDist(rect, 1);
    //cout << "dist_2:" << dist_2 << endl;
    /*---------------------------*/

    cout << "Built with OpenCV " << CV_VERSION << endl;

       VideoCapture capture(0);    // 打开摄像头
       if(!capture.isOpened())    // 判断是否打开成功
       {
           cout << "open camera failed. " << endl;
           return -1;
       }

       while(true)
       {
           Mat frame;

           Mat hsv_img;
           Mat bin_img;
           Mat bin_img1;
           capture >> frame;    // 读取图像帧至frame
           Mat copy_img = frame.clone();
           cvtColor(frame, hsv_img, COLOR_BGR2HSV);

           inRange(hsv_img, Scalar(0, 43, 26), Scalar(10, 250, 250), bin_img);
           inRange(hsv_img, Scalar(156, 43, 26), Scalar(180, 250, 250), bin_img1);

           bin_img = bin_img | bin_img1;



           threshold(bin_img, bin_img, 30, 255, THRESH_BINARY);



           vector < vector < Point > > contours;
           vector < RotatedRect > rotate_rect;

           findContours(bin_img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

           for(int i = 0; i < contours.size(); i++){
               RotatedRect rect = minAreaRect(contours[i]);

               if (rect.size.height > rect.size.width){
                   cv::Point2f points[4];
                   rect.points(points);
                   // angel.solveAngle(rect, 1);
                   rotate_rect.push_back(rect);
//                   for(size_t i=0;i<4;i++)
//                   {
//                       line(copy_img,points[i] ,points[(i+1)%4],Scalar(0,255,255),1,4,0);//黄色
//                   }

                  // cout << 'dist:' << angel.distance << endl;
               }
           }



           for(int j = 0; j < rotate_rect.size(); j++){
               RotatedRect rect2 = rotate_rect[j];
               angel.solveAngle(rect2, 1);
               Point2f po[4];

               rect2.points(po);
               for(size_t i=0;i<4;i++)
               {
                   line(copy_img,po[i] ,po[(i+1)%4],Scalar(0,255,255),1,4,0);//黄色
               }

           }


           cout << 'dist:' << angel.distance << endl;


           imshow("bin", bin_img);
           imshow("camera", copy_img);
           char c = waitKey(1);

           if(c == 27)		// delay 30 ms等待按键
           {
               break;
           }
       }






    cout << "Hello World!" << endl;
    return 0;
}


