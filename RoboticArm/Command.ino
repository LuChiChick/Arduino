#include "Command.hpp"
namespace std
{
    //构造函数
    CommandHandler::CommandHandler(unsigned long Serial_Rate)
    {
        //初始化波特率
        Serial.begin(Serial_Rate);
        dataBufferCursor = 0;
        dataCount = 0;
    }
    //获取指令
    MSG CommandHandler::nextCommand()
    {
        //指针归零
        dataBufferCursor = 0;
        dataCount = 0;
        //死循环缓冲区
        while (Serial.read() != '>')
            ;
        //无消息死循环先
        while (!Serial.available())
            ;
        //读取下一行
        msgBuffer = Serial.readStringUntil('<');
        //转大写
        msgBuffer.toUpperCase();
        //复制出命令行
        char CMD[10];
        sscanf(msgBuffer.c_str(), "%s", CMD);
        //处理数据部分
        for (int count = 0; msgBuffer.c_str()[count] != '\0'; count++)
        {
            if (msgBuffer.c_str()[count] == ' ')
            {
                dataBuffer[dataCount] = atof(msgBuffer.c_str() + count + 1);
                dataCount++;
            }
        }

        //命令筛选
        if (!strcmp(CMD, "CLS") || !strcmp(CMD, "CLEARSCREEN"))
            return CLEAR_SCREEN;
        else if (!strcmp(CMD, "MI") || !strcmp(CMD, "MOTERINFO"))
            return MOTER_INFO;
        else if (!strcmp(CMD, "MSI") || !strcmp(CMD, "MOTERSINFO"))
            return MOTERS_INFO;
        else if (!strcmp(CMD, "MP") || !strcmp(CMD, "MOTERPULSE"))
            return MOTER_PULSE;
        else if (!strcmp(CMD, "MT") || !strcmp(CMD, "MOTERTURN"))
            return MOTER_TURN_RUNDS;
        else if (!strcmp(CMD, "SMT") || !strcmp(CMD, "SETMOTERTHRESHOLD"))
            return SET_MOTER_THRESHOLD;
        else if (!strcmp(CMD, "EM") || !strcmp(CMD, "ENABLEMOTER"))
            return ENABLE_MOTER;
        else if (!strcmp(CMD, "EA") || !strcmp(CMD, "ENABLEALLMOTER"))
            return ENABLE_ALL_MOTER;
        else if (!strcmp(CMD, "DM") || !strcmp(CMD, "DISABLEMOTER"))
            return DISABLE_MOTER;
        else if (!strcmp(CMD, "DA") || !strcmp(CMD, "DISABLEALLMOTER"))
            return DISABLE_ALL_MOTER;
        else if (!strcmp(CMD, "WP") || !strcmp(CMD, "WRITEPAGE"))
            return WRITE_PAGE;
        else if (!strcmp(CMD, "RP") || !strcmp(CMD, "RUNPAGE"))
            return RUN_PAGE;
        else if (!strcmp(CMD, "MG") || !strcmp(CMD, "MOTERGO"))
            return MOTER_GO_TO_ANGLE;
        else if (!strcmp(CMD, "MSG") || !strcmp(CMD, "MOTERSGO"))
            return MOTERS_GO_TO_ANGLE;
        else if (!strcmp(CMD, "START") || !strcmp(CMD, "RAP") || !strcmp(CMD, "RUNALLPAGE"))
            return RUN_ALL_PAGE;
        else if (!strcmp(CMD, "RP") || !strcmp(CMD, "RUNPAGE"))
            return RUN_PAGE;
        else
            return UNKNOWN_INPUT;
    }
    //获取下一个整数
    MSG CommandHandler::nextInt(int *Container)
    {
        //检查数据区溢出
        if (dataBufferCursor >= dataCount)
            return OUT_OF_BUFFER;
        //返回命令值
        *Container = (int)dataBuffer[dataBufferCursor];
        dataBufferCursor++;
        return NEXT_INT_OK;
    }
    //获取下一个浮点数
    MSG CommandHandler::nextDouble(double *Container)
    {
        //检查数据区溢出
        if (dataBufferCursor >= dataCount)
            return OUT_OF_BUFFER;
        //返回命令值
        *Container = dataBuffer[dataBufferCursor];
        dataBufferCursor++;
        return NEXT_DOUBLE_OK;
    }
    //获取缓冲区
    const double *CommandHandler::getBuffer()
    {
        return dataBuffer;
    }
}