#include "Custom_i2c.hpp"
#define DEF_ARDUINO_SYSTICK 16000000
//源自GPIO_setOutputHighOnPin
#define SDA_H() digitalWrite(SDA_Pin, HIGH)
//源自GPIO_setOutputLowOnPin
#define SDA_L() digitalWrite(SDA_Pin, LOW)
//源自GPIO_setOutputHighOnPin
#define SCL_H() digitalWrite(SCL_Pin, HIGH)
//源自GPIO_setOutputHighOnPin
#define SCL_L() digitalWrite(SCL_Pin, LOW)
//源自GPIO_setAsOutputPin
#define SDA_OUT_MODE() pinMode(SDA_Pin, OUTPUT)
//源自GPIO_setAsInputPinWithPullUpResistor
#define SDA_IN_MODE() pinMode(SDA_Pin, INPUT_PULLUP)
//源自GPIO_getInputPinValue
#define SDA_CHECK() (digitalRead(SDA_Pin) ? 1 : 0)

void std::cus_IIC_Wire::sendACK()
{
    SCL_L();
    SDA_L();
    delay_us(speed_threshold);
    SCL_H();
    delay_us(speed_threshold);
    SCL_L();
}

void std::cus_IIC_Wire::sendNACK()
{
    SCL_L();
    SDA_H();
    delay_us(speed_threshold);
    SCL_H();
    delay_us(speed_threshold);
    SCL_L();
}

bool std::cus_IIC_Wire::checkACK()
{
    SCL_L();
    SDA_IN_MODE();
    delay_us(speed_threshold);
    SCL_H();
    delay_us(speed_threshold);
    bool condition = false;
    for (int count = 0; count < CUS_IIC_WAIT_MAX_TIMES; count++)
    {
        if (SDA_CHECK() == 0)
        {
            condition = true;
            break;
        }
    }
    SCL_L();
    SDA_OUT_MODE();
    return condition;
}

void std::cus_IIC_Wire::delay_us(uint32_t times)
{
    if (times == 0)
        return;
    //这里(tick_per_us * times)/3是因为每次循环执行了count--和比较两部分,需要6tick
    for (uint32_t count = (tick_per_us * times) / 6; count > 0; count--)
        ;
    //硬核delay
}

//无参构造
std::cus_IIC_Wire::cus_IIC_Wire()
{
    Init_already = false;
}
//有参构造
std::cus_IIC_Wire::cus_IIC_Wire(uint16_t SCL_Pin, uint16_t SDA_Pin)
{
    this->SCL_Pin = SCL_Pin;
    this->SDA_Pin = SDA_Pin;

    //SCL初始化
    pinMode(SCL_Pin, OUTPUT);
    //SDA初始化
    pinMode(SDA_Pin, OUTPUT);
    //拉高准备
    SDA_H();
    SCL_H();
    //相关参数初始化
    tick_per_us = DEF_ARDUINO_SYSTICK / 1000000;
    speed_threshold = CUS_IIC_DEF_SPEED_THRESHOLD;
    start_sent = false;
    data_length = 0;
    pop_Count = 0;
    Init_already = false;
}

//状态检查
bool std::cus_IIC_Wire::if_Init_OK()
{
    return Init_already;
}

void std::cus_IIC_Wire::focuson(uint8_t Aim_addr)
{
    this->Aim_addr = Aim_addr;
    Init_already = true;
}

void std::cus_IIC_Wire::speedLimit(uint32_t speed_threshold)
{
    this->speed_threshold = speed_threshold;
}

void std::cus_IIC_Wire::start()
{
    SDA_H();
    SCL_H();
    delay_us(speed_threshold);
    SDA_L();
    delay_us(speed_threshold);
    SCL_L();
    start_sent = true;
}

void std::cus_IIC_Wire::stop()
{
    SCL_L();
    SDA_L();
    delay_us(speed_threshold);
    SCL_H();
    delay_us(speed_threshold);
    SDA_H();
    delay_us(speed_threshold);
    SCL_L();
    delay_us(speed_threshold);
    SCL_H();
    //结束后置为高
    start_sent = false;
}

void std::cus_IIC_Wire::writeByte(uint8_t data)
{
    //循环写入
    for (int count = 7; count >= 0; count--)
    {
        SCL_L();
        delay_us(speed_threshold);
        if ((data >> count) & 1)
            SDA_H();
        else
            SDA_L();
        SCL_H();
        delay_us(speed_threshold);
    }
    SCL_L();
    delay_us(speed_threshold);
}

uint8_t std::cus_IIC_Wire::receiveByte()
{
    SCL_L();
    //更改为读取模式
    SDA_IN_MODE();
    uint8_t data = 0;
    //循环读取
    for (int count = 0; count < 8; count++)
    {
        data <<= 1;
        SCL_H();
        delay_us(speed_threshold);
        if (SDA_CHECK() == 1)
        {
            data++;
        }
        SCL_L();
        delay_us(speed_threshold);
    }
    //恢复输出模式
    SDA_OUT_MODE();
    return data;
}

bool std::cus_IIC_Wire::write(uint8_t data)
{
    //检查是否初始化
    if (Init_already == false)
    {
        return CUS_IIC_WRITE_FAILED;
    }
    //检查是否第一次发信
    if (start_sent == false)
    {
        start();
        //写入7位地址+写入位
        writeByte(Aim_addr << 1);
        //检查应答
        if (checkACK() == false)
        {
            stop();
            return CUS_IIC_WRITE_FAILED;
        }
    }
    //写入数据
    writeByte(data);
    //检查应答
    if (checkACK() == false)
    {
        stop();
        return CUS_IIC_WRITE_FAILED;
    }
    return CUS_IIC_WRITE_DONE;
}

bool std::cus_IIC_Wire::request(uint8_t count)
{
    //检查是否初始化
    if (Init_already == false)
    {
        return CUS_IIC_REQUEST_FAILED;
    }
    //检查缓冲区是否溢出
    if (data_length + count > CUS_IIC_BUFFER_MAX)
        return CUS_IIC_REQUEST_FAILED;
    //起始信号
    start();
    //写入7位地址+读取位
    writeByte((Aim_addr << 1) + 1);
    //检查应答
    if (checkACK() == false)
    {
        stop();
        return CUS_IIC_REQUEST_FAILED;
    }
    //循环读取
    for (int c = 0; c < count; c++)
    {
        //存入数据
        buffer[data_length] = receiveByte();
        //更新长度
        data_length++;
        //应答或者结束
        if (c == count - 1)
            end(true);
        else
            sendACK();
    }
    return CUS_IIC_REQUEST_DONE;
}

uint8_t std::cus_IIC_Wire::read()
{
    //检查缓冲区状态
    if (pop_Count == data_length)
        return 0;
    //读取数据并更新计数
    uint8_t temp;
    temp = buffer[pop_Count];
    pop_Count++;
    //判断是否需要清空缓冲区
    if (pop_Count == data_length)
        pop_Count = data_length = 0;
    return temp;
}

void std::cus_IIC_Wire::end(bool send_NACK_or_Not)
{
    if (send_NACK_or_Not == true)
        sendNACK();
    stop();
}