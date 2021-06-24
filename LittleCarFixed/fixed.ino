//包含软串口通信
#include <SoftwareSerial.h>
#define a0 46   //左侧循迹
#define a1 47   //前1循迹
#define a2 48   //前2循迹
#define a3 49   //前3循迹
#define a4 50   //前4循迹
#define a5 51   //右下
#define a6 52   //左下
#define ray1 42 //小车前光电
#define ray2 43 //侧光电
#define ahead 1
#define back 0
#define left 1
#define right 0

#define PWM_UP_LEFT_BLUE_WIRE 9       //左前无刷电机pwm,越小速度越快
#define BRAKE_UP_LEFT_GREEN_WIRE 29   //左前无刷电机刹车,高电平解除
#define TOWARDS_UP_LEFT_WHITE_WIRE 28 //左前无刷电机转向,高电平逆时针
#define PULSE_UP_LEFT_YELLOW_WIRE A0  //左前无刷电机的编码器脉冲

#define PWM_DOWN_LEFT_BLUE_WIRE 12      //左后无刷电机pwm,越小速度越快
#define BRAKE_DOWN_LEFT_GREEN_WIRE 25   //左后无刷电机刹车,高电平解除
#define TOWARDS_DOWN_LEFT_WHITE_WIRE 24 //左后无刷电机转向,高电平逆时针
#define PULSE_DOWN_LEFT_YELLOW_WIRE A1  //左后无刷电机的编码器脉冲

#define PWM_UP_RIGHT_BLUE_WIRE 7       //右前无刷电机pwm,越小速度越快
#define BRAKE_UP_RIGHT_GREEN_WIRE 31   //右前无刷电机刹车,高电平解除
#define TOWARDS_UP_RIGHT_WHITE_WIRE 30 //右前无刷电机转向,高电平逆时针
#define PULSE_UP_RIGHT_YELLOW_WIRE A2  //右前无刷电机的编码器脉冲

#define PWM_DOWN_RIGHT_BLUE_WIRE 11      //右后无刷电机pwm,越小速度越快
#define BRAKE_DOWN_RIGHT_GREEN_WIRE 27   //右后无刷电机刹车,高电平解除
#define TOWARDS_DOWN_RIGHT_WHITE_WIRE 26 //右后无刷电机转向,高电平逆时针
#define PULSE_DOWN_RIGHT_YELLOW_WIRE A3  //右后无刷电机的编码器脉冲

//电机配速结构体
typedef struct
{
    //速度及方向
    int Up_Left_Speed;
    bool Up_Left_rol;
    int Up_Right_Speed;
    bool Up_Right_rol;
    int Down_Left_Speed;
    bool Down_Left_rol;
    int Down_Right_Speed;
    bool Down_Right_rol;
} SpeedSetting;

//函数声明

//设置小车整体前进方向
void Move(bool ToWhere, uint16_t speed);
//前进时的强路径修正(强到能在拐角直接扭头起飞还贼拉准)
void Fix_Turn(bool ToWhere, uint16_t speed, uint16_t Speed_fix);
//刹车
void Barke();
//获取传感器值
uint16_t GetSensor();
//直线前进直到遇到某个条件,由函数指针int_Func决策
void GoTill(bool ToWhere, bool (*int_Func)(void), uint16_t speed, uint16_t Speed_fix);
//持续转弯直到遇到某个条件,由函数指针int_Func决策
void TurnTill(SpeedSetting Setting, bool (*int_Func)(void));
//持续路径修正
void FixTill(bool (*int_Func)(void));
//T型路口转向
void Turn(SpeedSetting Setting);
//状态判断函数区:
bool isEnd()
{
    switch (GetSensor() & 0b011110000)
    {
    case 0b00000:
        return true;
    default:
        return false;
    }
}
//侧面循迹检测(处于T字路口)
//需要行动补偿(在检测这个条件时大多需要小车先前进一点)
bool isT()
{
    switch (GetSensor() & 0b100000000)
    {
    case 0b100000000:
        return true;
    case 0b000000000:
        return false;
    }
}
//十字路口判定
bool isCross()
{
    if (!isEnd() && !back_End() && isT())
        return true;
    else
        return false;
}
//转向终止条件
bool attached()
{
    switch (GetSensor() & 0b001100000)
    {
    case 0b001100000:
        return true;
    default:
        return false;
    }
}

