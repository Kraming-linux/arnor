#include <iostream>
#include <vector>
#include <cmath>
#include "CameraApi.h"
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;


char myDiscern(Mat n)
{
    if(1.5*n.cols<n.rows)
    {
        return '1';
    }
    int x_half = n.cols/2;
    int y_one_third = n.rows/3;
    int y_two_third = n.rows*2/3;
    int a = 0,b = 0,c = 0,d = 0,e = 0,f = 0,g = 0;

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
        if(j<x_half)
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

    if(a==1 && b==1 && c==1 && d==1 && e==1 && f==1 && g==0)
    {
        return '0';
    }
    else if(a==1 && b==1 && c==0 && d==1 && e==1 && f==0 && g==1)
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
    else if(a==1 && b==1 && c==1 && d==0 && e==0 && f==0 && g==0)
    {
        return '7';
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
//        return '1';
//    }
    else
    {
        printf("[error_%d_%d_%d_%d_%d_%d_%d]\n",a,b,c,d,e,f,g);
        return 'e';
    }

    // cout<<"MyDiscern:OK"<<endl;
}

int main()
{
    Mat src_img = imread("/home/kraming/pictures/4.png");
    cvtColor(src_img, src_img, COLOR_BGR2GRAY);
    threshold(src_img, src_img, 25, 255, THRESH_BINARY);
    cout << "num:" << myDiscern(src_img) << endl;
    while (1) {
        imshow("src", src_img);
        char c = waitKey(1);
        if(c == 27)
            break;
    }
    return 0;
}
