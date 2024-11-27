//include header
#include "FMC4030.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include "stdlib.h"
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <thread>

#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <cctype>

// Rail const number ===========================================================================================
#define CircleNum 64        // 循环次数

#define Ylength 400         // Y 长度
#define Yspeed 40           // Y 速度
#define Yaccel 500          // Y 加速度
#define Ydecel 500          // Y 减速读


#define Xspeed 0.2          // X 速度
#define Xaccel 500          // X 加速度
#define Xdecel 500          // X 减速读
#define Xlength 2*CircleNum*(Xspeed*(Ylength/Yspeed))   // X 长度

// 导轨工作状态结构体 
struct posParam {

    float Xpos; // X axis position
	float Ypos; // Y axis position
    bool  flag_trig; // flag for determining whether to obtain the location and save them
    bool  flag_work; // flag for determining whether to obtain the location and save them
};

//AXI_GPIO ===========================================================================================
#define DEV_MEM "/dev/mem"

extern u_int8_t *axi_gpio_addr_0;
extern u_int8_t *axi_gpio_addr_1;
#define AXI_GPIO_PADDR_0 0xA0000000
#define AXI_GPIO_REG_SIZE 0x00001000

#define AXI_GPIO_PADDR_1 0xA0010000
#define AXI_GPIO_REG_SIZE_1 0x0000FFFF

#define GPIO_CHNNEL_1 1
#define GPIO_CHNNEL_2 2
#define CHAN_OFFSET 0x8

#define DATA_OFFSET 0x0      // 通道1数据寄存器
#define DATA_TRI_OFFSET 0x4  // 通道1三态控制寄存器
#define DATA2_OFFSET 0x8     // 通道2数据寄存器
#define DATA2_TRI_OFFSET 0xC // 通道2三态控制寄存器

#define GPIO_ALL_OUT 0x00000000
#define GPIO_ALL_IN 0xffffffff

//雷达与导轨工作状态,雷达高电平工作,雷达低电平复位;导轨高电平休止,导轨低电平工作 ============================================
#define work 0x01       // <2'b01>     导轨工作，雷达开机并接收到FPGA的外部触发
#define stop 0x02       // <2'b10>     导轨停止，雷达关机,急停
#define rstop 0x00      // <2'b00>     导轨工作，雷达复位：一般用不到
#define pause 0x03      // <2'b11>     导轨停止，雷达开机但FPGA不提供外部触发

//AXIGPIO Function
u_int32_t ReadAddr_32(u_int8_t *addr, u_int32_t offset);
u_int32_t AxiGpio_ReadReg(u_int8_t *gpio_addr, u_int8_t chnnel);
void WriteAddr_32(u_int8_t *addr, u_int32_t offset, u_int32_t value);               // 将一个32位的值写入指定的内存地址加偏移量处
void WriteAddr_2(u_int8_t *addr, u_int32_t offset, u_int32_t value);                     // 将一个1位的值写入指定的内存地址加偏移量处
void AxiGpio_SetDirection_32(u_int8_t *gpio_addr, u_int8_t chnnel, u_int32_t mask); // 设置AXI GPIO指定通道的引脚方向（输入或输出）
void AxiGpio_SetDirection_2(u_int8_t *gpio_addr, u_int8_t chnnel, u_int32_t mask);  // 设置AXI GPIO指定通道的引脚方向（输入或输出）

// 初始化导轨 ============================================
int railInitialization(void);
// 初始化套接字
int Socket_Init();
// 关闭套接字
int CloseSocket();
// 初始化与去内存映射
int AddrMap();
void AddrUnmap();
// 时延
void delay(unsigned int milliseconds);
// 等待上位机指令（阻塞型）
int recvfromWIN_wait(bool* CloseEvent_flag);
// 发送图像至上位机
int sendtoWIN_bin(const std::string& filePath, int BytesPerPackage);
// 采集结束后，向上位机发送 “CollectDone” 指令，告知上位机 雷达导轨已经运行结束
int CollectDone();
// 等待上位机“SendBIN”指令，以阻塞发送图像
int recvfromWIN_waitImage(bool* CloseEvent_flag);
// 测试代码，应被忽略
int testSendto();

//A thread for receiving data frim python ============================================
void* thread_PythonRXData(void *arg);
//A thread for RailControl
void* thread_FMC4030control( void* arg );
//A thread for position getting
void* thread_posGetandSave(void* arg);
//A thread for readPRF
void* thread_TriggerRead( void* arg );
//A thread for linsen CLOSE msg
void *thread_lisnCLOSE(void *arg);

//互斥锁
//pthread_mutex_t mutex ;

// kill a PythonScript
int killPythonScript(void);

