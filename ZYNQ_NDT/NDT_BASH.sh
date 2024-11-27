#!/bin/bash

# message = "-> Hello, now In radar configuration"
# echo $message
echo "-> Hello, now In radar configuration."

# 进入工作目录
cd /mnt/NDT_YDS/FPGA_ControlRadar

#
echo "-> Waiting for configuration to complete."

python3 controlRadar.py /dev/ttyS1 1000000 RadarConfig.txt

echo "-> config complete."

# message = "-> Now Serial port data receiving"
# echo $message
