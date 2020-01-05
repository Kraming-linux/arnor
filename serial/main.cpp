#include <iostream>
#include "serialport.h"

using namespace std;

int main()

{


    SerialPort serial;

    int x = 320;
    int y = 240;

    for(int i =0;i < 20; i++){

        waitKey(1000);
        serial.receiveData();
        serial.sendData(x, y);
//        SerialPort::receiveData();
//        SerialPort::sendData(x, y);

        cout << "send successful" << endl;

    }


    return 0;
}
