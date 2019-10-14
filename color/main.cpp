#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;
void on_mouse(int EVENT, int x, int y, int flags, void* userdata);
int main()
{
 namedWindow("【display】");
 Mat src, dst;
 src = imread("/home/kraming/video/111.png");
 //cvtColor(src, dst, COLOR_RGB2HSV);
 setMouseCallback("【display】", on_mouse,&src);
 //以40ms刷新显示
 while (1)
 {
  imshow("【display】", src);
  char c = waitKey(10);
  if(c == 27)
      break;
 }

return 0;

}

void on_mouse(int EVENT, int x, int y, int flags, void* userdata) //鼠标事件
 {
  Mat hh;
  hh = *(Mat*)userdata;
  Point p(x, y);
  switch (EVENT)
  {
   case EVENT_LBUTTONDOWN:
   {

    printf("b=%d\t", hh.at<Vec3b>(p)[0]);  // 打印数值
    printf("g=%d\t", hh.at<Vec3b>(p)[1]);
    printf("r=%d\n", hh.at<Vec3b>(p)[2]);
    circle(hh, p, 2, Scalar(255),3);
   }
   break;

  }




 }