//尾部为空
bool back_End()
{
    switch (GetSensor() & 0b000001100)
    {
    case 0b0000:
        return true;
    default:
        return false;
    }
}

//前循迹障碍检测
bool get_close()
{
    switch (GetSensor() & 0b000000010)
    {
    case 0b10:
        return false;
    case 0b00:
        return true;
    }
}

//全局配速文件

//五边形角落
SpeedSetting angleMode;
//原地左转
SpeedSetting Cross_Left_Mode;
//原地右转
SpeedSetting Cross_Right_Mode;
SoftwareSerial Connect(39, 38);

//挂起进程直到读取到信息
void Wait_For_signal()
{
    while (Connect.read() != 'G')
    {
        Connect.flush();
    }
}
//发送信息
void Send_signal()
{
    Connect.write('G');
    Serial.println("AAAAAA");
    Connect.flush();
    Serial.println("BBBBB");
}

void setup()
{
    Connect.begin(9600);
    //开始监听软串口通信
    Connect.listen();
    //确认监听状态
    // put your setup code here, to run once:
    pinMode(a0, INPUT);
    pinMode(a1, INPUT); //定义循迹
    pinMode(a2, INPUT);
    pinMode(a3, INPUT);
    pinMode(a4, INPUT);
    pinMode(a5, INPUT);
    pinMode(a6, INPUT);
    pinMode(ray1, INPUT);
    pinMode(ray2, INPUT);

    pinMode(PWM_UP_RIGHT_BLUE_WIRE, OUTPUT);      //右无刷电机PWM
    pinMode(BRAKE_UP_RIGHT_GREEN_WIRE, OUTPUT);   //刹车线
    pinMode(TOWARDS_UP_RIGHT_WHITE_WIRE, OUTPUT); //方向

    pinMode(PWM_DOWN_RIGHT_BLUE_WIRE, OUTPUT);      //右无刷电机PWM
    pinMode(BRAKE_DOWN_RIGHT_GREEN_WIRE, OUTPUT);   //刹车线
    pinMode(TOWARDS_DOWN_RIGHT_WHITE_WIRE, OUTPUT); //方向

    pinMode(PWM_UP_LEFT_BLUE_WIRE, OUTPUT);      //左无刷电机PWM
    pinMode(BRAKE_UP_LEFT_GREEN_WIRE, OUTPUT);   //刹车线
    pinMode(TOWARDS_UP_LEFT_WHITE_WIRE, OUTPUT); //方向

    pinMode(PWM_DOWN_LEFT_BLUE_WIRE, OUTPUT);      //左无刷电机PWM
    pinMode(BRAKE_DOWN_LEFT_GREEN_WIRE, OUTPUT);   //刹车线
    pinMode(TOWARDS_DOWN_LEFT_WHITE_WIRE, OUTPUT); //方向

    //十字路口或直角右转向配速
    Cross_Right_Mode.Up_Left_Speed = 150;
    Cross_Right_Mode.Up_Right_Speed = 150;
    Cross_Right_Mode.Down_Left_Speed = 150;
    Cross_Right_Mode.Down_Right_Speed = 150;
    Cross_Right_Mode.Up_Left_rol = Cross_Right_Mode.Down_Left_rol = HIGH;
    Cross_Right_Mode.Up_Right_rol = Cross_Right_Mode.Down_Right_rol = HIGH;

    //十字路口或直角左转向配速
    Cross_Left_Mode.Up_Left_Speed = 150;
    Cross_Left_Mode.Up_Right_Speed = 150;
    Cross_Left_Mode.Down_Left_Speed = 150;
    Cross_Left_Mode.Down_Right_Speed = 150;
    Cross_Left_Mode.Up_Left_rol = Cross_Left_Mode.Down_Left_rol = LOW;
    Cross_Left_Mode.Up_Right_rol = Cross_Left_Mode.Down_Right_rol = LOW;

    //五边形角落配速
    angleMode.Up_Left_Speed = 150;
    angleMode.Up_Right_Speed = 150;
    angleMode.Down_Left_Speed = 230;
    angleMode.Down_Right_Speed = 150;
    angleMode.Up_Left_rol = angleMode.Down_Left_rol = LOW;
    angleMode.Up_Right_rol = angleMode.Down_Right_rol = LOW;

    //串口通信
    Serial.begin(9600);
}

