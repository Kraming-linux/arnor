#include "CameraApi.h"
//---工业相机中使用到opencv2.0的 IplImage 需要包含此头文件 ---*/
//#include "opencv2/imgproc/imgproc_c.h"
///*---工业相机中使用到opencv2.0的 cvReleaseImageHeader 需要包含此头文件 ---
#include <opencv2/opencv.hpp>
#include <iostream>

#define DRAW_RECT
//#define PUT_TEXT

#define RED_COLOR 1
#define BLUE_COLOR 2

using namespace std;
using namespace cv;

unsigned char           * g_pRgbBuffer;     //处理后数据缓存区

RotatedRect fit_Rrect(RotatedRect &rect_left,RotatedRect &rect_right);
float centerDistance(Point p1, Point p2);

int main()
{
    int                     iCameraCounts = 1;
    int                     iStatus=-1;
    tSdkCameraDevInfo       tCameraEnumList;
    int                     hCamera;
    tSdkCameraCapbility     tCapability;      //设备描述信息
    tSdkFrameHead           sFrameInfo;
    BYTE*			        pbyBuffer;
    IplImage *iplImage = NULL;
    int                     channel=3;
    BOOL                    AEstate=FALSE;

    CameraSdkInit(1);

    //枚举设备，并建立设备列表
    iStatus = CameraEnumerateDevice(&tCameraEnumList,&iCameraCounts);
    printf("state = %d\n", iStatus);

    printf("count = %d\n", iCameraCounts);
    //没有连接设备
    if(iCameraCounts==0){
        return -1;
    }

    //相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList,-1,-1,&hCamera);

    //初始化失败
    printf("state = %d\n", iStatus);
    if(iStatus!=CAMERA_STATUS_SUCCESS){
        return -1;
    }

    //获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera,&tCapability);

    //
    g_pRgbBuffer = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);
    //g_readBuf = (unsigned char*)malloc(tCapability.sResolutionRange.iHeightMax*tCapability.sResolutionRange.iWidthMax*3);

    cout<<CameraGetAeState(hCamera,&AEstate);
    cout<<CameraSetAeState(hCamera,FALSE);
    CameraSetExposureTime(hCamera,500);
    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    CameraPlay(hCamera);
    CameraReleaseImageBuffer(hCamera,pbyBuffer);
    /*其他的相机参数设置
    例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
         CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
         CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
         更多的参数的设置方法，，清参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
    */

    if(tCapability.sIspCapacity.bMonoSensor){
        channel=1;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_MONO8);
    }else{
        channel=3;
        CameraSetIspOutFormat(hCamera,CAMERA_MEDIA_TYPE_BGR8);
    }

    Mat src_img;
    Mat dst_img;
    Mat bin_img_gray;
    Mat bin_img_hsv;
    Mat gray_img;
    Mat hsv_img;
    //VideoCapture capture(0);
    while (1) {
        double t = (double)cv::getTickCount();
        if(CameraGetImageBuffer(hCamera,&sFrameInfo,&pbyBuffer,1000) == CAMERA_STATUS_SUCCESS)
        {
            CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer,&sFrameInfo);
            if (iplImage)
            {
                cvReleaseImageHeader(&iplImage);
            }
            iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth,sFrameInfo.iHeight),IPL_DEPTH_8U,channel);
            cvSetData(iplImage,g_pRgbBuffer,sFrameInfo.iWidth*channel);//此处只是设置指针，无图像块数据拷贝，不需担心转换效率
            src_img = cvarrToMat(iplImage,true);//这里只是进行指针转换，将IplImage转换成Mat类型
        }
        //cout<<" w:"<< src_img.cols<<"   h:"<<src_img.rows <<endl;
        //resize(src_img,src_img,Size(640,512),INTER_NEAREST);
        src_img.copyTo(dst_img);
        cvtColor(src_img,gray_img,COLOR_BGR2GRAY);
        cvtColor(src_img,hsv_img,COLOR_BGR2HSV);
        threshold(gray_img,bin_img_gray,40,255,THRESH_BINARY);
        Mat element = getStructuringElement(MORPH_RECT,Size(3,5));
        dilate(bin_img_gray,bin_img_gray,element);

        inRange(hsv_img,Scalar(),Scalar(),bin_img_hsv);

        int color = 1;
        if (color == RED_COLOR) { //己方颜色
            Mat mix_img_1;
            Mat mix_img_2;
            inRange(hsv_img, Scalar(156, 110, 100), Scalar(180, 255, 255), mix_img_1); //red
            inRange(hsv_img, Scalar(0, 110, 100), Scalar(10, 255, 255), mix_img_2);
            bin_img_hsv = mix_img_1 + mix_img_2;
        } else if (color == BLUE_COLOR) {
            //inRange(hsv_img, Scalar(80, 210, 130), Scalar(124, 255, 255), bin_img_1); //blue
            inRange(hsv_img, Scalar(80, 220, 60), Scalar(124, 255, 255), bin_img_hsv); //blue    rm_use
        }
        medianBlur(bin_img_hsv,bin_img_hsv,3);

        vector <RotatedRect> candidate_light;
        vector <vector <float> > candidate_light_param;
        vector < vector < Point > > contours_gray;
        vector < Vec4i > hierarchy_gray;
        findContours(bin_img_gray, contours_gray, hierarchy_gray, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));
        vector < vector < Point > > contours_hsv;
        vector < Vec4i > hierarchy_hsv;
        findContours(bin_img_hsv, contours_hsv, hierarchy_hsv, RETR_EXTERNAL, CHAIN_APPROX_NONE, Point(0, 0));
        for (size_t i = 0; i < contours_gray.size();++i){
            for (size_t j = 0; j < contours_hsv.size();++j){
                if (contours_hsv[j].size() < 6){
                    continue;
                }
                RotatedRect R_rect = fitEllipse(contours_hsv[j]);
                if(pointPolygonTest(contours_gray[i],R_rect.center,true) >= 0){
                    float width = R_rect.size.width > R_rect.size.height ? R_rect.size.height:R_rect.size.width;
                    float height =R_rect.size.width > R_rect.size.height ? R_rect.size.width:R_rect.size.height;
                    float rect_area = width * height;
                    float w_h_ratio = width / height;
                    if ((w_h_ratio < 0.3) /*高宽比,角度筛选形状符合要求的轮廓*/
                            && ((0< R_rect.angle && R_rect.angle<45)||(135<R_rect.angle && R_rect.angle<180))){

                        candidate_light.push_back(R_rect);
                        vector<float> param(3);
                        param[0] = rect_area;
                        param[1] = w_h_ratio;
                        param[2] = 0;

#ifdef DRAW_RECT
                        Point2f vtx[4];
                        R_rect.points(vtx);
                        for (int j = 0; j < 4; j++){
                            line(dst_img, vtx[j], vtx[(j + 1) % 4], Scalar(200, 0, 0),2,8,0);
                        }
#endif
#ifdef PUT_TEXT
                        putText(dst_img, to_string(R_rect.angle),R_rect.center,FONT_HERSHEY_PLAIN,1,Scalar(255, 255, 255),1,8,false);
                        //putText(dst_img, to_string(rect_area),vtx[0],FONT_HERSHEY_PLAIN,1,Scalar(255, 255, 255),1,8,false);
#endif
                    }
                }
            }
        }

        for (size_t i = 0;i<candidate_light.size();++i){
            for (size_t j = i+1;j<candidate_light.size();++j){
                float angle_left;
                float angle_right;
                float w1,h1,w2,h2;

                //区别出左右灯条
                if(candidate_light[i].center.x < candidate_light[j].center.x){
                    //灯条的角度
                    angle_left = candidate_light[i].angle;
                    angle_right = candidate_light[j].angle;
                    //灯条的宽高
                    w1 = MIN(candidate_light[i].size.height,candidate_light[i].size.width);
                    h1 = MAX(candidate_light[i].size.height,candidate_light[i].size.width);
                    w2 = MIN(candidate_light[j].size.height,candidate_light[j].size.width);
                    h2 = MAX(candidate_light[j].size.height,candidate_light[j].size.width);
                } else {
                    //灯条的角度
                    angle_right = candidate_light[i].angle;
                    angle_left = candidate_light[j].angle;
                    //灯条的宽高
                    w1 = MIN(candidate_light[j].size.height,candidate_light[j].size.width);
                    h1 = MAX(candidate_light[j].size.height,candidate_light[j].size.width);
                    w2 = MIN(candidate_light[i].size.height,candidate_light[i].size.width);
                    h2 = MAX(candidate_light[i].size.height,candidate_light[i].size.width);
                }
                //面积比例部分
                float area_ratio = MIN(w1*h1,w2*h2)/MAX(w1*h1,w2*h2);

                //高宽比
                float w_h_ratio_left = w1/h1;
                float w_h_ratio_right = w2/h2;

//                //高宽比之差
//                float ratio_diff = abs(w_h_ratio_left - w_h_ratio_right);
//                //高宽比之比
//                float ratio_ratio = MIN(w_h_ratio_left,w_h_ratio_right)/MAX(w_h_ratio_left,w_h_ratio_right);
                //灯条高度差
                float light_y_diff = fabs(candidate_light[i].center.y - candidate_light[j].center.y);
                bool is_height_diff_catch = (light_y_diff < (h1+h2)*0.25);

                //灯条间的距离
                float light_distance = centerDistance(candidate_light[i].center,candidate_light[j].center);
                bool light_distance_catch = (light_distance < MAX(h1,h2) * 5);

                //灯条之间角度的关系
                bool is_light_angle_catch;
                if((angle_left <90 && angle_right < 90) || (angle_left > 90 && angle_right > 90)){
                    //同侧
                    is_light_angle_catch = (fabs(angle_left-angle_right) <= 5);
                } else if(angle_left < 90 && angle_right > 90){
                    //--- \ / --- 内八
                    //angle_right = angle_right<90? angle_right:(180-angle_right);
                    is_light_angle_catch = (170 <= fabs(angle_left-angle_right) && fabs(angle_left-angle_right) <= 180);
                } else if(angle_left > 90 && angle_right < 90){
                    //--- / \ --- 外八
                    //angle_left = angle_left<90? angle_left:(180-angle_left);
                    is_light_angle_catch = (170 <= fabs(angle_left-angle_right) && fabs(angle_left-angle_right) <= 180);
                } else if(angle_left ==0 && angle_right != 0){
                    //左边竖直
                    //is_light_angle_catch = (fabs(angle_left-angle_right) <= 10);
                    is_light_angle_catch = (170 <= fabs(angle_left-angle_right) || fabs(angle_left-angle_right) <= 10);
                } else if(angle_left !=0 && angle_right == 0){
                    //右边竖直
                    //is_light_angle_catch = (fabs(angle_left-angle_right) <= 10);
                    is_light_angle_catch = (170 <= fabs(angle_left-angle_right) || fabs(angle_left-angle_right) <= 10);
                }

#ifdef PUT_TEXT
//                if(candidate_light[i].center.x > candidate_light[j].center.x){
//                    putText(dst_img, to_string(w_h_ratio_left),candidate_light[i].boundingRect().tl(),FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
//                    putText(dst_img, to_string(w_h_ratio_right),candidate_light[j].boundingRect().tl(),FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
//                } else {
//                    putText(dst_img, to_string(w_h_ratio_right),candidate_light[i].boundingRect().tl(),FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
//                    putText(dst_img, to_string(w_h_ratio_left),candidate_light[j].boundingRect().tl(),FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
//                }
//                putText(dst_img,to_string(ratio_diff),(candidate_light[i].center+candidate_light[j].center)*0.5,FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
//                putText(dst_img,to_string(ratio_ratio),
//                        Point((candidate_light[i].center.x+candidate_light[j].center.x)*0.5,(candidate_light[i].center.y+candidate_light[j].center.y)*0.5 + 20),
//                        FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
#endif
                light_distance_catch = true;
                if(area_ratio > 0
                        && is_height_diff_catch
                        && is_light_angle_catch
                        && light_distance_catch){
                    RotatedRect armor_rect = fit_Rrect(candidate_light[i],candidate_light[j]);
#ifdef DRAW_RECT
                    Point2f vtx[4];
                    armor_rect.points(vtx);
                    for (int j = 0; j < 4; j++){
                        line(dst_img, vtx[j], vtx[(j + 1) % 4], Scalar(100, 200, 100),2,8,0);
                    }
#endif
#ifdef PUT_TEXT
                    putText(dst_img,to_string(fabs(angle_left-angle_right)),
                            (candidate_light[i].center+candidate_light[j].center)*0.5,
                            FONT_HERSHEY_PLAIN,2,Scalar(255, 255, 255),1,8,false);
                    //putText(dst_img, to_string(armor_rect.angle),armor_rect.center,FONT_HERSHEY_PLAIN,1,Scalar(255, 255, 255),1,8,false);
#endif
                }
            }
        }

        cout <<"size: "<<candidate_light.size()<<endl;
        imshow("bin_img_gray",bin_img_gray);
        imshow("bin_img_hsv",bin_img_hsv);
        imshow("src_img",src_img);
        imshow("dst_img",dst_img);

        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        int fps = int(1.0 / t);
        cout << "FPS: " << fps<<endl;
        CameraReleaseImageBuffer(hCamera,pbyBuffer);
        int key = waitKey(1);
        if(char(key) == 27)break;
    }

    //CameraReleaseImageBuffer(hCamera,pbyBuffer);
    CameraUnInit(hCamera);
    //注意，现反初始化后再free
    free(g_pRgbBuffer);
    return 0;
}

float centerDistance(Point p1, Point p2) {
    float D = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    return D;
}

RotatedRect fit_Rrect(RotatedRect &rect_left,RotatedRect &rect_right){
    Point2f center = (rect_left.center+rect_right.center)*0.5;
    float center_slope = (rect_left.center.y-rect_right.center.y)/(rect_left.center.x-rect_right.center.x);
    float distance = centerDistance(rect_left.center,rect_right.center);
    float width_left =  MIN(rect_left.size.width,rect_left.size.height);
    float height_left = MAX(rect_left.size.width,rect_left.size.height);
    float width_right = MIN(rect_right.size.width,rect_right.size.height);
    float height_right = MAX(rect_right.size.width,rect_right.size.height);

    float W = distance-(width_left + width_right);
    float H = MAX(height_left,height_right)*2.27;
    float angle = atan(center_slope);
    RotatedRect Rrect = RotatedRect(center,Size2f(W,H),angle*180/CV_PI);
    return Rrect;
}
