#ifndef __COMMAND__
#define __COMMAND__

#define SERIAL_RATE 9600
#define DATA_BUFFER_MAX 6
//消息集
enum MSG
{
    //==============调试命令区====================
    UNKNOWN_INPUT,       //无效输入
    OUT_OF_BUFFER,       //缓冲区溢出
    CLEAR_SCREEN,        //清理屏幕
    NEXT_INT_OK,         //读取整数OK
    NEXT_DOUBLE_OK,      //读取浮点数OK
    MOTER_INFO,          //显示当前电机信息
    MOTERS_INFO,         //显示所有电机角度信息
    MOTER_PULSE,         //电机脉冲
    MOTER_TURN_RUNDS,    //电机转圈
    SET_MOTER_THRESHOLD, //设置电机速度阈值
    ENABLE_MOTER,        //启用电机
    ENABLE_ALL_MOTER,    //启用所有电机
    DISABLE_MOTER,       //关闭电机
    DISABLE_ALL_MOTER,   //关闭所有电机
    MOTER_GO_TO_ANGLE,   //让电机转到指定角度
    MOTERS_GO_TO_ANGLE,  //全部电机移动到指定参数列表
    //================上层命令区===================
    WRITE_PAGE,   //写路径点缓存
    RUN_PAGE,     //执行指定页路径缓存
    RUN_ALL_PAGE, //执行所有缓存
};

namespace std
{
    //命令处理类
    class CommandHandler
    {
    protected:
        //消息行缓冲区
        String msgBuffer;
        //缓冲区指针
        uint8_t dataBufferCursor;
        //数据计数器
        uint8_t dataCount;

    public:
        //数据集缓存区
        double dataBuffer[DATA_BUFFER_MAX];
        /**
         *  构造函数
         *  @param  Serial_Rate 串口波特率
         */
        CommandHandler(unsigned long Serial_Rate);
        /**
         *  指令获取
         *  @return  MSG消息集
         */
        MSG nextCommand();
        //获取下一个浮点数
        MSG nextInt(int *Container);
        //获取下一个浮点数
        MSG nextDouble(double *Container);
        //获取缓冲区
        const double *getBuffer();
    };
}

#endif