//主体流程
void loop()
{
    GoTill(ahead, isEnd, 220, 100);
    delay(500);
    GoTill(back, isT, 220, 100);
    delay(500);
    TurnTill(Cross_Left_Mode, attached);
    delay(500);
    Move(ahead, 200);
    delay(200);
    GoTill(ahead, isCross, 200, 100);
    //到达十字路口，发送信号并等待上层机械臂就位

    delay(500);
    for (int count = 0; count < 5; count++)
    {
        Send_signal();
        Serial.println("Sended!!!!!!!");
        Wait_For_signal();
        GoTill(ahead, get_close, 230, 100);
        Send_signal();
        Wait_For_signal();
        GoTill(back, isT, 200, 100);
        TurnTill(Cross_Right_Mode, attached);
        delay(500);
        Move(ahead, 220);
        delay(150);
        GoTill(ahead, isCross, 220, 100);
        delay(500);
        TurnTill(Cross_Left_Mode, attached);
    }
    GoTill(back, back_End, 220, 100);
    delay(400000000);
}

//函数定义
void Move(bool ToWhere, uint16_t speed)
{
    if (ToWhere)
    {
        //右侧电机Low为前进
        digitalWrite(TOWARDS_UP_RIGHT_WHITE_WIRE, LOW);   //右无刷电机的方向
        analogWrite(PWM_UP_RIGHT_BLUE_WIRE, speed);       //右无刷电机的速度
        digitalWrite(TOWARDS_DOWN_RIGHT_WHITE_WIRE, LOW); //右无刷电机的方向
        analogWrite(PWM_DOWN_RIGHT_BLUE_WIRE, speed);     //右无刷电机的速度
        //左侧电机High为前进
        digitalWrite(TOWARDS_UP_LEFT_WHITE_WIRE, HIGH);   //左无刷电机的方向
        analogWrite(PWM_UP_LEFT_BLUE_WIRE, speed);        //左无刷电机的速度
        digitalWrite(TOWARDS_DOWN_LEFT_WHITE_WIRE, HIGH); //左无刷电机的方向
        analogWrite(PWM_DOWN_LEFT_BLUE_WIRE, speed);      //左无刷电机的速度
        digitalWrite(BRAKE_UP_RIGHT_GREEN_WIRE, HIGH);    //右无刷电机的刹车为关
        digitalWrite(BRAKE_UP_LEFT_GREEN_WIRE, HIGH);     //左无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_RIGHT_GREEN_WIRE, HIGH);  //右无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_LEFT_GREEN_WIRE, HIGH);   //左无刷电机的刹车为关
    }
    else
    {
        digitalWrite(TOWARDS_UP_RIGHT_WHITE_WIRE, HIGH);   //右无刷电机的方向
        analogWrite(PWM_UP_RIGHT_BLUE_WIRE, speed);        //右无刷电机的速度
        digitalWrite(TOWARDS_DOWN_RIGHT_WHITE_WIRE, HIGH); //右无刷电机的方向
        analogWrite(PWM_DOWN_RIGHT_BLUE_WIRE, speed);      //右无刷电机的速度
        digitalWrite(TOWARDS_UP_LEFT_WHITE_WIRE, LOW);     //左无刷电机的方向
        analogWrite(PWM_UP_LEFT_BLUE_WIRE, speed);         //左无刷电机的速度
        digitalWrite(TOWARDS_DOWN_LEFT_WHITE_WIRE, LOW);   //左无刷电机的方向
        analogWrite(PWM_DOWN_LEFT_BLUE_WIRE, speed);       //左无刷电机的速度
        digitalWrite(BRAKE_UP_LEFT_GREEN_WIRE, HIGH);      //左无刷电机的刹车为关
        digitalWrite(BRAKE_UP_RIGHT_GREEN_WIRE, HIGH);     //右无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_RIGHT_GREEN_WIRE, HIGH);   //右无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_LEFT_GREEN_WIRE, HIGH);    //左无刷电机的刹车为关
    }
}

