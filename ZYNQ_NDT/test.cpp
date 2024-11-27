#include "main.h"

//   g++ -std=gnu++11 -o test test.cpp
// aarch64-linux-gnu-g++ -Wall -std=gnu++11 -pthread -o RailControl test.cpp AXIGPIO.cpp FMC4030.cpp -L. -lFMC4030 -lpthread
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main()
{
    int res = 0;
     std::cout << "-> 系统启动 ^_^ " << std::endl;
    // 初始化FPGA IP address
    int result = std::system(" ifconfig eth0 192.168.0.35 netmask 255.255.255.0 \n");if (result != 0) {printf("-> IP地址配置失败 !!!\n");}
    // res = FMC4030_Open_Device(0, "192.168.0.30", 8088);
    char ip[] = "192.168.0.30";
    res = FMC4030_Open_Device(0, ip, 8088);
    if (res != 0)
    {
        perror("FMC OPEN DEVICE Fail !!!\r\n");
        FMC4030_Close_Device(0);
        return -1;
    }
    printf("-> 导轨控制器开启成功 !!!\n");
    //Run X&Y Axis to Begin Point
    res = FMC4030_Jog_Single_Axis(0, 0, -128, 80, 160, 160, 2); //59 42
    if (res != 0)
    {
        perror("X Axis Run Fail !!!\r\n");
        return -2;
    }
    while (1){
        if(1 == FMC4030_Check_Axis_Is_Stop(0, 0)){
                printf("-> X轴初始化成功 !!! %d.\n",res);
                break;
        }
        printf("-> wait x轴初始化 !!!\n");
        delay(2000);
    }
        
    res = FMC4030_Jog_Single_Axis(0, 1, -200, 80, 160, 160, 2); //59 42
    if (res != 0)
    {
        perror("X Axis Run Fail !!!\r\n");
        return -3;
    }
    while (1){
        if(1 == FMC4030_Check_Axis_Is_Stop(0, 1)){
                printf("-> Y轴初始化成功 !!! %d.\n",res);
                break;
        }
        printf("-> wait y轴初始化 !!!\n");
        delay(2000);
    }
    // int circle = 0;
    // //Run X axis
    // printf("-> X&Y轴开始工作 !!!\n");
    // res = FMC4030_Jog_Single_Axis(0, 0, Xlength, Xspeed, Xaccel, Xdecel, 2);
    // if (res != 0)
    // {
    //     perror("X Axis Run Fail !!!\r\n");
    //     pthread_exit(NULL);
    // }
    
    // //Run Y axis circle
    // while (circle < 64)
    // {
    //     res = FMC4030_Jog_Single_Axis(0, 1, Ylength, Yspeed, Yaccel, Ydecel, 2);
    //     if (res != 0)
    //     {
    //         perror("Y+ Axis Run Fail !!!\r\n");
    //         pthread_exit(NULL);
    //     }
    //     while (!FMC4030_Check_Axis_Is_Stop(0, 1))
    //         ;
    //     res = FMC4030_Jog_Single_Axis(0, 1, -Ylength, Yspeed, Yaccel, Ydecel, 2);
    //     if (res != 0)
    //     {
    //         perror("Y- Axis Run Fail !!!\r\n");
    //         pthread_exit(NULL);
    //     }
    //     while (!FMC4030_Check_Axis_Is_Stop(0, 1))
    //         ;
    //     circle = circle + 1;
    // }

    // //Stop X&Y axis
    // res = FMC4030_Stop_Single_Axis(0, 0, 1);
    // if (res != 0)
    // {
    //     perror("X Axis Stop Fail !!!\r\n");
    //     pthread_exit(NULL);
    // }
    // while (!FMC4030_Check_Axis_Is_Stop(0, 0))
    //     ;
    // res = FMC4030_Stop_Single_Axis(0, 1, 1);
    // if (res != 0)
    // {
    //     perror("Y Axis Stop Fail !!!\r\n");
    //     pthread_exit(NULL);
    // }
    // while (!FMC4030_Check_Axis_Is_Stop(0, 1))
    //     ;
    // printf("-> X&Y轴停止工作工作，开始回零 !!!\n");
    // //Back to zero Point
    // res = FMC4030_Home_Single_Axis(0, 0, 80, 160, 160, 2);
    // if (res != 0)
    // {
    //     perror("X back to zero point fail !!!\r\n");
    //     pthread_exit(NULL);
    // }
    // while (!FMC4030_Check_Axis_Is_Stop(0, 0))
    //     ;
    // res = FMC4030_Home_Single_Axis(0, 1, 80, 160, 160, 2);
    // if (res != 0)
    // {
    //     perror("Y back to zero point fail !!!\r\n");
    //     pthread_exit(NULL);
    // }
    // while (!FMC4030_Check_Axis_Is_Stop(0, 1))
    //     ;
    delay(5000);
    // close controler connect
    FMC4030_Close_Device(0);
    printf("-> 导轨控制器关闭成功 !!!\n");

    return 0;
}   