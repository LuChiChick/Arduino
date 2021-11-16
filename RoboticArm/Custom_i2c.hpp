#ifndef __CUSTOM_IIC__
#define __CUSTOM_IIC__
extern "C"
{
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
}
#define CUS_IIC_BUFFER_MAX 20
#define CUS_IIC_WAIT_MAX_TIMES 300
#define CUS_IIC_DEF_SPEED_THRESHOLD 10
#define CUS_IIC_REQUEST_FAILED 0
#define CUS_IIC_REQUEST_DONE 1
#define CUS_IIC_WRITE_DONE 1
#define CUS_IIC_WRITE_FAILED 0
#define CUS_IIC_NODATA_CONTAINED 0
#define CUS_IIC_DEFAULT_TIMEOUT 250
namespace std
{
    //IIC通信类
    class cus_IIC_Wire
    {
    protected:
        //获取每us的tick数
        uint32_t tick_per_us;
        //配速阈值,用来调节时钟频率(us)进而调节iic通信速率
        uint32_t speed_threshold;
        //判断初始化是否结束
        bool Init_already;
        //引脚信息
        uint16_t SCL_Pin;
        uint16_t SDA_Pin;
        //目标地址
        uint8_t Aim_addr;
        //判断起始信息是否已经发出
        bool start_sent;
        //缓存区
        uint8_t buffer[CUS_IIC_BUFFER_MAX];
        //缓存区内数据长度
        uint8_t data_length;
        //缓存区已排出计数
        uint8_t pop_Count;
        void start();
        void stop();
        void sendACK();
        void sendNACK();
        bool checkACK();
        //收发功能
        //写入一个字节
        void writeByte(uint8_t data);
        //读取一个字节数据
        uint8_t receiveByte();
        //时延
        void delay_us(uint32_t times);

    public:
        //默认构造函数
        cus_IIC_Wire();
        //带参构造函数
        cus_IIC_Wire(uint16_t SCL_Pin, uint16_t SDA_Pin);
        //状态检查
        bool if_Init_OK();
        //指定地址
        virtual void focuson(uint8_t Aim_addr);
        //指定速度阈值
        void speedLimit(uint32_t speed_threshold);
        //连续写入
        bool write(uint8_t data);
        //请求count组数据
        bool request(uint8_t count);
        //从buffer中取出数据
        uint8_t read();
        //结束通信
        void end(bool send_NACK_or_Not);
    };
}

#endif