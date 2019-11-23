/**
 * @file main.cpp
 * @author Ming
 * @brief 队内赛
 * @version 1.0
 * @date 2019-11-16
 * @copyright
 */

#include <iostream>
#include <vector>
#include <cmath>
#include "CameraApi.h"   // 工业相机的API
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>


using namespace cv;
using namespace std;


#define INDUSTRIAL_CAMERA  // 启用工业相机
//#define USB_CAMERA         // 启用USB相机



int main()
{

    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

#ifdef INDUSTRIAL_CAMERA
     unsigned char     *g_pRgbBuffer;    // 缓存
    /*---------------设备描述信息--------------------*/
     int                    iCameraCounts = 1;
     int                    iStatus = -1;
     tSdkCameraDevInfo      tCameraEnumList;
     int                    hCamera;
     tSdkCameraCapbility    tCapability;
     tSdkFrameHead          sFrameInfo;
     BYTE*                  pbyBuffer;
     IplImage *iplImage = NULL;
     int                     channel = 3;
     BOOL                    AEstate = FALSE;
    /*-------------------------------------------*/

     CameraSdkInit(1);    // 工业相机SDK初始化
     iStatus = CameraEnumerateDevice(&tCameraEnumList, &iCameraCounts); // 枚举设备，并建立设备列表
     cout << "state:" << iStatus << endl;
     cout << "count:" << iCameraCounts << endl;
       /*设备未连接*/
     if(iCameraCounts == 0)
     {
         return -1;
     }

     /*相机初始化，初始化成功后才能调用其他相机相关的操作接口*/
     iStatus = CameraInit(&tCameraEnumList, -1, -1, &hCamera);
     cout << "state:" << iStatus << endl;


     if(iStatus != CAMERA_STATUS_SUCCESS)
     {
         cout << "fail" << endl;
         return -1;
     }

     /*获得相机的特性描述结构体，此结构体中包含了相机可设置的各种参数的范围信息，决定了相关函数的参数*/
     CameraGetCapability(hCamera, &tCapability);

     g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

     cout << CameraGetAeState(hCamera, &AEstate);
     cout << CameraSetAeState(hCamera, FALSE);

     CameraSetExposureTime(hCamera, 10000); // 调曝光，10000也相当于正常曝光了

     /*让SDK进入工作模式，开始接收来自相机发送的图像
     数据。如果当前相机是触发模式，则需要接收到
     触发帧以后才会更新图像*/
     CameraPlay(hCamera);
     CameraReleaseImageBuffer(hCamera, pbyBuffer);

     /*其他的相机参数设置
     例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
          CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
          CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
          更多的参数的设置方法，，请参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
     */

     if(tCapability.sIspCapacity.bMonoSensor){
         channel = 1;
         CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_MONO8); // 单通道
     }else {
         channel = 3;
         CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_BGR8); // 三通道
     }
#endif


#ifdef USB_CAMERA
     VideoCapture capture(2);

     if(!capture.isOpened()){
         cout << "try again" << endl;
     }
#endif

     Mat src_img;

     int flag = 0;  // 标志位，统计到多少帧
     while (1)
     {
         flag+=1;
         double t = (double)cv::getTickCount();

#ifdef INDUSTRIAL_CAMERA
         if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
         {
             CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer,&sFrameInfo);
             if (iplImage)
             {
                 cvReleaseImageHeader(&iplImage);  // 注意这个空...
             }
             iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight),IPL_DEPTH_8U,channel);
             cvSetData(iplImage,g_pRgbBuffer,sFrameInfo.iWidth*channel);//此处只是设置指针，无图像块数据拷贝，不需担心转换效率
             src_img = cvarrToMat(iplImage,true);//这里只是进行指针转换，将IplImage转换成Mat类型
         }
#endif

#ifdef USB_CAMERA
         capture >> src_img;
