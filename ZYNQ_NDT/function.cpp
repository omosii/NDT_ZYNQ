#include "main.h"

//变量声明
int Socket_image_fd, Socket_ctrl_fd, Socket_close_fd;
struct sockaddr_in localAddr_image, remoteAddr_image; // 图像传输
struct sockaddr_in localAddr_ctrl, remoteAddr_ctrl;   // 启动控制
struct sockaddr_in localAddr_close, remoteAddr_close;   // 启动控制

// A delay function (Unit: ms)
void delay(unsigned int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// 去除\n与\r
void remove_newline(char* buffer) {
    char* newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';
    newline = strchr(buffer, '\r');
    if (newline) *newline = '\0';
}

//字符串转u_int32_t
u_int32_t ipv4_str2num(char *ip_str)
{
	//e.g. ip_str="192.168.1.1"
	char str[4][4];
	u_int32_t num[4];
	u_int8_t i;
	u_int8_t pos;
	u_int8_t digit;

	pos = 0;
	digit = 0;
	for(i=0; i<15; i++){
		if(ip_str[i] == '.'){
			//如果当前字符是点，则将之前收集的字符串转换为整数并存储在num数组中
			num[pos] = (u_int32_t)atoi(str[pos]);
			pos++;
			digit = 0;			
		}
		else if(ip_str[i] == '\0'){
			//检查当前字符是否是字符串的结束
			//如果是，处理后退出for循环，并准备返回输出
			num[pos] = (u_int32_t)atoi(str[pos]);
			break;
		}
		else{
			str[pos][digit] = ip_str[i];
			digit++;
		}
	}
	//将四个数字合并为一个32位无符号整数并返回。
	//每个数字左移了其相应的位数（由其在地址中的位置决定）
	return num[0]<<24 | num[1]<<16 | num[2]<<8 | num[3];
	//num[0]==192 | num[1]==168 | num[2]==1 | num[3]==1
	//return 32bit:  192 168 1 1 

}

// socket Initialization
// 套接字初始化
int Socket_Init()
{

	u_int32_t ip_num;
	u_int16_t port;
    char ipaddr[]= "192.168.137.1";
    //创建图像套接字=========================================================================================================
	Socket_image_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Socket_image_fd == -1){
		printf("->create socket Socket_image_fd error.\n");
		return -1;
	}

	port = (u_int16_t)atoi("9999");
	//设置localAddr
	bzero(&localAddr_image, sizeof(localAddr_image));
	localAddr_image.sin_addr.s_addr = INADDR_ANY;//主机可能有多个网卡，INADDR_ANY表示绑定所有网卡
	localAddr_image.sin_family = AF_INET; //使用IPV4
	localAddr_image.sin_port = htons(port);//端口号
	if (bind(Socket_image_fd, (struct sockaddr *)&localAddr_image, sizeof(localAddr_image)) < 0){
        printf("Socket binds error!!!\n");
		return -1;
	}

	//Remote Socket ip addr and port
	//GetProfileString(CONFIG_FILE, "sys_config", "remote_ip", temp);
	ip_num = ipv4_str2num(ipaddr);
	//GetProfileString(CONFIG_FILE, "sys_config", "remote_port", temp);
	port = (u_int16_t)atoi("10000");
	bzero(&remoteAddr_image, sizeof(remoteAddr_image));
	remoteAddr_image.sin_addr.s_addr = htonl(ip_num);
	remoteAddr_image.sin_family = AF_INET;
	remoteAddr_image.sin_port = htons(port);

	//创建系统控制套接字（上位机向雷达导轨发送指令）=========================================================================================================
	Socket_ctrl_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Socket_ctrl_fd == -1){
		printf("->create socket Socket_ctrl_fd error.\n");
		return -1;
	}

	port = (u_int16_t)atoi("8889");
	//设置localAddr
	bzero(&localAddr_ctrl, sizeof(localAddr_ctrl));
	localAddr_ctrl.sin_addr.s_addr = INADDR_ANY;//主机可能有多个网卡，INADDR_ANY表示绑定所有网卡
	localAddr_ctrl.sin_family = AF_INET; //使用IPV4
	localAddr_ctrl.sin_port = htons(port);//端口号
	if (bind(Socket_ctrl_fd, (struct sockaddr *)&localAddr_ctrl, sizeof(localAddr_ctrl)) < 0){
        printf("Socket binds error!!!\n");
		return -1;
	}

	//Remote Socket ip addr and port
	//GetProfileString(CONFIG_FILE, "sys_config", "remote_ip", temp);
	ip_num = ipv4_str2num(ipaddr);
	//GetProfileString(CONFIG_FILE, "sys_config", "remote_port", temp);
	port = (u_int16_t)atoi("8888");
	bzero(&remoteAddr_ctrl, sizeof(remoteAddr_ctrl));
	remoteAddr_ctrl.sin_addr.s_addr = htonl(ip_num);
	remoteAddr_ctrl.sin_family = AF_INET;
	remoteAddr_ctrl.sin_port = htons(port);

    //创建系统关闭套接字（上位机向ZYNQ发送关闭指令）=========================================================================================================
	Socket_close_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(Socket_close_fd == -1){
		printf("->create socket Socket_close_fd error.\n");
		return -1;
	}

	port = (u_int16_t)atoi("8001");
	//设置localAddr
	bzero(&localAddr_close, sizeof(localAddr_close));
	localAddr_close.sin_addr.s_addr = INADDR_ANY;//主机可能有多个网卡，INADDR_ANY表示绑定所有网卡
	localAddr_close.sin_family = AF_INET; //使用IPV4
	localAddr_close.sin_port = htons(port);//端口号
	if (bind(Socket_close_fd, (struct sockaddr *)&localAddr_close, sizeof(localAddr_close)) < 0){
        printf("Socket binds error!!!\n");
		return -1;
	}

	//Remote Socket ip addr and port
	//GetProfileString(CONFIG_FILE, "sys_config", "remote_ip", temp);
	ip_num = ipv4_str2num(ipaddr);
	//GetProfileString(CONFIG_FILE, "sys_config", "remote_port", temp);
	port = (u_int16_t)atoi("8000");
	bzero(&remoteAddr_close, sizeof(remoteAddr_close));
	remoteAddr_close.sin_addr.s_addr = htonl(ip_num);
	remoteAddr_close.sin_family = AF_INET;
	remoteAddr_close.sin_port = htons(port);

    return 0;
}

