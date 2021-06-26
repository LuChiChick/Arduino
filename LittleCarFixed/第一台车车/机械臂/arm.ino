#机械臂重构代码
#include <Servo.h>

//舵机信息
typedef struct
{
    Servo ThisServo;
    uint8_t angle;
} Servos;

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
    Serial.begin(9600);
}

void loop()
{
    //舵机复位
    resetServo();
    //等待信息
    for(int count=0;count<5;count++){
        Catch_Item(count);
        
    }

}