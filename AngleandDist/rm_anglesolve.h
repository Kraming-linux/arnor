#ifndef RM_ANGLESOLVE_H
#define RM_ANGLESOLVE_H

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


enum armor_class{
    /**
      @param： SAMLL_ARMOR    小装甲
      @param: BIG_ARMOR      大装甲
      @param: LIGHT          灯条
    */
    SAMLL_ARMOR = 1,
    BIG_ARMOR = 2,
    LIGHT = 3,
};



enum armor_size{
    /**
      @param： SAMLL_ARMOR
      @param: BIG_ARMOR_SIZE_W      大装甲实际宽度
      @param: BIG_ARMOR_SIZE_H      大装甲实际高度
      @param: SMALL_ARMOR_SIZE_W    小装甲实际宽度
      @param: SMALL_ARMOR_SIZE_H    小装甲实际高度
      @param: LIGHT_SIZE_W          灯条实际宽度
      @param: LIGHT_SIZE_H          灯条实际高度
    */
    BIG_ARMOR_SIZE_W = 225,
    BIG_ARMOR_SIZE_H = 55,
    SMALL_ARMOR_SIZE_W = 125,
    SMALL_ARMOR_SIZE_H = 55,
    LIGHT_SIZE_W = 10,
    LIGHT_SIZE_H = 55,
};


class RM_AngleSolve
{
public:
    RM_AngleSolve();
    ~RM_AngleSolve();
public:
    float yaw;         // x轴偏移角度
    float pitch;      // y轴偏移角度
    float distance;  // 深度

    void solveAngle(RotatedRect &rect, int object_size);  // 结算角度和距离
    void sortVertex(RotatedRect & box);                   // 排序
    float solveDist(RotatedRect &rect, int object_size);  // 单纯结算距离
    Mat camera_ptz(Mat &t);
private:
    /* 记得把文件路径改对，这个标定文件 */
   //string file_path = "/home/kraming/workspace/qt/AngleandDist/param/cameraParams_infantry4.xml"; // 标定文件
    string file_path = "/home/kraming/workspace/qt/AngleandDist/param/cam.xml"; // 标定文件
    Mat cameraMatrix;  // 相机的内参矩阵
    Mat distCoeffs;    // 相机的畸变系数

    Mat rvec = Mat::zeros(3, 3, CV_64FC1);  // 输出的旋转向量。使坐标点从世界坐标系旋转到相机坐标系
    Mat tvec = Mat::zeros(3, 1, CV_64FC1);  // 输出的平移向量。使坐标点从世界坐标系平移到相机坐标系

    const float ptz_camera_x = 0;  // 相机在云台的坐标
    const float ptz_camera_y = 0;
    const float ptz_camera_z = 0;

    vector<Point2f> target_2D;   // 控制点在图像的坐标
    vector<Point3f> object_3D;   // 控制点在世界坐标系的坐标

};

#endif // RM_ANGLESOLVE_H