// 关闭套接字
int CloseSocket(){
    close(Socket_ctrl_fd);
    close(Socket_image_fd);
    close(Socket_close_fd);
    return 0;
}


//udp 接收上位机信息，若接收到“BEGIN”则跳出本函数, 启动导轨和雷达,没有则阻塞
int recvfromWIN_wait(bool* CloseEvent_flag){

    // 设置超时循环，可以由雅的退出函数
	//初始化读取长度
    int read_len = 0;
	//接收数据
	socklen_t addrLen = sizeof(localAddr_ctrl);
    char recv_buffer[1024] = "";
    bzero(recv_buffer, sizeof(recv_buffer));//清空缓冲区

    // 设置接收超时时间为1秒
    struct timeval timeout;
    timeout.tv_sec = 1; // 秒
    timeout.tv_usec = 0; // 微秒
    if (setsockopt(Socket_ctrl_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt失败");
        return -1;
    }

    // 循环接收数据，直到收到有效数据或上位机指令后退出
    while(1){
        read_len = recvfrom(Socket_ctrl_fd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&localAddr_ctrl, &addrLen);
        if(*CloseEvent_flag) break;
        if (read_len > 0) {
            recv_buffer[read_len] = '\0'; // 确保字符串以null结尾
            // 移除可能存在的换行符
            remove_newline(recv_buffer);

            if (strcmp(recv_buffer, "BEGIN") == 0) {
                printf("        收到指令： %s ------导轨、雷达启动!\n\n", recv_buffer);
                break;
            } else {
                printf("        未收到BEGIN，recvfrom返回值read_len为： %d .\n\n", read_len);
            }
        } else if (read_len == 0 || (read_len < 0 && errno == EAGAIN)) {
            continue;
        } else {
            printf("错误类型errno(ly): %s\n\n", strerror(errno));
            // 关闭套接字
            close(Socket_ctrl_fd);
            printf("套接字Socket_fd：(%d) 已被关闭.\n\n", Socket_ctrl_fd);
            return -1;
        } 
    }
    
	return 0;
}

// 采集结束后，向上位机发送 “CollectDone” 指令，告知上位机 雷达导轨已经运行结束
int CollectDone(){
    ssize_t bytesSent;
    const char* msg = "CollectDone" ;
    if ((bytesSent = sendto(Socket_ctrl_fd, msg, strlen(msg), 0, 
                        (const struct sockaddr*)&remoteAddr_ctrl, sizeof(remoteAddr_ctrl))) < 0) {
            perror("Sendto CollectDone failed");
            close(Socket_image_fd);
            exit(EXIT_FAILURE);
    }
    return 0;
}

// 等待上位机“SendBIN”指令，以阻塞发送图像
int recvfromWIN_waitImage(bool* CloseEvent_flag){
    //初始化读取长度
    int read_len = 0;
	//接收数据
	socklen_t addrLen = sizeof(localAddr_ctrl);
    char recv_buffer[1024] = "";
    bzero(recv_buffer, sizeof(recv_buffer));//清空缓冲区

    // 设置接收超时时间为1秒
    struct timeval timeout;
    timeout.tv_sec = 1; // 秒
    timeout.tv_usec = 0; // 微秒
    if (setsockopt(Socket_ctrl_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt失败");
        return -1;
    }
    while(1){
        // 接收数据 &localAddr_ctrl：指向一个 struct sockaddr 结构的指针，该结构在函数返回时将包含发送方的地址信息。如果你不关心发送方的地址，可以传递 NULL。
        read_len = recvfrom(Socket_ctrl_fd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&localAddr_ctrl, &addrLen);

        if(*CloseEvent_flag) break;

        if (read_len > 0) {
            recv_buffer[read_len] = '\0'; // 确保字符串以null结尾
            // 移除可能存在的换行符
            remove_newline(recv_buffer);

            if(strcmp(recv_buffer, "SendBIN") == 0){
                printf("        收到指令： %s ------回传图像!\n\n",recv_buffer);
                break; 
            }
            else {
                printf("        未收到SendBIN，recvfrom返回值read_len为： %d .\n\n", read_len);
            }
        } else if ((read_len < 0 && errno == EAGAIN)) {
            continue;
        } else {
            printf("错误类型errno(ly): %s\n\n", strerror(errno));
            // 关闭套接字
            close(Socket_ctrl_fd);
            printf("套接字Socket_fd：(%d) 已被关闭.\n\n", Socket_ctrl_fd);
            return -1;
        } 
    }
    return 0;
}

// ZYNQ 向上位机发送 uint8 bin格式 的文件
int sendtoWIN_bin(const std::string& filePath, int BytesPerPackage){
    
    ssize_t bytesSent;
    // 打开二进制文件
    std::ifstream binFile(filePath, std::ios::binary);
    if (!binFile) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        close(Socket_image_fd);
        exit(EXIT_FAILURE);
    }

    // 读取文件内容到vector中
    // 利用输入流迭代器（std::istreambuf_iterator 专门用于从输入流（如文件流）中读取字符）从文件流 binFile 中读取所有数据，并将这些数据存储到一个 std::vector<uint8_t> 类型的向量 buffer 中
    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(binFile)), std::istreambuf_iterator<char>());

    // 发送数据
    while (buffer.size() > 0) {

        // reinterpret_cast<const char*>(buffer.data()) 将 buffer 的数据指针转换为 const char* 类型，因为 sendto 函数期望数据是以 const char* 的形式提供的
        // MSG_CONFIRM 是一个标志，但在大多数系统上，它对于UDP套接字来说可能不是有效的选项（具体取决于操作系统和库的实现）
        if ((bytesSent = sendto(Socket_image_fd, reinterpret_cast<const char*>(buffer.data()), BytesPerPackage, 0, 
                                (const struct sockaddr*)&remoteAddr_image, sizeof(remoteAddr_image))) < 0) {
            perror("Sendto failed");
            close(Socket_image_fd);
            exit(EXIT_FAILURE);
        }
         
        // std::cout << "第"<< i<<"次发送字节数："<< bytesSent << std::endl;
        // delay(10);
        // 否则，移除已发送的数据，并继续发送剩余数据
        buffer.erase(buffer.begin(), buffer.begin() + bytesSent);
    }
    // // 关闭套接字
    // close(Socket_image_fd);

    return 0;
}

