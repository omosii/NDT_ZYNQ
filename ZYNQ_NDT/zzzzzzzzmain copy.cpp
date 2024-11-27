#include "main.h"

using namespace std;

pthread_t tid_PythonRXData;
pthread_t tid_RailCtrl;
pthread_t tid_posGetandSave;
pthread_t tid_ReadGPIO;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    
    std::cout << "-> 系统启动 ^_^ " << std::endl;
    // 初始化导轨状态结构体
    posParam railpos = {(float)0,(float)0,(bool)0,(bool)0};
    // 初始化互斥锁
    pthread_mutex_init(&mutex,NULL); 
    // 初始化FPGA IP address
    // int result = std::system(" ifconfig eth0 192.168.0.35 netmask 255.255.255.0 \n");if (result != 0) {printf("-> IP地址配置失败 !!!\n");}
    // 初始化导轨
    // result = railInitialization(); if (result != 0) {printf("-> 导轨初始化失败，返回值: %d.\n",result); return 0;}
    // printf("-> 导轨初始化成功.\n");
    
    // int flag_thread_FMC4030control = 0; // 为1时导轨开始循环
    // pthread_create(&tid_RailCtrl,NULL,thread_FMC4030control,(void*)&flag_thread_FMC4030control);  
    // 要搞清楚雷达的TrrigerOut在外部触发时的表现=========================================================================================
    // pthread_create(&tid_ReadGPIO,NULL,thread_TriggerRead,(void*)&railpos); // 读到雷达触发信号，保存在 railpos中，
    // printf("-> thread_posGetandSave begin !!!\n");
    // pthread_create(&tid_posGetandSave,NULL,thread_posGetandSave,(void*)&railpos); // 根据railpos中flag值读取导轨位置并保存在.bin文件中 
    // printf("-> 4 !!!\n");
    // AXIGPIO内存映射
    AddrMap();
    AxiGpio_SetDirection_32(axi_gpio_addr_0, GPIO_CHNNEL_1, GPIO_ALL_OUT);
    AxiGpio_SetDirection_2(axi_gpio_addr_0, GPIO_CHNNEL_2, GPIO_ALL_OUT);

    //启动python脚本开始接收雷达数据========================================================================================================
     int t_arg = 0;
     if(pthread_create(&tid_PythonRXData, NULL, thread_PythonRXData, &t_arg)) perror("->Fail to create thread_PythonRXData.\n");
     pthread_detach(tid_PythonRXData);
     delay(2500);
     std::cout << "-> python接收数据开启. " << std::endl;
    // 外部触发改内部
     int result = 0;
     result = std::system(" stty -F /dev/ttyS1 1000000 \n");if (result != 0) {printf("-> 雷达内部触发配置失败1.\n");}
     result = std::system(" echo -n -e 'TrigMode=SELF;\r\n' > /dev/ttyS1 ");if (result != 0) {printf("-> 雷达内部触发配置失败2.\n");}
    // 雷达导轨开始启动
    WriteAddr_2(axi_gpio_addr_0, DATA2_OFFSET, u_int32_t(work));
    WriteAddr_32(axi_gpio_addr_0, 0, (u_int32_t)1666667);
    
    // flag_thread_FMC4030control = 1;
    std::cout << "-> 雷达导轨启动成功. " << std::endl;

    // // 读取控制直到要求结束app========================================================================================================
    std::string sys_cmd;
    std::cout << "-> 命令提示：输入 stop        会将雷达持续复位. " << std::endl;
    std::cout << "-> 命令提示：输入 work        会将雷达开机重配置，之后让导轨工作. " << std::endl;
    std::cout << "-> 命令提示：输入 pause       会将雷达开机重配置. " << std::endl << std::endl;
    std::cout << "-> 请输入命令（输入'exit'结束程序）,以换行结束: "<< std::endl;
    bool cmd_state = 0;
    while(std::getline(std::cin, sys_cmd) )
    {
        std::cout << "-> === " << "输入为："  << sys_cmd << " ==="  << std::endl;
        // 执行设置功能
        if ("work" == sys_cmd)          {WriteAddr_2(axi_gpio_addr_0, DATA2_OFFSET, pause);    cmd_state=1;
                                         std::system("./NDT_BASH.sh \n"); 
                                         WriteAddr_2(axi_gpio_addr_0, DATA2_OFFSET, work);                 } 
        else if ("stop" == sys_cmd)     {WriteAddr_2(axi_gpio_addr_0, DATA2_OFFSET, stop);     cmd_state=1;}
        else if ("pause" == sys_cmd)    {WriteAddr_2(axi_gpio_addr_0, DATA2_OFFSET, pause);    cmd_state=1;
                                         std::system("./NDT_BASH.sh \n");                                  }                              
        //  打印输入的字符串
        if (cmd_state){
            std::cout << "-> *** " << sys_cmd << "  功能设置成功." << std::endl;
            cmd_state = 0;
        }
        // 如果输入是"退出"，则退出循环
        if (sys_cmd == "exit")
        {
            std::cout << "-> 程序结束." << std::endl;
            break;
        }
        // 再次提示用户输入
        std::cout << "-> 请输入字符串（输入'exit'结束程序）: "<< std::endl;
    }

    // 终止python脚本========================================================================================================
    int PID_PythonScript = killPythonScript();
    printf("-> PythonScript终止, PID = %ld\n", (long)PID_PythonScript);
    // 退出线程
    // while(!flag_thread_FMC4030control); // 等待 thread_FMC4030control 执行完成
    railpos.flag_work = 1;
    // pthread_join(tid_RailCtrl,NULL);  
    // pthread_join(tid_posGetandSave,NULL);
    // pthread_cancel(tid_ReadGPIO);
    // 去互斥锁
    pthread_mutex_destroy(&mutex); 
    // AXIGPIO内存去映射
    AddrUnmap();
    
    return 0;
}