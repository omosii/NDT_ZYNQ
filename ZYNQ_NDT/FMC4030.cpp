#include "main.h"

// struct posParam {

//     float Xpos; // X axis position
// 	float Ypos; // Y axis position
//     bool  flag_trig; // flag_trig for determining whether to obtain the location and save them
// };

extern pthread_mutex_t mutex;

int railInitialization(void)
{
    int res = 0;
    // res = FMC4030_Open_Device(0, "192.168.0.30", 8088);
    char ip[] = "192.168.0.30";
    res = FMC4030_Open_Device(0, ip, 8088);
    printf("-> 导轨控制器开启成功 !!!\n");
    if (res != 0)
    {
        perror("FMC OPEN DEVICE Fail !!!\r\n");
        FMC4030_Close_Device(0);
        return -1;
    }
    //Run X&Y Axis to Begin Point
    res = FMC4030_Jog_Single_Axis(0, 0, -128, 80, 160, 160, 2); //59 42
    if (res != 0)
    {
        perror("X Axis Run Fail !!!\r\n");
        return -2;
    }
    while (!FMC4030_Check_Axis_Is_Stop(0, 0))
        ;
        printf("-> X轴初始化成功 !!!\n");
    res = FMC4030_Jog_Single_Axis(0, 1, -200, 80, 160, 160, 2); //59 42
    if (res != 0)
    {
        perror("X Axis Run Fail !!!\r\n");
        return -3;
    }
    while (!FMC4030_Check_Axis_Is_Stop(0, 1))
        ;
        printf("-> Y轴初始化成功 !!!\n");
    return 0;
}

void *thread_FMC4030control(void *arg)
{
    int res = 0;
    int circle = 0;

    int *flag = (int *)arg;
    while (*flag)
        ;

    //Run X axis
    printf("-> X&Y轴开始工作 !!!\n");
    res = FMC4030_Jog_Single_Axis(0, 0, Xlength, Xspeed, Xaccel, Xdecel, 2);
    if (res != 0)
    {
        perror("X Axis Run Fail !!!\r\n");
        pthread_exit(NULL);
    }
    
    //Run Y axis circle
    while (circle < 64)
    {
        res = FMC4030_Jog_Single_Axis(0, 1, Ylength, Yspeed, Yaccel, Ydecel, 2);
        if (res != 0)
        {
            perror("Y+ Axis Run Fail !!!\r\n");
            pthread_exit(NULL);
        }
        while (!FMC4030_Check_Axis_Is_Stop(0, 1))
            ;
        res = FMC4030_Jog_Single_Axis(0, 1, -Ylength, Yspeed, Yaccel, Ydecel, 2);
        if (res != 0)
        {
            perror("Y- Axis Run Fail !!!\r\n");
            pthread_exit(NULL);
        }
        while (!FMC4030_Check_Axis_Is_Stop(0, 1))
            ;
        circle = circle + 1;
    }

    //Stop X&Y axis
    res = FMC4030_Stop_Single_Axis(0, 0, 1);
    if (res != 0)
    {
        perror("X Axis Stop Fail !!!\r\n");
        pthread_exit(NULL);
    }
    while (!FMC4030_Check_Axis_Is_Stop(0, 0))
        ;
    res = FMC4030_Stop_Single_Axis(0, 1, 1);
    if (res != 0)
    {
        perror("Y Axis Stop Fail !!!\r\n");
        pthread_exit(NULL);
    }
    while (!FMC4030_Check_Axis_Is_Stop(0, 1))
        ;
    printf("-> X&Y轴停止工作工作，开始回零 !!!\n");
    //Back to zero Point
    res = FMC4030_Home_Single_Axis(0, 0, 80, 160, 160, 2);
    if (res != 0)
    {
        perror("X back to zero point fail !!!\r\n");
        pthread_exit(NULL);
    }
    while (!FMC4030_Check_Axis_Is_Stop(0, 0))
        ;
    res = FMC4030_Home_Single_Axis(0, 1, 80, 160, 160, 2);
    if (res != 0)
    {
        perror("Y back to zero point fail !!!\r\n");
        pthread_exit(NULL);
    }
    while (!FMC4030_Check_Axis_Is_Stop(0, 1))
        ;

    // close controler connect
    FMC4030_Close_Device(0);
    printf("-> 导轨控制器关闭成功 !!!\n");
    *flag = 0;
    return 0;
}

// Get X&Y axis position and save them
// err = pthread_create(&ntid, NULL, thr_fn, &param1);
void *thread_posGetandSave(void *arg)
{

    // 打开文件以二进制写入模式
    printf("-> 0 !!!\n");
    std::ofstream file("PosofXandY.bin", std::ios::binary | std::ios::out);
    printf("-> 1 !!!\n");
    // 检查文件是否成功打开
    if (!file)
    {
        std::cerr << "无法打开文件以进行写入！" << std::endl;
        pthread_exit(NULL);
    }
    // obtain posParam from main function
    posParam *pos = (posParam *)arg;
    printf("-> 2 !!!\n");
    // Get X axis position and save them
    int res = 0;
    while (1)
    {
        if (pos->flag_work)
        {
            // 关闭文件
            file.close();
            printf("-> 3.1 !!!\n");
            pthread_exit(NULL);
        }
        // Determining whether to obtain the location
        if (pos->flag_trig)
        {
            // Waiting for next trigger
            pthread_mutex_lock(&mutex);
            pos->flag_trig = 0;
            printf("-> 3.2 !!!\n");
            // Save X&Y Position
            res = FMC4030_Get_Axis_Current_Pos(0, 0, &(pos->Xpos));
            printf("-> 3.3 !!!\n");
            if (res != 0)
            {
                perror(" Get X Axis position Fail !!! SubThreadExit. \r\n");
                pthread_exit(NULL);
            }
            res = FMC4030_Get_Axis_Current_Pos(0, 1, &(pos->Ypos));
            if (res != 0)
            {
                perror(" Get Y Axis position Fail !!! SubThreadExit. \r\n");
                pthread_exit(NULL);
            }
            // 写入浮点数到文件
            printf("-> 3.4 !!!\n");
            file.write(reinterpret_cast<char *>(&pos->Xpos), sizeof(pos->Xpos));
            file.write(reinterpret_cast<char *>(&pos->Ypos), sizeof(pos->Ypos));
            printf("-> 3.5 !!!\n");
            pthread_mutex_unlock(&mutex);
        }
    }
}

// 读取 AXIGPIO 数值
void *thread_TriggerRead(void *arg)
{

    // obtain posParam from main function
    posParam *pos = (posParam *)arg;
    // 读取 FPGA的trig_out 映射到 AXIGPIO 的数值
    while (1)
        if (1 == AxiGpio_ReadReg(axi_gpio_addr_1, GPIO_CHNNEL_1))
        {
            pthread_mutex_lock(&mutex);
            pos->flag_trig = 1;
            pthread_mutex_unlock(&mutex);
            while (!AxiGpio_ReadReg(axi_gpio_addr_1, GPIO_CHNNEL_1));
               
        }

    pthread_exit(NULL);
}