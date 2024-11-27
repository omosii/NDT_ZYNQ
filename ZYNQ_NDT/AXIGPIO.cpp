#include "main.h"

using namespace std;

u_int8_t *axi_gpio_addr_0;
u_int8_t *axi_gpio_addr_1;

u_int32_t ReadAddr_32(u_int8_t *addr, u_int32_t offset)
{
    return *(u_int32_t *)(addr + offset);
}

u_int32_t AxiGpio_ReadReg(u_int8_t *gpio_addr, u_int8_t chnnel)
{
    u_int32_t DataOffset;

    DataOffset = ((chnnel - 1) * CHAN_OFFSET) + DATA_OFFSET;
    AxiGpio_SetDirection_32(gpio_addr, chnnel, GPIO_ALL_IN);

    return ReadAddr_32(gpio_addr, DataOffset);
}

void WriteAddr_32(u_int8_t *addr, u_int32_t offset, u_int32_t value)
{
    *(u_int32_t *)(addr + offset) = value;
}

void WriteAddr_2(u_int8_t *addr, u_int32_t offset, u_int32_t value)
{
    *(u_int32_t *)(addr + offset) = value;
}

void AxiGpio_SetDirection_32(u_int8_t *gpio_addr, u_int8_t chnnel, u_int32_t mask)
{
    u_int32_t DataOffset;

    DataOffset = ((chnnel - 1) * CHAN_OFFSET) + DATA_TRI_OFFSET;
    WriteAddr_32(gpio_addr, DataOffset, mask);
}

void AxiGpio_SetDirection_2(u_int8_t *gpio_addr, u_int8_t chnnel, u_int32_t mask)
{
    u_int32_t DataOffset;

    DataOffset = ((chnnel - 1) * CHAN_OFFSET) + DATA_TRI_OFFSET;
    WriteAddr_2(gpio_addr, DataOffset, mask);
}



