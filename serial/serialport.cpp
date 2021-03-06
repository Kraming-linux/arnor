/**
 * @file serialport.cpp
 * @brief RM 2020 串口部分文件
 * @version 1.1
 * @date 2020-1-03
 * @copyright Copyright (c) 2020 GCU Robot Lab. All Rights Reserved.
 */
#include "serialport.h"


int SerialPort::fd;
char SerialPort::send_buf[BUFF_LENGTH];
char SerialPort::send_buf_temp[BUFF_TEMP_LENGTH];
char SerialPort::rec_buf[REC_BUFF_LENGTH];


SerialPort::SerialPort(void)
{
    fd = open(PORT_NAME, O_RDWR|O_NONBLOCK|O_NOCTTY|O_NDELAY); //开串口，就像文件操作
    if (fd == -1){
        perror("Can't Open Serial Port");// 输出错误
    }else{
        cout << "open" << PORT_NAME << " successful"<<endl;
    }

    struct termios SerialData;//通过设置termios类型的数据结构中的值和使用一小组函数调用，你就可以对终端接口进行控制。
     /*打开串口*/
    bzero(&SerialData, sizeof(SerialData));//清零

    /*设置发送波特率*/
    cfsetospeed(&SerialData, B115200);  // 发送
    cfsetispeed(&SerialData, B115200);  // 接收

    //本地连线, 取消控制功能 | 开始接收
    SerialData.c_cflag |= CLOCAL | CREAD;
    //设置字符大小
    SerialData.c_cflag &= ~CSIZE;
    //设置停止位1
    SerialData.c_cflag &= ~CSTOPB;
    //设置数据位8位
    SerialData.c_cflag |= CS8;
    //设置无奇偶校验位，N
    SerialData.c_cflag &= ~PARENB;

    /*阻塞模式的设置*/
    SerialData.c_cc[VTIME]=0;  // 设置超时时间
    SerialData.c_cc[VMIN]=0;

    tcflush(fd,TCIOFLUSH);   // 清空输入端

    tcsetattr(fd, TCSANOW, &SerialData);  // 让设置立刻生效

}


SerialPort::~SerialPort(void)
{
    if (!close(fd))
        printf("Close Serial Port Successful\n");
    else
        close(fd);
}


/**
*  @brief: 串口数据读取函数
**/
void SerialPort::receiveData()//接收数据
{
    memset(rec_buf, '0', REC_BUFF_LENGTH); //清空缓存
    char rec_buf_temp[10];   //数据10位
    memset(rec_buf_temp,'0',sizeof(rec_buf_temp)); //清0
    read(fd,rec_buf_temp,sizeof(rec_buf_temp)); // 读取数据
    for(int i = 0; i < (int)sizeof(rec_buf_temp); ++i) // 接收数据位
    {
        if(rec_buf_temp[i]=='S' && rec_buf_temp[i+sizeof(rec_buf)-1] == 'E')  // 获取有效数据
        {
            for(int j = 0;j<((int)sizeof(rec_buf));++j)
            {
                rec_buf[j] = rec_buf_temp[i+j];  // 把临时数据存到g_rec_buf中
            }
            break;  // 每次只读取一串有效数据
        }
    }
    tcflush(fd,TCIFLUSH);  // 清空文件数据
    rec_buf_temp[10] = '\0';   // 数据末尾

    cout<<"rec_buf_temp: "<< rec_buf_temp << endl;
    cout<<"rec_buf: "<< rec_buf << endl;

}



/**
*  @brief: 串口数据处理函数
**/
int SerialPort::databitProcessing(int bit)
{
    int return_value = 0;  //数据选择
    if(rec_buf[0] == 'S' && rec_buf[sizeof(rec_buf)-1] == 'E') // 判断发回来的数据是否有效
    {
        switch (bit)
        {
        case 1:
        {
            //处理第1位数据，模式选择
            switch (rec_buf[1])
            {
            case '1': return_value = support_shooting_mode; break;
            case '2': return_value = energu_mode;           break;
            default:  return_value = support_shooting_mode; break;
            }
        }   break;
        case 2:
        {
            //处理第2位数据，大小装甲
            switch (rec_buf[2])
            {
            case '1': return_value = little_armor;break;
            case '2': return_value = big_armor;   break;
            default: return_value = little_armor; break;
            }

        }   break;
        case 3:
        {
            //处理第3位数据，敌方装甲颜色
            switch (rec_buf[3])
            {
            case '1': return_value = RED_ARMOR;  break;
            case '2': return_value = BLUE_ARMOR; break;
            default: return_value = ALL_ARMOR;   break;
            }
        }   break;
        case 4:
        {
            //处理第4位数据
        }   break;
        default:
        {
            return_value = 0;
        }   break;
        }
    }
    return return_value;
}


/**
*  @brief: 串口数据发送函数
**/
void SerialPort::sendData(int x, int y){
    int model =1;
    int model_select = 1;
    int depth = 1;
    sprintf(send_buf_temp, "%c%01d%01d%03d%03d%01d", 'S',model, model_select,  x, y, depth);
    uint8_t CRC = Check_CRC8(send_buf_temp, sizeof(send_buf_temp));
    sprintf(send_buf, "%c%01d%01d%03d%03d%01d%03d%c", 'S',model, model_select, x, y,depth, CRC, 'E');
    write(fd, send_buf, sizeof(send_buf));
    cout << "send_buff:" << send_buf << endl;
    usleep(5);  // 防止发送过快出现乱码
}


/**
*  @brief: CRC8校验
**/
uint8_t SerialPort::Check_CRC8(char *buf,uint16_t len)
{
    uint8_t check = 0;

    while(len--)
    {
        check = CRC8_TAB[check^(*buf++)];
    }

    return (check)&0x00ff;

}