// test
int testSendto(){
    ssize_t bytesSent;
    const char* msg = "HelloUDP" ;
    if ((bytesSent = sendto(Socket_image_fd, msg, strlen(msg), 0, 
                        (const struct sockaddr*)&remoteAddr_image, sizeof(remoteAddr_image))) < 0) {
            perror("Sendto failed");
            close(Socket_image_fd);
            exit(EXIT_FAILURE);
    }
    std::cout << "发送字节数："<< bytesSent << std::endl;
    return 0;
}
//Implement mapping from DDR physical address to DDR virtual address
int AddrMap()
{
    int memfd;

    memfd = open(DEV_MEM, O_RDWR | O_SYNC);
    if (memfd < 0)
    {
        printf("-> mem open failed.\n");
        return -1;
    }

    axi_gpio_addr_0 = (u_int8_t *)mmap64(NULL, AXI_GPIO_REG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, AXI_GPIO_PADDR_0);
    axi_gpio_addr_1 = (u_int8_t *)mmap64(NULL, AXI_GPIO_REG_SIZE_1, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, AXI_GPIO_PADDR_1);

    close(memfd);
    return 0;
}

//Implement demapping of DDR physical address
void AddrUnmap()
{
    munmap(axi_gpio_addr_0, AXI_GPIO_REG_SIZE);
    munmap(axi_gpio_addr_1, AXI_GPIO_REG_SIZE_1);
}

