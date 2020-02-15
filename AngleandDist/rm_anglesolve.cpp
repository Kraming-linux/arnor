#include "rm_anglesolve.h"

RM_AngleSolve::RM_AngleSolve(){

    /* 读取标定文件 */
    FileStorage fd(file_path, FileStorage::READ);
    //fd["camera-matrix"] >> cameraMatrix;
    //fd["distortion"] >> distCoeffs;

    fd["cameraMatrix"] >> cameraMatrix;
    fd["distCoeffs"] >> distCoeffs;

    cout << cameraMatrix << endl;
    cout << distCoeffs << endl;

    cout << "ready" << endl;
}



void RM_AngleSolve::sortVertex(RotatedRect &box){
    Point2f vertex[4];
    Point2f left_up, left_down, right_up, right_down;
    box.points(vertex);

    /* 对旋转矩形的x排序 */
    sort(vertex, vertex + 4, [](const Point2f & p1, const Point2f & p2) { return p1.x < p2.x; });


    /* 对y进行排序 */
    if(vertex[0].y < vertex[1].y){
        left_up = vertex[0];
        left_down = vertex[1];
    } else {
        left_up = vertex[1];
        left_down = vertex[0];
    }

    if(vertex[2].y < vertex[3].y){
        right_up = vertex[2];
        right_down = vertex[3];
    } else {
        right_up = vertex[3];
        right_down = vertex[2];
    }

    target_2D.clear();
    target_2D.push_back(left_up);
    target_2D.push_back(right_up);
    target_2D.push_back(right_down);
    target_2D.push_back(left_down);

}



Mat RM_AngleSolve::camera_ptz(Mat &t){
    // 假设相机坐标系绕X轴你是逆时针旋转θ后与云台坐标系的各个轴向平行
    double theta = 0;
    // 旋转矩阵，自行看线性代数
    double r_data[] = {1,0,0,0,cos(theta),sin(theta),0,-sin(theta),cos(theta)};
    // 偏移量,设相机坐标系的原点在云台坐标系中的坐标为(x0,y0,z0),其实就是原点
    double t_data[] = {static_cast<double>(ptz_camera_x),static_cast<double>(ptz_camera_y),static_cast<double>(ptz_camera_z)};
    Mat r_camera_ptz(3,3,CV_64FC1,r_data);
    Mat t_camera_ptz(3,1,CV_64FC1,t_data);

    /* t是物体在世界坐标系下的坐标 */
    Mat position_ptz = r_camera_ptz * t - t_camera_ptz;

    /* 这样就获得了物体在相机坐标系下的坐标，z轴按网上的说法就是深度了 */
    return position_ptz;

}


/**
 * 结算pnp需要知道装甲板的真实宽高
 * 换句话说就是你还是要区分大小装甲板
 * 当然你也可以算灯条的，但没有测试，也不知道稳不稳
 * 如果能用灯条的话应该是可以不用区分大小装甲的
 * 这里打算弄个宽高比的逻辑来区分大小装甲
 * 最后的距离是mm为单位计算的
 * 也是可以直接用串口把数据发给下位机的
 * 请直接参考大三的代码
 * 但是发送的时候注意角度是有正负的
 * 并且要转成整型（角度和距离都要转）
 */

void RM_AngleSolve::solveAngle(RotatedRect &rect, int object_size){

    float half_x;  // 真实点的中点
    float half_y;

    /* 后期这里应该可以优化，通过矩形的宽高比来区分大小装甲 */
    if(object_size == SAMLL_ARMOR){
        half_x = SMALL_ARMOR_SIZE_W / 2;
        half_y = SMALL_ARMOR_SIZE_H / 2;
    } else if (object_size == BIG_ARMOR) {
        half_x = BIG_ARMOR_SIZE_W / 2;
        half_y = BIG_ARMOR_SIZE_H / 2;
    } else if (object_size == LIGHT) {
        half_x = LIGHT_SIZE_W / 2;
        half_y = LIGHT_SIZE_H / 2;
    } else {  /* 默认小装甲 */
        half_x = SMALL_ARMOR_SIZE_W / 2;
        half_y = SMALL_ARMOR_SIZE_H / 2;
    }

    object_3D.clear();
    object_3D.push_back(Point3f(-half_x, -half_y, 0));
    object_3D.push_back(Point3f(half_x, -half_y, 0));
    object_3D.push_back(Point3f(half_x, half_y, 0));
    object_3D.push_back(Point3f(-half_x, half_y, 0));

    sortVertex(rect);

    /* pnp方法结算 */
    solvePnP(object_3D, target_2D, cameraMatrix, distCoeffs, rvec, tvec);

    /* 坐标转换 */
    Mat ptz = camera_ptz(tvec);

    const double *_xyz = (const double *)ptz.data;
    double xyz[3] = {_xyz[0], _xyz[1] , _xyz[2]};

    pitch = atan2(xyz[1], xyz[2]);
    yaw = atan2(xyz[0], xyz[2]);

    /* 转成角度值 */
    yaw = yaw * 180 / 3.1415926;
    pitch = pitch * 180 / 3.1415926;

    /* 这个方法参考深大那边计算的，精度暂不知 */
//    distance = sqrt(tvec.at<double>(0, 0) * tvec.at<double>(0, 0) + tvec.at<double>(0, 1) * tvec.at<double>(0, 1)
//                           + tvec.at<double>(0, 2) * tvec.at<double>(0, 2));

    /* 这个是参考网上和大三代码的，其实和上面的数值也差不多，差5cm */
    distance = xyz[2];

}



/** 测距的第二种方法，求rect的短边（灯条的高度）来测距
 *  通过求某一个函数来拟合出距离
 *  精度的话我觉得可能这种会比较高
 *  但是需要数据去拟合
 *
 */
float RM_AngleSolve::solveDist(RotatedRect &rect, int object_size){
    int len = 0;
    if(rect.size.width > rect.size.height){
        len = rect.size.height;
    } else {
        len = rect.size.width;
    }

    // 这函数我瞎写的，因为没有数据，测完数据会把这个函数给换掉
    float dist = 9998 * pow(len, -1.8);

    return dist;

}


RM_AngleSolve::~RM_AngleSolve(){

}

