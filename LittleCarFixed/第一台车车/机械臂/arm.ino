//机械臂重构代码
#include <Servo.h>
//机械臂各个舵机引脚定义
#define Servo1 10
#define Servo2 A2
#define Servo3 A3
#define Servo4 A0
#define Servo5 A1
#define Servo6 7

//舵机信息体
typedef struct
{
    Servo ThisServo;
    uint8_t angle;
} Servos;

//机械臂状态记录
Servos ServoChain[6];

//机械臂配置文件,用于Move()
typedef struct
{
    uint8_t Servo1_angle;
    uint8_t Servo2_angle;
    uint8_t Servo3_angle;
    uint8_t Servo4_angle;
    uint8_t Servo5_angle;
    uint8_t Servo6_angle;
} Servo_Setting;

//机械臂复位
void resetServo()
{
    //机械臂复位使用配置
    Servo_Setting resetSetting;
    //这里需要对设置文件进行配置,如下
    //resetSetting.Servo1_angle=100;

    Move(resetSetting);
}

//根据舵机配置文件移动各个舵机
void Move(Servo_Setting Setting)
{
}

//抓取装配台零件并存放
void Catch_Item(uint8_t WhichOne)
{
    switch (WhichOne)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        break;
    }
    //抓完之后记得放进去然后归位
}

//抓取存储内零件
void Catch_Item_From_Storage(uint8_t WhichOne)
{
    switch (WhichOne)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    default:
        break;
    }
    //抓取完之后应该将机械臂置于刚好对准的状态
}

//投放零件
void PutItem(uint8_t WhichOne)
{
    switch (WhichOne)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        break;
    }
}

void setup()
{
    ServoChain[0].ThisServo.attach(Servo1);
    ServoChain[1].ThisServo.attach(Servo2);
    ServoChain[2].ThisServo.attach(Servo3);
    ServoChain[3].ThisServo.attach(Servo4);
    ServoChain[4].ThisServo.attach(Servo5);
    ServoChain[5].ThisServo.attach(Servo6);
    Serial.begin(9600);
}

void loop()
{
    //舵机复位
    resetServo();
    //等待信息
    //Wait_For_Signal()
    //循环存放
    for(int count=0;count<5;count++){
        Catch_Item(count);
        //抓取完毕后发送信息
        //Send_Signal()
    }

    //此处抓取部分完成，小车进入装配区域

    //舵机复位
    resetServo();
    //等待信息
    //Wait_For_Signal()
    //循环拿取存储并装配
    for(int count=0;count<5;count++){
        //等待小车就位后抓取零件
        //Wait_For_Signal()
        Catch_Item(count);
        //抓取完毕后发送信息并等待下层就绪
        //Send_Signal()
        //Wait_For_Signal()
        //循环存放
        PutItem(count);
        //发送信息表示投放完成
        //Send_Signal()
    }

}