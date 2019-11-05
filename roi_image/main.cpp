#include <iostream>
#include <vector>
#include <cmath>
#include "CameraApi.h"
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>


using namespace std;
using namespace cv;


//#define VIDEO
//#define TEMPLATE_MATCH
//#define SHOW_RECT


Point get_rect_center(Rect rect); /*rect类取中点的函数*/
unsigned char          *g_pRgbBuffer;   // 处理后数据缓存区
int main()
{


#ifdef TEMPLATE_MATCH
    //Mat temp_img = imread("/home/kraming/pictures/temp.png");  // 测试用
    double *minVal = 0;
    double *maxVal = 0;
#endif




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

    CameraSdkInit(1);   // 工业相机初始化

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

    /*初始化失败*/
    if(iStatus != CAMERA_STATUS_SUCCESS){
        return -1;
    }


    /*获得相机的特性描述结构体，此结构体中包含了相机可设置的各种参数的范围信息，决定了相关函数的参数*/
    CameraGetCapability(hCamera, &tCapability);

    g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    cout << CameraGetAeState(hCamera, &AEstate);
    cout << CameraSetAeState(hCamera, FALSE);

    CameraSetExposureTime(hCamera, 3000);
    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像*/
    CameraPlay(hCamera);
    CameraReleaseImageBuffer(hCamera, pbyBuffer);
    /*其他的相机参数设置*//*
    例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
         CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
         CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
         更多的参数的设置方法，，请参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
    /**/


    if(tCapability.sIspCapacity.bMonoSensor){
        channel = 1;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_MONO8); // 单通道
    }else {
        channel = 3;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_BGR8); // 三通道
    }


    Mat temp_img;
    Mat gray_img;
    Mat bin_img;
    Mat dst_img;
    Mat element = cv::getStructuringElement(MORPH_ELLIPSE, Size(3, 4));


#ifdef VIDEO
    VideoCapture capture(2);
    if(!capture.isOpened())
    {
        cout << "please open again..." << endl;
        return -1;
    }
#endif


#ifdef TEMPLATE_MATCH
    vector<Mat>srcs(9);  // 存放数字的容器
    srcs[0] = imread("/home/kraming/pictures/1.png");
    srcs[1] = imread("/home/kraming/pictures/2.png");
    srcs[2] = imread("/home/kraming/pictures/3.png");
    srcs[3] = imread("/home/kraming/pictures/4.png");
    srcs[4] = imread("/home/kraming/pictures/5.png");
    srcs[5] = imread("/home/kraming/pictures/6.png");
    srcs[6] = imread("/home/kraming/pictures/7.png");
    srcs[7] = imread("/home/kraming/pictures/8.png");
    srcs[8] = imread("/home/kraming/pictures/9.png");
#endif


    while(1)
    {

#ifdef VIDEO
    capture >> temp_img;
#endif

    if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer,&sFrameInfo);
        if (iplImage)
        {
            cvReleaseImageHeader(&iplImage);  // 注意这个空...
        }
        iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight),IPL_DEPTH_8U,channel);
        cvSetData(iplImage,g_pRgbBuffer,sFrameInfo.iWidth*channel);//此处只是设置指针，无图像块数据拷贝，不需担心转换效率
        temp_img = cvarrToMat(iplImage,true);//这里只是进行指针转换，将IplImage转换成Mat类型
    }

    resize(temp_img, temp_img, Size(640, 480));  // 此代码各参数仅在640X480分辨率有效
    dst_img.create(temp_img.dims, temp_img.cols, temp_img.type());  // 模板匹配才用的
    cvtColor(temp_img, gray_img, COLOR_BGR2GRAY);
    threshold(gray_img, bin_img, 42, 255, THRESH_BINARY);
    morphologyEx(bin_img, bin_img, MORPH_CLOSE, element);
    dilate(bin_img, bin_img, element);

    vector<vector<Point>> contours;
    vector<Rect> big_num(contours.size());  // 筛选大矩形的容器
    vector<Rect> digit_num(contours.size());  // 筛选小数码容器
    vector<Point> rect_center;  // 大矩形的中点容器
    vector<Mat> rect_img;     // 大矩形的Mat容器

    findContours(bin_img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for(size_t i = 0; i < contours.size(); i++)  // 寻找最外层轮廓
    {
        if(contours.size() <= 1) break;
        const Rect num_rect = boundingRect(contours[i]);
        //rectangle(temp_img, num_rect, Scalar(0, 0, 255), 2);
        bool big_area_condition = num_rect.area() > 5000 && num_rect.area() < 40001;
        bool digit_area_condition = num_rect.area() > 75 && num_rect.area() <= 730;
        bool the_slope = num_rect.size().height / num_rect.size().width <= 1.3;
        bool logic_slope = num_rect.size().height >= num_rect.size().width;

        if(logic_slope && the_slope && big_area_condition)  // 利用面积大小和宽高比筛选
        {
           big_num.push_back(num_rect);
        }

        if(logic_slope  && digit_area_condition)
        {
           digit_num.push_back(num_rect);
        }

    }

    for(size_t k = 0; k < big_num.size(); k++) /*遍历大数字*/
    {
        Point center = get_rect_center(big_num[k]);
        rect_center.push_back(center);  // 存点
        Mat roi_img = temp_img(big_num[k]); // 把rect截成ROI传入Mat变量
        rect_img.push_back(roi_img);
        rectangle(temp_img, big_num[k], Scalar(0, 255, 255), 2);

#ifdef SHOW_RECT
        cout << "center" << rect_center[k];
        char rect_name[60];
        sprintf(rect_name, "%d.png", k);
        imshow(rect_name, rect_img[k]);
#endif   // 开的话太卡，建议不用
    }


    for(size_t l = 0; l < digit_num.size(); l++) /*遍历数码管*/
    {
        rectangle(temp_img, digit_num[l], Scalar(255, 0, 0), 1);
    }


#ifdef TEMPLATE_MATCH
    for(int j = 0; j < 9; j++)  // 循环匹配模板
    {
     //resize(srcs[j], srcs[j], Size(55, 60));  // 最好改一下size，这样精度会高一点
     matchTemplate(temp_img, srcs[j], dst_img, 1);  // 模板匹配,最后的参数是枚举，越高精度越好
     //normalize(dst_img, dst_img, 0, 1, 32);  // 矩阵归一化
     Point minPoint;
     Point maxPoint;
     minMaxLoc(dst_img, minVal, maxVal, &minPoint,&maxPoint);  // 返回最小点和最大点
     rectangle(temp_img, minPoint, cv::Point(minPoint.x + srcs[j].cols, minPoint.y + srcs[j].rows), Scalar(0,255,0), 1, 8);
   }
#endif


//        imshow("temp_img", temp_img);
        imshow("bin_img", bin_img);
        CameraReleaseImageBuffer(hCamera, pbyBuffer);  // 这句别忘记了
        char c = waitKey(1);
        if( c == 27 )
           break;
    }
/*--------注意，先反初始化再释放------------------*/
    CameraUnInit(hCamera);
    free(g_pRgbBuffer);
    return 0;
}


Point get_rect_center(Rect rect)  // 因为rect这个类是没有取中点的，所以要自己手动写一个
{
    Point center;
    center.x = rect.x + rect.size().width / 2;
    center.y = rect.y + rect.size().height / 2;
    return center;
}
