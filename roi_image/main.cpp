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
//#define DEBUG_CONTOURS
#define CONTOUR_DIGIT

Point get_rect_center(Rect rect); /*rect类取中点的函数*/
bool TRUE_DIGIT(Mat ROI);  /*判断是否为数码管,暂未使用*/
char myDiscern(Mat n)   /*检测数码管的数字*/
{
    bool one_area = (n.rows * n.cols) < 190;
    if(one_area)  // 重点改进，要先判断1的情况，不然容易跟8搞错
    {
        return '1';
    }

    int x_half = n.cols / 2;       // 穿线法的竖线
    int y_one_third = n.rows / 3;  // 穿线法的第一条横线
    int y_two_third = n.rows*2/3;  // 穿线法的第二条横线
    int a = 0,b = 0,c = 0,d = 0,e = 0,f = 0,g = 0;  // 各数码管的标号，从a开始顺时针,看下面

    /*   a
      f     b
         g
      e     c
         d
     */

    for(int i = 0;i < n.rows;i++)
    {
        uchar *data=n.ptr<uchar>(i);
        if(i < y_one_third)
        {
            if(data[x_half] == 255) a = 1;
        }
        else if(i > y_one_third && i < y_two_third)
        {
            if(data[x_half] == 255) g = 1;
        }
        else
        {
            if(data[x_half] == 255) d = 1;
        }
    }
    for(int j = 0;j < n.cols;j++)
    {
        uchar *data=n.ptr<uchar>(y_one_third);
        if(j < x_half)
        {
            if(data[j] == 255) f = 1;
        }
        else
        {
            if(data[j]==255) b = 1;
        }
    }
    for(int j = 0;j < n.cols;j++)
    {
        uchar *data=n.ptr<uchar>(y_two_third);
        if(j < x_half)
        {
            if(data[j] == 255) e = 1;
        }
        else
        {
            if(data[j] == 255) c = 1;
        }
    }

//    if(a==1 && b==1 && c==1 && d==1 && e==1 && f==1 && g==0)
//    {
//        return '0';   // 本数码管没有0的情况，故舍去
//    }
    if(a==1 && b==1 && c==0 && d==1 && e==1 && f==0 && g==1)
    {
        return '2';
    }
    else if(a==1 && b==1 && c==1 && d==1 && e==0 && f==0 && g==1)
    {
        return '3';
    }
    else if(a==0 && b==1 && c==1 && d==0 && e==0 && f==1 && g==1)
    {
        return '4';
    }
    else if(a==1 && b==0 && c==1 && d==1 && e==0 && f==1 && g==1)
    {
        return '5';
    }
    else if(a==1 && b==0 && c==1 && d==1 && e==1 && f==1 && g==1)
    {
        return '6';
    }
    else if((a==1 && b==1 && c==1 && d==1 && e==0 && f==0 && g==0)
            || (a==1 && b==1 && c==1 && d==0 && e==0 && f==0 && g==0))
    {
        return '7';  // 因为数码管太小，有时难免误判
    }
    else if(a==1 && b==1 && c==1 && d==1 && e==1 && f==1 && g==1)
    {
        return '8';
    }
    else if(a==1 && b==1 && c==1 && d==1 && e==0 && f==1 && g==1)
    {
        return '9';
    }
//    else if(a==0 && b==1 && c==1 && d==0 && e==0 && f==0 && g==0)
//    {
//        return '1';  // 这个不算1，舍去
//    }
    else
    {
        printf("[error_%d_%d_%d_%d_%d_%d_%d]\n",a,b,c,d,e,f,g);
        return 'e';  // 打印错误信息，看是哪个数码管识别不到
    }

    // cout<<"MyDiscern:FXXKING OK"<<endl;
}

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

    CameraSetExposureTime(hCamera, 10000);  // 可能高曝光看的会比较好一点，因为他的数字比较少
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
    Mat element = cv::getStructuringElement(MORPH_ELLIPSE, Size(3, 5));
    Mat element_1 = cv::getStructuringElement(MORPH_RECT, Size(1, 3));
    Mat element_2 = cv::getStructuringElement(MORPH_ELLIPSE, Size(3, 3));

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
        double t = (double)cv::getTickCount();

