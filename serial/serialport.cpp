/**
 * @file serialport.cpp
 * @author GCUROBOT-Visual-Group (GCUROBOT_WOLF@163.com)
 * @brief RM 2019 步兵视觉串口部分源文件
 * @version 1.1
 * @date 2019-05-01
 * @copyright Copyright (c) 2019 GCU Robot Lab. All Rights Reserved.
 */
#include "serialport.h"

int SerialPort::fd;
char SerialPort::g_buf[BUFF_LENGTH];
char SerialPort::g_buf_temp[BUFF_TEMP_LENGTH];
char SerialPort::g_rec_buf[REC_BUFF_LENGTH];

SerialPort::SerialPort()
{
    cout<<"The Serial set ......"<<endl;
}


SerialPort::~SerialPort(void)
{
    if (!close(fd))
        printf("Close Serial Port Successful\n");
}


void SerialPort::serialSet()
{
    //const char* DeviceName[4] = {"", "/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyUSB2"};

     /* WARNING :  终端设备默认会设置为控制终端，因此open(O_NOCTTY不作为控制终端)
     * Terminals'll default to be set as Control Terminals
     */
    struct termios newstate;//通过设置termios类型的数据结构中的值和使用一小组函数调用，你就可以对终端接口进行控制。
     /*打开串口*/
    bzero(&newstate, sizeof(newstate));//清零
    fd=open(PORT_NAME, O_RDWR|O_NONBLOCK|O_NOCTTY|O_NDELAY); //开串口，就像文件操作

    if (fd == -1){
        perror("Can't Open Serial Port\n");// 输出错误
    }else{
        printf("Open Serial Port %s Successful\n");
    }

    /*设置发送波特率*/
    cfsetospeed(&newstate, B115200);
    cfsetispeed(&newstate, B115200);

    //本地连线, 取消控制功能 | 开始接收
    newstate.c_cflag |= CLOCAL | CREAD;
    //设置字符大小
    newstate.c_cflag &= ~CSIZE;
    //设置停止位1
    newstate.c_cflag &= ~CSTOPB;
    //设置数据位8位
    newstate.c_cflag |= CS8;
    //设置无奇偶校验位，N
    newstate.c_cflag &= ~PARENB;

    /*阻塞模式的设置*/
    newstate.c_cc[VTIME]=0;  // 设置超时时间
    newstate.c_cc[VMIN]=0;

    tcflush(fd,TCIOFLUSH);   // 清空输入端

    tcsetattr(fd, TCSANOW, &newstate);  // 让设置立刻生效
}

/**
*  @brief: 串口数据读取函数
**/
void SerialPort::receiveData()//接收数据
{
    memset(g_rec_buf, '0', REC_BUFF_LENGTH); //清空缓存
    char rec_buf_temp[10];   //数据10位
    memset(rec_buf_temp,'0',sizeof(rec_buf_temp)); //清0
    read(fd,rec_buf_temp,sizeof(rec_buf_temp)); // 读取数据
    for(int i = 0; i < (int)sizeof(rec_buf_temp); ++i) // 接收数据位
    {
        if(rec_buf_temp[i]=='S' && rec_buf_temp[i+sizeof(g_rec_buf)-1] == 'E')  // 获取有效数据
        {
            for(int j = 0;j<((int)sizeof(g_rec_buf));++j)
            {
                g_rec_buf[j] = rec_buf_temp[i+j];  // 把临时数据存到g_rec_buf中
            }
            break;  // 每次只读取一串有效数据
        }
    }
    tcflush(fd,TCIFLUSH);  // 清空文件数据
    rec_buf_temp[10] = '\0';   // 数据末尾

    cout<<"  rec_buf_temp: "<< rec_buf_temp << endl;
    cout<<"  g_rec_buf: "<< g_rec_buf << endl;
}



/**
*  @brief: 串口数据处理函数
**/
int SerialPort::databitProcessing(int bit)
{
    int return_value = NULL;  //数据选择
    if(g_rec_buf[0] == 'S' && g_rec_buf[sizeof(g_rec_buf)-1] == 'E') // 判断发回来的数据是否有效
    {
        switch (bit)
        {
        case 1:
        {
            //处理第1位数据，模式选择
            switch (g_rec_buf[1])
            {
            case '1': return_value = support_shooting_mode; break;
            case '2': return_value = energu_mode;           break;
            default:  return_value = support_shooting_mode; break;
            }
        }   break;
        case 2:
        {
            //处理第2位数据，大小装甲
            switch (g_rec_buf[2])
            {
            case '1': return_value = little_armor;break;
            case '2': return_value = big_armor;   break;
            default: return_value = little_armor; break;
            }

        }   break;
        case 3:
        {
            //处理第3位数据，敌方装甲颜色
            switch (g_rec_buf[3])
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

        }   break;
        }
    }
    return return_value;
}


/**
*  @brief: 串口数据发送函数
**/
void SerialPort::sendData(int x, int y){
    sprintf(g_buf_temp, "%c%03d%03d", 'S', x, y);
    uint8_t CRC = Check_CRC8(g_buf_temp, sizeof(g_buf_temp));
    sprintf(g_buf, "%c%03d%03d%03d%c", 'S', x, y, CRC, 'E');
    write(fd, g_buf, sizeof(g_buf));
    cout << "g_buff:" << g_buf << endl;
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