void Fix_Turn(bool ToWhere, uint16_t speed, uint16_t Speed_fix)
{
    if (ToWhere)
    {
        analogWrite(PWM_DOWN_RIGHT_BLUE_WIRE, speed - Speed_fix); //右无刷电机的速度
        analogWrite(PWM_UP_RIGHT_BLUE_WIRE, speed - Speed_fix);   //右无刷电机的速度
        analogWrite(PWM_UP_LEFT_BLUE_WIRE, speed);                //左无刷电机的速度
        analogWrite(PWM_DOWN_LEFT_BLUE_WIRE, speed);              //左无刷电机的速度
        digitalWrite(BRAKE_UP_RIGHT_GREEN_WIRE, HIGH);            //右无刷电机的刹车为关
        digitalWrite(BRAKE_UP_LEFT_GREEN_WIRE, HIGH);             //左无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_RIGHT_GREEN_WIRE, HIGH);          //右无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_LEFT_GREEN_WIRE, HIGH);           //左无刷电机的刹车为关
    }
    else
    {
        analogWrite(PWM_UP_RIGHT_BLUE_WIRE, speed);              //右无刷电机的速度
        analogWrite(PWM_DOWN_RIGHT_BLUE_WIRE, speed);            //右无刷电机的速度
        analogWrite(PWM_UP_LEFT_BLUE_WIRE, speed - Speed_fix);   //左无刷电机的速度
        analogWrite(PWM_DOWN_LEFT_BLUE_WIRE, speed - Speed_fix); //左无刷电机的速度
        digitalWrite(BRAKE_UP_RIGHT_GREEN_WIRE, HIGH);           //右无刷电机的刹车为关
        digitalWrite(BRAKE_UP_LEFT_GREEN_WIRE, HIGH);            //左无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_RIGHT_GREEN_WIRE, HIGH);         //右无刷电机的刹车为关
        digitalWrite(BRAKE_DOWN_LEFT_GREEN_WIRE, HIGH);          //左无刷电机的刹车为关
    }
}

void Barke()
{
    analogWrite(PWM_UP_RIGHT_BLUE_WIRE, 0);       //右无刷电机的速度
    analogWrite(PWM_DOWN_RIGHT_BLUE_WIRE, 0);     //右无刷电机的速度
    analogWrite(PWM_UP_LEFT_BLUE_WIRE, 0);        //左无刷电机的速度
    analogWrite(PWM_DOWN_LEFT_BLUE_WIRE, 0);      //左无刷电机的速度
    digitalWrite(BRAKE_UP_RIGHT_GREEN_WIRE, LOW); //右无刷电机的刹车
    digitalWrite(BRAKE_UP_LEFT_GREEN_WIRE, LOW);  //左无刷电机的刹车
    digitalWrite(BRAKE_DOWN_RIGHT_GREEN_WIRE, LOW);
    digitalWrite(BRAKE_DOWN_LEFT_GREEN_WIRE, LOW);
}

uint16_t GetSensor()
{
    uint16_t SensorStatus;
    SensorStatus = digitalRead(a0) << 8 |
                   digitalRead(a1) << 7 |
                   digitalRead(a2) << 6 |
                   digitalRead(a3) << 5 |
                   digitalRead(a4) << 4 |
                   digitalRead(a5) << 3 |
                   digitalRead(a6) << 2 |
                   digitalRead(ray1) << 1 |
                   digitalRead(ray2);
    return SensorStatus;
}