#ifdef VIDEO
    capture >> temp_img;
#endif

    if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer,&sFrameInfo);
        if (iplImage)
        {
            cvReleaseImageHeader(&iplImage);  // 注意这个空...有地址符号的。
        }
        iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight),IPL_DEPTH_8U,channel);
        cvSetData(iplImage,g_pRgbBuffer,sFrameInfo.iWidth*channel);//此处只是设置指针，无图像块数据拷贝，不需担心转换效率
        temp_img = cvarrToMat(iplImage,true);//这里只是进行指针转换，将IplImage转换成Mat类型
    }

    resize(temp_img, temp_img, Size(640, 480));  // 此代码各参数仅在640X480分辨率有效
    //dst_img.create(temp_img.dims, temp_img.cols, temp_img.type());  // 模板匹配才用的
    GaussianBlur(temp_img, temp_img, Size(3, 3), 0);
    cvtColor(temp_img, gray_img, COLOR_BGR2GRAY);
    threshold(gray_img, bin_img, 50, 255, THRESH_BINARY);
    Canny(bin_img, bin_img, 60, 200);
    morphologyEx(bin_img, bin_img, MORPH_CLOSE, element);
    dilate(bin_img, bin_img, element_1);

    vector<vector<Point>> contours;   
    vector<Rect> big_num;  // 筛选大矩形的容器
    vector<Rect> digit_num;  // 筛选小数码容器
    vector<Point> rect_center;  // 大矩形的中点容器
    vector<Mat> rect_img;     // 大矩形的Mat容器

    findContours(bin_img, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
    for(size_t i = 0; i < contours.size(); i++)  // 寻找最外层轮廓
    {
        if(contours.size() <= 2) break;
         const Rect &num_rect = boundingRect(contours[i]);
#ifdef DEBUG_CONTOURS
        cout << "area:" << num_rect.area() << endl;
        rectangle(temp_img, num_rect, Scalar(0, 0, 255), 2);
        putText(temp_img, to_string(num_rect.area()), num_rect.tl(), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
#endif
        bool big_area_condition = num_rect.area() > 3000 && num_rect.area() < 10000;
        bool digit_area_condition = num_rect.area() > 136 && num_rect.area() <= 755;
        bool the_slope = fabs(num_rect.size().width / num_rect.size().height) <= 1.3;

        if(big_area_condition && the_slope)  // 利用面积大小和宽高比筛选
        {           
           big_num.push_back(num_rect);

        }
#ifdef CONTOUR_DIGIT
        if(the_slope  && digit_area_condition)
        {
           digit_num.push_back(num_rect);
        }
#endif
    }


    sort(digit_num.begin(), digit_num.end(),  //让数码管从左到右排序
         [](Rect & a1,Rect & a2){
             return a1.tl().x < a2.tl().x;}); // Lambda 用法


    for(size_t k = 0; k < big_num.size(); k++) /*遍历大数字*/
    {
        Point center = get_rect_center(big_num[k]);
        rect_center.push_back(center);  // 存点
        Mat roi_img = temp_img(big_num[k]); // 把rect截成ROI传入Mat变量
        rect_img.push_back(roi_img);
        //imshow("rect", rect_img[k]);
        rectangle(temp_img, big_num[k], Scalar(0, 255, 255), 2);
#ifdef SHOW_RECT
        cout << "center" << rect_center[k];
        char rect_name[60];
        sprintf(rect_name, "%d.png", k);
        imshow(rect_name, rect_img[k]);
#endif   // 开的话太卡，建议不用
    }



#ifdef CONTOUR_DIGIT    
    for(size_t l = 0; l < digit_num.size(); l++) /*遍历数码管*/        
    {
        Mat temp_roi = temp_img(digit_num[l]);  // 装ROI
        cvtColor(temp_roi, temp_roi, COLOR_BGR2GRAY);
        threshold(temp_roi, temp_roi, 60, 255, THRESH_BINARY);
        //dilate(temp_roi, temp_roi, element_1);
        //erode(temp_roi, temp_roi, element_1);
        imshow("fxxk", temp_roi);
        rectangle(temp_img, digit_num[l], Scalar(255, 0, 0), 1);
        cout << "num:" << myDiscern(temp_roi) << endl;  /*解决1的问题基本可以了*/
    }
#endif


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
        CameraReleaseImageBuffer(hCamera, pbyBuffer);  // 这句别忘记了,忘了后果很严重
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        int fps = int(1.0 / t);
        putText(temp_img, to_string(fps), Point(50, 50), FONT_HERSHEY_SIMPLEX, 30, Scalar::all(255));
        //cout << "FPS: " << fps<<endl;   /*帧率*/
        char c = waitKey(1);
        if( c == 27 )
           break;
    }
/*--------------注意，先反初始化再释放------------------*/
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


bool TRUE_DIGIT(Mat ROI)
{
    bool is_digit = false;

    int half_x = ROI.cols / 2;
    int one_thrid_y = ROI.rows / 3;
    int two_third_y = ROI.rows * 2/3;

    int a=0,b=0,c=0,d=0,e=0,f=0,g=0; // 数码管对应的管道，a开始顺时针,我也不想这样命名的

    for(int i=0;i<ROI.rows;i++) /*确定横着的数码管是否亮灯*/
       {
           uchar *data = ROI.ptr<uchar>(i);
           if(i<one_thrid_y)
           {
               if(data[half_x]==255) a=1;
           }
           else if(i>one_thrid_y&&i<two_third_y)
           {
               if(data[half_x]==255) g=1;
           }
           else
           {
               if(data[half_x]==255) d=1;
           }
       }

    for(int j=0;j<ROI.cols;j++) /*上半部分的数码管*/
       {
           uchar *data=ROI.ptr<uchar>(one_thrid_y);
           if(j<half_x)
           {
               if(data[j]==255) f=1;
           }
           else
           {
               if(data[j]==255) b=1;
           }
       }

    for(int j=0;j<ROI.cols;j++) /*下半部分的数码管*/
       {
           uchar *data=ROI.ptr<uchar>(two_third_y);
           if(j<half_x)
           {
               if(data[j]==255) e=1;
           }
           else
           {
               if(data[j]==255) c=1;
           }
       }

      if(ROI.cols < 2 * ROI.rows)
      {
        is_digit = true;
      }

      if(a==1 && b==1 && c==1 && d==1 && e==1 && f==1 && g==0)
      {
        is_digit = true; // 0
      }

      else if(a==1 && b==1 && c==0 && d==1 && e==1 && f==0 && g==1)
      {
          is_digit = true; // 2
      }
      else if(a==1 && b==1 && c==1 && d==1 && e==0 && f==0 && g==1)
      {
          is_digit = true; // 3
      }
      else if(a==0 && b==1 && c==1 && d==0 && e==0 && f==1 && g==1)
      {
          is_digit = true; // 4
      }
      else if(a==1 && b==0 && c==1 && d==1 && e==0 && f==1 && g==1)
      {
          is_digit = true; // 5
      }
      else if(a==1 && b==0 && c==1 && d==1 && e==1 && f==1 && g==1)
      {
          is_digit = true; // 6
      }
      else if(a==1 && b==1 && c==1 && d==0 && e==0 && f==0 && g==0)
      {
          is_digit = true; // 7
      }
      else if(a==1 && b==1 && c==1 && d==1 && e==1 && f==1 && g==1)
      {
          is_digit = true; // 8
      }
      else if(a==1 && b==1 && c==1 && d==1 && e==0 && f==1 && g==1)
      {
          is_digit = true; // 9
      }
      else
      {
         is_digit = false; // none
      }

      return is_digit;
}