#endif

         resize(src_img, src_img, Size(640, 480));
         Mat gray_img, bin_img;
         cvtColor(src_img, gray_img, COLOR_BGR2GRAY);
         threshold(gray_img, bin_img, 60, 255, THRESH_BINARY);
         Mat dst_img = src_img.clone();
         vector<Mat> srcs;
         Mat blue_img,green_img,red_img;
         Mat blue_img_1,green_img_1,red_img_1;
         Mat bin_blue,bin_green, bin_red;
         split(src_img, srcs); // 通道分离法,三基色的话这种就比较好一点，就那三种颜色


         /*----------------------blue------------------------*/
         subtract(srcs[0], srcs[1], blue_img);
         subtract(srcs[0], srcs[2], blue_img_1);
         blue_img = blue_img & blue_img_1;
         threshold(blue_img, bin_blue, 60, 255, THRESH_BINARY);
         bin_blue = bin_blue & bin_img;
         erode(bin_blue, bin_blue, element);
         /*--------------------------------------------------*/

         /*----------------------greed-----------------------*/
         subtract(srcs[1], srcs[0], green_img);
         subtract(green_img, srcs[2], green_img_1);
         green_img = green_img + green_img_1;
         threshold(green_img, bin_green, 60, 255, THRESH_BINARY);
         bin_green = bin_green & bin_img;
         /*-------------------------------------------------*/

         /*----------------------red-----------------------*/
         subtract(srcs[2], srcs[0], red_img);
         subtract(srcs[2], srcs[1], red_img_1);
         red_img = red_img & red_img_1;
         threshold(red_img, bin_red, 60, 255, THRESH_BINARY);
         bin_red = bin_red & bin_img;
         /*-----------------------------------------------*/

         vector< RotatedRect > blue_rect;
         vector< vector< Point > > contours_b;
         findContours(bin_blue, contours_b, RETR_EXTERNAL, CHAIN_APPROX_NONE);/*分别在三张图上找轮廓*/
         for(size_t i = 0; i < contours_b.size();i++){
             RotatedRect BLUE_RECT = minAreaRect(contours_b[i]);  // 根据轮廓建立旋转矩形
             if(BLUE_RECT.size.area() > 700){    /*面积限制，筛掉一些杂点,下面两个是一样方法的*/
             blue_rect.push_back(BLUE_RECT); // 把符合蓝色轮廓的放进容器里
             }
         }


         vector< RotatedRect > green_rect;
         vector< vector< Point > > contours_g;
         findContours(bin_green, contours_g, RETR_EXTERNAL, CHAIN_APPROX_NONE);
         for(size_t i = 0; i < contours_g.size();i++){
             RotatedRect GREEN_RECT = minAreaRect(contours_g[i]);
             if(GREEN_RECT.size.area() > 700){
             green_rect.push_back(GREEN_RECT);
             }
         }


         vector< RotatedRect > red_rect;
         vector< vector< Point > > contours_r;
         findContours(bin_red, contours_r, RETR_EXTERNAL, CHAIN_APPROX_NONE);
         for(size_t i = 0; i < contours_r.size();i++){
             RotatedRect RED_RECT = minAreaRect(contours_r[i]);
             if(RED_RECT.size.area() > 700 ){
             red_rect.push_back(RED_RECT);
             }
         }


         for(size_t j = 0; j < blue_rect.size(); j++){
             Point2f vertice[4];
             blue_rect[j].points(vertice);
             for (int i = 0; i < 4; i++)  // 筛选结果
             {
                line(dst_img, vertice[i], vertice[(i + 1) % 4], Scalar(255, 255, 255), 2);
             }
         }

         for(size_t j = 0; j < green_rect.size(); j++){
             Point2f vertice[4];
             green_rect[j].points(vertice);
             for (int i = 0; i < 4; i++)  // 筛选结果
             {
                line(dst_img, vertice[i], vertice[(i + 1) % 4], Scalar(0, 0, 255), 2);
             }
         }

         for(size_t j = 0; j < red_rect.size(); j++){
             Point2f vertice[4];
             red_rect[j].points(vertice);
             for (int i = 0; i < 4; i++)  // 筛选结果
             {
                line(dst_img, vertice[i], vertice[(i + 1) % 4], Scalar(0, 255, 0), 2);
             }
         }

         /*判断的方法有很多，这里用的是对应颜色的格子的数量比较，颜色最多的一定是容器数量最多的*/
         bool send_blue_condition = blue_rect.size()>red_rect.size() && blue_rect.size()>green_rect.size();
         bool send_green_condition = green_rect.size()>red_rect.size() && green_rect.size()>blue_rect.size();
         bool send_red_condition = red_rect.size()>blue_rect.size() && red_rect.size() > green_rect.size();
         /*当然也可以用面积，周长，颜色像素统计的方法*/

         if(flag >= 5){   // 第五帧才判断和发送
             if(send_blue_condition){
                 cout << "blue" << endl;
             }
             else if (send_green_condition){
                 cout << "green" << endl;
             }
             else if (send_red_condition){
                 cout << "red" << endl;
             }else{
                 cout << "lack of condition" << endl;
             }
         }

         imshow("dst_img", dst_img);
         CameraReleaseImageBuffer(hCamera, pbyBuffer);  // 这句别忘记了
         t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

         int fps = int(1.0 / t);
         //cout << "FPS: " << fps<<endl;
         char c = waitKey(1);
         if( c == 27 )
            break;
     }

/*--------注意，先反初始化再释放-----------*/
     CameraUnInit(hCamera);
     free(g_pRgbBuffer);
/*------------------------------------*/
    return 0;
}