// 函数用于提取字符串中的所有数字并存储到字符串向量中
std::vector<std::string> extractNumbers(const std::string& input) {
    std::vector<std::string> numbers;
    std::string currentNumber;

    for (char ch : input) {
        // 检查字符是否是数字
        if (std::isdigit(ch)) {
            // 将数字字符添加到当前数字字符串中
            currentNumber.push_back(ch);
        } else {
            // 如果当前数字字符串不为空，将其添加到结果数组中
            if (!currentNumber.empty()) {
                numbers.push_back(currentNumber);
                currentNumber.clear(); // 重置当前数字字符串
            }
        }
    }

    // 检查字符串末尾是否有未添加的数字
    if (!currentNumber.empty()) {
        numbers.push_back(currentNumber);
    }

    return numbers;
}

//A thread for receiving data frim python
void *thread_PythonRXData(void *arg)
{

    int result = std::system("python3 /media/sd-mmcblk1p1/NDT_project/TxTSerial.py \
                                256 /dev/ttyS1 1000000 /media/sd-mmcblk1p1/NDT_project/OriginData \n");  
    if (result != 0) {
       printf("-> 接收雷达数据串口配置失败.\n");
       pthread_exit(NULL);
    }

    // 初始化子进程的 PID
    static int tpid = -959;  

    pthread_exit((void*)(&tpid));
}

int killPythonScript(void){

// 构造命令
    std::string command = "ps aux | grep 'TxTSerial.py'";   
    // 使用 popen 执行命令
    FILE *fp = popen(command.c_str(), "r");
    if (fp == NULL) {
        std::cerr << "Failed to run command: " << command << std::endl;
        pthread_exit(NULL);
    }
    // 读取命令输出
    std::array<char, 256> buffer;
    std::string output;
    while (fgets(buffer.data(), buffer.size(), fp) != NULL)  output += buffer.data();
    // 关闭文件流
    int result = pclose(fp);
    if (result == -1) {
        std::cerr << "Failed to close pipe." << std::endl;
        pthread_exit(NULL);
    }
    // 输出结果
    std::cout << "Command output:\n" << output << std::endl;

    std::vector<std::string> numbers = extractNumbers(output);
    int pid = -105;
    // 检查容器中是否有足够的元素
    if (numbers.size() > 1) {
        // 访问PID字符串
        pid = std::stoi(numbers[0]);
        std::cout << "Python PID is: " << numbers[0] << std::endl;
    } 
    else std::cout << "There is no string you want in the vector." << std::endl;

    kill(pid, SIGTERM);
    return pid;
}

//A thread for linsen CLOSE msg
void *thread_lisnCLOSE(void *arg)
{
    int read_len = 0;
    bool* flag_close = static_cast<bool*>(arg);
    //接收数据
	socklen_t addrLen = sizeof(localAddr_close);
    char recv_buffer[100] = "";
    bzero(recv_buffer, sizeof(recv_buffer));//清空缓冲区

    // 监听 
    read_len = recvfrom(Socket_close_fd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr*)&localAddr_close, &addrLen);
    if (read_len > 0) recv_buffer[read_len] = '\0';  // 确保字符串以null结尾
    // 移除可能存在的换行符
    char *newline = strchr(recv_buffer, '\n'); if (newline)  *newline = '\0';
    newline = strchr(recv_buffer, '\r');  if (newline)   *newline = '\0';

    if(strcmp(recv_buffer, "CLOSE") == 0){
        printf("        收到指令： %s ------ZYNQ控制程序关闭!\r\n",recv_buffer);
        *flag_close = true;
    }
    else if(read_len > 0){
        printf("recvfrom返回值read_len为： %d .\r\n", read_len);
        bzero(recv_buffer, sizeof(recv_buffer));//清空缓冲区
    } 
    else {
        printf("错误类型errno:%s\r\n",strerror(errno));
        //关闭套接字
        close(Socket_close_fd);
        printf("套接字Socket_fd：(%d) 已被关闭.\r\n",Socket_close_fd);
        pthread_exit(NULL);
    } 
    pthread_exit(NULL);
}