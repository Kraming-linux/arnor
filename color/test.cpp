//#include <iostream>
//#include <cmath>
//#include <opencv2/core.hpp>
//#include <opencv2/video/tracking.hpp>
//#include <opencv2/highgui.hpp>
//#include <opencv2/imgproc.hpp>

//using namespace std;
//using namespace cv;


//int main()
//{

//    long red=0, deepgreen=0, black=0, yellow=0,blue=0,brown=0, pink=0, sky=0;
//    float B=0.0,G=0.0,R=0.0;
//    Mat src, dst;
//    src = imread("/home/kraming/video/111.png");
//    //resize(src, src, Size(300, 300));
//    //cvtColor(src, dst, COLOR_RGB2HSV);
//    for(int i = 0; i<src.rows; i++)
//    {
//        for(int j = 0; i < src.cols ;j++)
//        {

//            B = dst.at<cv::Vec3b>(j, i)[0];
//            G = dst.at<cv::Vec3b>(j, i)[1];
//            R = dst.at<cv::Vec3b>(j, i)[2];
//            if(R > 235 && G<=43 && B<=30){
//                red++;
//                //cout<<1<<endl;
//                //cout << "red:" <<red<<endl;
//            }
//            else if (G>=250) {
//                deepgreen++;
//                cout<<1<<endl;
//            }
//            else if (B>=107 && B<=118 && G>=237 && R>=239) {
//                yellow++;
//                //cout<<1<<endl;
//                //cout << "yellow" << yellow<<endl;
//            }
//            else if (B>=173 && B<=184 && G>=93 && G<=104 && R>=106 && R<=117) {

//                blue++;
//            }
//            else if (B>=148 && B<=157 && G==87 && R>=199 && R<=210) {
//                //cout<<1<<endl;
//                pink++;
//            }
//            else {
//                //cout<<1<<endl;
//                sky++;
//            }
//        }
//    }




//    while (true) {
//        imshow("src", src);
//        char c = waitKey(1);
//        cout << "red:" <<red<<endl;
//        cout << "deepgreen"<<deepgreen<<endl;
//        //system("pause");
//        if(c == 27)
//            break;
//    }



//    return 0;
//}
