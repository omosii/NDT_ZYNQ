GCC = g++ 
AR = ar

# aarch64-linux-gnu-g++ -Wall -std=gnu++11 -pthread -o ZYNQ_NDT  main.cpp  -L. -lFMC4030-Lib -lpthread -pthread
# 定义交叉编译工具
CXX := aarch64-linux-gnu-g++

# 定义程序名称
TARGET := ZYNQ_NDT

# 定义源文件
SOURCES := main.cpp AXIGPIO.cpp function.cpp

# 定义编译选项
CXXFLAGS := -Wall -std=gnu++11 -pthread   # 添加你的头文件路径
# CXXFLAGS += -I/path/to/include
LDFLAGS :=  -lpthread   # 添加你的库文件路径
# LDFLAGS += -L/path/to/lib -lname

# 生成目标文件
$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# 伪目标，确保总是执行
.PHONY: clean

# 清理编译生成的文件
clean:
	rm -f $(TARGET)


# 旧：aarch64-linux-gnu-g++ -Wall -std=gnu++11 -pthread -o ZYNQ_NDT  main.cpp  -L. -lFMC4030-Lib -lpthread -pthread