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

using namespace cv;
using namespace std;

unsigned char     *g_pRgbBuffer;

int main()
{
     cout << "Hello World!" << endl;

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

     CameraSetExposureTime(hCamera, 3000); // 调曝光
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

     Mat src_img;

     while (1)
     {
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

         resize(src_img, src_img, Size(640, 480));
         imshow("src_img", src_img);

         CameraReleaseImageBuffer(hCamera, pbyBuffer);  // 这句别忘记了
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