void GoTill(bool ToWhere, bool (*int_Func)(void), uint16_t speed, uint16_t Speed_fix)
{
    //初动补偿,避免一开始移动就触发终止条件
    Move(ToWhere, speed);
    delay(40);
    //当中断函数返回为true时持续前进
    while (!int_Func())
    {
        //前进与后退分别使用前后两组循迹来判别校正
        if (ToWhere)
        {
            //与门过滤,屏蔽侧面与尾部循迹
            switch ((GetSensor() & 0b0011110000))
            {
                //右倾状态
            case 0b010000000:
                //Fix_Turn(left, speed, Speed_fix + 20); //极端倾斜大力修正
                FixTill(attached);
                break;
            case 0b011000000:
            case 0b001000000:
                Fix_Turn(left, speed, Speed_fix);
                break;
                //左倾状态
            case 0b000010000:
                //Fix_Turn(right, speed, Speed_fix + 20); //极端倾斜大力修正
                FixTill(attached);
                break;
            case 0b000110000:
            case 0b000100000:
                Fix_Turn(right, speed, Speed_fix);
                break;
            default:
                Move(ToWhere, speed);
                break;
            }
        }
        else
        {
            switch ((GetSensor() & 0b1100))
            {
            //左倾状态
            case 0b0100:
                Fix_Turn(left, speed, Speed_fix);
                break;
            //右倾状态
            case 0b1000:
                Fix_Turn(right, speed, Speed_fix);
                break;
            default:
                Move(ToWhere, speed);
                break;
            }
        }
    }
    //回动补偿,精确停止
    Move(!ToWhere, 200);
    delay(40);
    Barke();
}

void Turn(SpeedSetting Setting)
{

    digitalWrite(TOWARDS_UP_LEFT_WHITE_WIRE, Setting.Up_Left_rol);     //左前电机方向
    digitalWrite(TOWARDS_DOWN_LEFT_WHITE_WIRE, Setting.Down_Left_rol); //左后电机方向

    analogWrite(PWM_UP_LEFT_BLUE_WIRE, Setting.Up_Left_Speed);     //左前电机的速度
    analogWrite(PWM_DOWN_LEFT_BLUE_WIRE, Setting.Down_Left_Speed); //左后电机的速度
    //右前方电机Low为前进
    digitalWrite(TOWARDS_UP_RIGHT_WHITE_WIRE, Setting.Up_Right_rol);     //右无刷电机的方向
    digitalWrite(TOWARDS_DOWN_RIGHT_WHITE_WIRE, Setting.Down_Right_rol); //右无刷电机的方向

    analogWrite(PWM_UP_RIGHT_BLUE_WIRE, Setting.Up_Right_Speed); //右前无刷电机的速度
    //内扭矩轮慢速旋转
    analogWrite(PWM_DOWN_RIGHT_BLUE_WIRE, Setting.Down_Right_Speed); //右后无刷电机的速度

    digitalWrite(BRAKE_UP_RIGHT_GREEN_WIRE, HIGH);   //右前刹车为关
    digitalWrite(BRAKE_UP_LEFT_GREEN_WIRE, HIGH);    //左前刹车为关
    digitalWrite(BRAKE_DOWN_RIGHT_GREEN_WIRE, HIGH); //右后刹车为关
    digitalWrite(BRAKE_DOWN_LEFT_GREEN_WIRE, HIGH);  //左后刹车为关
}

void TurnTill(SpeedSetting Setting, bool (*int_Func)(void))
{
    //=================正在施工==================
    //起始转向补偿
    Turn(Setting);
    delay(150);
    while (!int_Func())
        Turn(Setting);
    //终止转向补偿，依据重量和地面可能需要微调
    //好了,现在有FixTill了,微调可有可无
    Barke();
}

//路径修正
void FixTill(bool (*int_Func)(void))
{
    bool pre_path_founded = false;
    //先前路径方向
    bool pre_path;
    //当没有检测到路径时,先找到路径在哪个方向
    while (!pre_path_founded)
    {
        switch (GetSensor() & 0b011110000)
        {
        case 0b011000000:
        case 0b010000000:
            pre_path_founded = true;
            pre_path = left;
            break;
        case 0b000110000:
        case 0b000010000:
            pre_path_founded = true;
            pre_path = right;
            break;
        default:
            //这里是处理发生偏移，但正好中路两循迹在路上的情况
            //先让小车前进一点点直到读取到路径
            Move(ahead, 200);
            delay(10);
            Barke();
        }
    }
    while (!int_Func())
    {
        if (pre_path)
            Turn(Cross_Left_Mode);
        else
            Turn(Cross_Right_Mode);
    }
}