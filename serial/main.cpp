#include <iostream>
#include "serialport.h"

using namespace std;

int main()
{
//    SerialPort::serialSet();

//    int x = 320;
//    int y = 240;


//    for(;;){
//        SerialPort::sendData(x, y);
//        cout << "send successful" << endl;
//    }
//    system("pause");

    char var[20] = "hellow,world";
    cout << "size: " << sizeof(var);

    return 0;
}
