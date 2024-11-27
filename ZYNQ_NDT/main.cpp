#include "main.h"

using namespace std;

pthread_t tid_PythonRXData;
pthread_t tid_RailCtrl;
pthread_t tid_posGetandSave;
pthread_t tid_ReadGPIO;
pthread_t tid_CloseEvent;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    std::cout << std::endl << "-> ==================================================================" 
            << std::endl << std::endl;
    std::cout << std::endl << "-> 系统启动 ^_^ " << std::endl << std::endl;
    // 初始化导轨状态结构体
    // posParam railpos = {(float)0,(float)0,(bool)0,(bool)0};
    // 初始化互斥锁
    pthread_mutex_init(&mutex,NULL); 
    // 初始化FPGA IP address
    int result = std::system(" ifconfig eth0 192.168.137.100 netmask 255.255.255.0 \n");
        if (result != 0) {printf("-> IP地址配置失败 !!!\n\n"); return -1;}
    // ifconfig eth0 192.168.137.100 netmask 255.255.255.0
    // 初始化套接字
    result = Socket_Init(); 
        if (result != 0) {printf("-> 套接字初始化失败 !!!\n\n");  return -1;}
    
    printf("-> 套接字初始化完成.\n\n");
  
    // AXIGPIO内存映射
    AddrMap();
    AxiGpio_SetDirection_32(axi_gpio_addr_0, GPIO_CHNNEL_1, GPIO_ALL_OUT);
    AxiGpio_SetDirection_2(axi_gpio_addr_0, GPIO_CHNNEL_2, GPIO_ALL_OUT);

    // 创建退出监测线程
    bool CloseEvent_flag = 0;
    pthread_create(&tid_CloseEvent,NULL,thread_lisnCLOSE,(void*)&CloseEvent_flag);  
    pthread_detach(tid_CloseEvent);
    // 主循环
    while(1){
        // 等待上位机指令后开机    // 设置超时循环，可以优雅的退出函数
	    printf("     等待上位机启动系动指示 ... \n\n");
        result = recvfromWIN_wait(&CloseEvent_flag); if ( result != 0 || CloseEvent_flag == 1) { break;}

        //启动python脚本开始接收雷达数据========================================================================================================
        //  int t_arg = 0;
        //  if(pthread_create(&tid_PythonRXData, NULL, thread_PythonRXData, &t_arg)) perror("->Fail to create thread_PythonRXData.\n");
        //  pthread_detach(tid_PythonRXData);
        //  delay(2500);
        //  std::cout << "-> python接收数据开启. " << std::endl;
        // 外部触发改内部
        //  result = std::system(" stty -F /dev/ttyS1 1000000 \n");if (result != 0) {printf("-> 雷达内部触发配置失败1.\n");}
        //  result = std::system(" echo -n -e 'TrigMode=SELF;\r\n' > /dev/ttyS1 ");if (result != 0) {printf("-> 雷达内部触发配置失败2.\n");}
        // 雷达导轨开始启动
        WriteAddr_2(axi_gpio_addr_0, DATA2_OFFSET, u_int32_t(work));
        WriteAddr_32(axi_gpio_addr_0, 0, (u_int32_t)1666667);
        
        // flag_thread_FMC4030control = 1;
        std::cout << "-> 雷达导轨启动成功. " << std::endl << std::endl;

        

        // 检测到雷达导轨运行结束后，运行成像程序，并将结果存为uint8格式的BIN文件=========================================================
        std::cout << "-> 采集中... " << std::endl << std::endl;delay(1000);
        std::cout << "-> 成像中... " << std::endl << std::endl;delay(1000);
        // 向上位机发送“CollectDone”，等待下一步上位机指令
        CollectDone();

        // 终止python脚本========================================================================================================
        // int PID_PythonScript = killPythonScript();
        // printf("-> PythonScript终止, PID = %ld\n", (long)PID_PythonScript);
        // 退出线程
        // while(!flag_thread_FMC4030control); // 等待 thread_FMC4030control 执行完成
        // railpos.flag_work = 1;
        // pthread_join(tid_RailCtrl,NULL);  
        // pthread_join(tid_posGetandSave,NULL);
        // pthread_cancel(tid_ReadGPIO);

        // 收到回传图像指令，阻塞解除，执行sendtoWIN_bin
	    printf("     图像生成结束，等待上位机出图指示 ... \n\n");
        result = recvfromWIN_waitImage(&CloseEvent_flag);if ( result != 0 || CloseEvent_flag == 1) { break;}
        // 回传图像数据
        std::string filePath = "HDD_200r_336c_uint8_RowMajor.bin";
        int BytesSizeperPakg = 2400;
        result = sendtoWIN_bin(filePath, BytesSizeperPakg); 
            if (result != 0) {printf("-> 图像发送失败 !!!\n"); return -1;}
        std::cout << "              File sent successfully." << std::endl<< std::endl;
        
    }
    

    std::cout << std::endl << "-> 系统关闭，欢迎下次光临～～ ^_^ （比心！）" << std::endl;
    std::cout << std::endl << "-> ==================================================================" 
            << std::endl << std::endl;
    // AXIGPIO内存去映射
    AddrUnmap();
    // 关闭套接字
    CloseSocket();
    // 去互斥锁
    pthread_mutex_destroy(&mutex); 
    
    return 0;
}