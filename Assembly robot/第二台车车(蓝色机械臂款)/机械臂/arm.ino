//机械臂重构代码
#include <Servo.h>
//机械臂各个舵机引脚定义
#define Servo1 10
#define Servo2 A2
#define Servo3 A3
#define Servo4 A0
#define Servo5 A1
#define Servo6 7
#define READY_TO_CATCH_FROM_STORAGE 1
#define READY_TO_CATCH_ITEM 2

//舵机信息体
struct Servos
{
    Servo ThisServo;
    uint8_t angle;
};

//舵机串，用于机械臂状态记录,不要手动更改！！！不要手动更改！！！！！！不要手动更改！！！！！
Servos ServoChain[6];

//机械臂角度配置结构体,用于Move()
struct angle_Setting
{
    uint8_t Servo0_angle;
    uint8_t Servo1_angle;
    uint8_t Servo2_angle;
    uint8_t Servo3_angle;
    uint8_t Servo4_angle;
    uint8_t Servo5_angle;
};

//机械臂速度配置结构体,用于Move()的可选参数，决定每个舵机移动的时延
struct speed_Setting
{
    uint8_t Servo0_delay;
    uint8_t Servo1_delay;
    uint8_t Servo2_delay;
    uint8_t Servo3_delay;
    uint8_t Servo4_delay;
    uint8_t Servo5_delay;
};

//从舵机串返回舵机角度信息并组成配置结构体
angle_Setting Get_Angle_Setting_FromChain(Servos Chain[6])
{
    angle_Setting temp_angle_Setting;
    temp_angle_Setting.Servo0_angle = Chain[0].angle;
    temp_angle_Setting.Servo1_angle = Chain[1].angle;
    temp_angle_Setting.Servo2_angle = Chain[2].angle;
    temp_angle_Setting.Servo3_angle = Chain[3].angle;
    temp_angle_Setting.Servo4_angle = Chain[4].angle;
    temp_angle_Setting.Servo5_angle = Chain[5].angle;
    return temp_angle_Setting;
}

//判断舵机位置是否达到设置指向的位置
bool All_attached(angle_Setting angle_s, Servos Chain[6])
{
    if (angle_s.Servo0_angle != Chain[0].angle)
        return false;
    if (angle_s.Servo1_angle != Chain[1].angle)
        return false;
    if (angle_s.Servo2_angle != Chain[2].angle)
        return false;
    if (angle_s.Servo3_angle != Chain[3].angle)
        return false;
    if (angle_s.Servo4_angle != Chain[4].angle)
        return false;
    if (angle_s.Servo5_angle != Chain[5].angle)
        return false;
    return true;
}

//根据舵机配置结构体移动各个舵机到对应角度，可选调速配置,不提供的话默认每组移动defdelay
void Move(angle_Setting angle_s, unsigned long defdelay, speed_Setting *speed_s = NULL)
{
    //对各个舵机当前的状态进行操作,根据需要移动的角度判断该怎么动
    //当没达到的时候当然一直进行移动咯
    //每个舵机都进行一次轮询为一组移动

    while (!All_attached(angle_s, ServoChain))
    {
        //判断要移动的方向
        if (ServoChain[0].angle != angle_s.Servo0_angle)
        {
            if (ServoChain[0].angle > angle_s.Servo0_angle)
                ServoChain[0].angle--;
            else
                ServoChain[0].angle++;
            //移动
            ServoChain[0].ThisServo.write(ServoChain[0].angle);
            //依据配速结体或默认设置进行时延
            if (speed_s == NULL)
                delay(defdelay);
            else
                delay(speed_s->Servo0_delay);
        }
        if (ServoChain[1].angle != angle_s.Servo1_angle)
        {
            if (ServoChain[1].angle > angle_s.Servo1_angle)
                ServoChain[1].angle--;
            else
                ServoChain[1].angle++;
            ServoChain[1].ThisServo.write(ServoChain[1].angle);
            if (speed_s == NULL)
                delay(defdelay);
            else
                delay(speed_s->Servo1_delay);
        }
        if (ServoChain[2].angle != angle_s.Servo2_angle)
        {
            if (ServoChain[2].angle > angle_s.Servo2_angle)
                ServoChain[2].angle--;
            else
                ServoChain[2].angle++;
            ServoChain[2].ThisServo.write(ServoChain[2].angle);
            if (speed_s == NULL)
                delay(defdelay);
            else
                delay(speed_s->Servo2_delay);
        }
        if (ServoChain[3].angle != angle_s.Servo3_angle)
        {
            if (ServoChain[3].angle > angle_s.Servo3_angle)
                ServoChain[3].angle--;
            else
                ServoChain[3].angle++;
            ServoChain[3].ThisServo.write(ServoChain[3].angle);
            if (speed_s == NULL)
                delay(defdelay);
            else
                delay(speed_s->Servo3_delay);
        }
        if (ServoChain[4].angle != angle_s.Servo4_angle)
        {
            if (ServoChain[4].angle > angle_s.Servo4_angle)
                ServoChain[4].angle--;
            else
                ServoChain[4].angle++;
            ServoChain[4].ThisServo.write(ServoChain[4].angle);
            if (speed_s == NULL)
                delay(defdelay);
            else
                delay(speed_s->Servo4_delay);
        }
        if (ServoChain[5].angle != angle_s.Servo5_angle)
        {
            if (ServoChain[5].angle > angle_s.Servo5_angle)
                ServoChain[5].angle--;
            else
                ServoChain[5].angle++;
            ServoChain[5].ThisServo.write(ServoChain[5].angle);
            if (speed_s == NULL)
                delay(defdelay);
            else
                delay(speed_s->Servo5_delay);
        }
    }
}

//机械臂复位
void resetServoChain(int How_to_reset)
{
    switch (How_to_reset)
    {
    case READY_TO_CATCH_FROM_STORAGE:
    {
        //机械臂复位使用配置
        angle_Setting resetSetting = {0, 0, 145, 0, 0, 80};
        //看情况需要加入配速
        Move(resetSetting, 5);
        break;
    }
    case READY_TO_CATCH_ITEM:
    {
        //机械臂复位使用配置
        angle_Setting resetSetting = {0, 60, 145, 20, 45, 20};
        //看情况需要加入配速
        Move(resetSetting, 5);
        break;
    }
    default:
        break;
    }
}

//抓取装配台零件并存放
void Catch_Item(uint8_t WhichOne)
{
    //各种动作用Move()来实现,这个部分比较头疼
    angle_Setting to_catch = {0, 55, 145, 60, 100, 20};
    Move(to_catch, 1);
    delay(300);
    to_catch = {0, 55, 145, 50, 105, 20};
    Move(to_catch, 1);
    delay(800);
    to_catch.Servo0_angle = 80;
    Move(to_catch, 1);
    delay(800);
    to_catch = {80, 50, 145, 10, 40, 20};
    Move(to_catch, 1);
    delay(300);
    to_catch = {80, 60, 20, 90, 70, 20};
    Move(to_catch, 1);
    delay(1200);
    switch (WhichOne)
    {
    case 1: //右
    {
        angle_Setting Steps = {80, 95, 10, 25, 32, 30};
        Move(Steps, 2);
        delay(1500);
        //磕碰
        Steps.Servo1_angle = 160;
        Move(Steps, 2);
        delay(500);
        //移动到目标口
        Steps.Servo1_angle = 140;
        Move(Steps, 2);
        delay(300);
        Steps.Servo1_angle = 130;
        Steps.Servo3_angle = 10;
        Move(Steps, 2);
        delay(300);
        //怼进去
        Steps.Servo1_angle = 150;
        Steps.Servo3_angle = 0;
        Move(Steps, 2);
        delay(300);
        //挪出来
        Steps.Servo0_angle = 0;
        Move(Steps, 2);
        delay(300);
        //复位
        resetServoChain(READY_TO_CATCH_ITEM);
        break;
    }
    case 2: //左
    {
        //挪动底盘
        angle_Setting Steps = to_catch;
        Steps.Servo5_angle = 155;
        Move(Steps, 2);
        delay(1000);
        Steps = {80, 95, 10, 25, 32, 155};
        Move(Steps, 2);
        delay(1500);
        //磕碰
        Steps.Servo1_angle = 160;
        Move(Steps, 2);
        delay(500);
        //移动到目标口
        Steps.Servo1_angle = 140;
        Move(Steps, 2);
        delay(300);
        Steps.Servo1_angle = 130;
        Steps.Servo3_angle = 10;
        Move(Steps, 2);
        delay(300);
        //怼进去
        Steps.Servo1_angle = 150;
        Steps.Servo3_angle = 0;
        Move(Steps, 2);
        delay(300);
        //挪出来
        Steps.Servo0_angle = 0;
        Move(Steps, 2);
        delay(300);
        //复位
        Steps = {80, 60, 145, 40, 70, 155};
        Move(Steps, 2);
        delay(300);
        Steps.Servo5_angle = 20;
        Move(Steps, 2);
        delay(1000);
        resetServoChain(READY_TO_CATCH_ITEM);
        break;
    }
    case 3: //右上
    {
        angle_Setting Steps = {80, 95, 10, 25, 32, 65};
        Move(Steps, 2);
        delay(1500);
        //磕碰
        Steps.Servo1_angle = 160;
        Move(Steps, 2);
        delay(500);
        //移动到目标口
        Steps.Servo1_angle = 140;
        Move(Steps, 2);
        delay(300);
        Steps.Servo1_angle = 130;
        Steps.Servo3_angle = 10;
        Move(Steps, 2);
        delay(300);
        //怼进去
        Steps.Servo1_angle = 150;
        Steps.Servo3_angle = 0;
        Move(Steps, 2);
        delay(300);
        //挪出来
        Steps.Servo0_angle = 0;
        Move(Steps, 2);
        delay(300);
        //复位
        resetServoChain(READY_TO_CATCH_ITEM);
        break;
    }
    case 4: //左上
    {
        angle_Setting Steps = {80, 95, 10, 25, 32, 100};
        Move(Steps, 1);
        delay(1500);
        //磕碰
        Steps.Servo1_angle = 160;
        Move(Steps, 1);
        delay(500);
        //移动到目标口
        Steps.Servo1_angle = 140;
        Steps.Servo2_angle = 20;
        Move(Steps, 1);
        delay(500);
        Steps.Servo1_angle = 130;
        Steps.Servo3_angle = 10;
        Move(Steps, 1);
        delay(300);
        //怼进去
        Steps.Servo1_angle = 150;
        Steps.Servo3_angle = 0;
        Move(Steps, 1);
        delay(300);
        //挪出来
        Steps.Servo0_angle = 0;
        Move(Steps, 1);
        delay(300);
        //复位
        resetServoChain(READY_TO_CATCH_ITEM);
        break;
    }
    case 5: //捏手里
    {
        angle_Setting Steps = {80, 95, 10, 25, 32, 80};
        Move(Steps, 2);
        delay(1500);
        //磕碰
        Steps.Servo1_angle = 160;
        Move(Steps, 2);
        delay(500);
        //对准
        Steps = {80, 0, 20, 5, 40, 80};
        Move(Steps, 2);
        delay(500);
    }
    default:
        break;
    }
    //抓完之后记得放进去然后归位
}

//抓取存储内零件
void Catch_Item_From_Storage(uint8_t WhichOne)
{
    //初期准备角度

    angle_Setting prepare = {0, 0, 20, 10, 90, 80};
    Move(prepare, 1);
    delay(1200);
    prepare = {20, 120, 20, 40, 90, 80};
    Move(prepare, 1);
    delay(500);
    //同样，使用Move()来实现
    switch (WhichOne)
    {
    case 1: //手上的
    {
        delay(100);
        break;
    }
    case 4: //左
    {
        //继承初始化角度，转移到对应存储区
        angle_Setting Steps = prepare;
        //先移动底盘对准，再进行操作
        Steps.Servo5_angle = 140;
        Move(Steps, 1);
        delay(500);
        //微调位置
        Steps.Servo4_angle = 60;
        Steps.Servo1_angle = 100;
        Move(Steps, 1);
        Steps.Servo0_angle = 5;
        Steps.Servo3_angle = 10;
        Move(Steps, 5);
        delay(500);
        //机械爪靠近
        Steps.Servo1_angle = 130;
        Move(Steps, 5);
        delay(150);
        //薅螺栓X2
        for (int count = 0; count < 2; count++)
        {
            Steps.Servo1_angle = 110;
            Steps.Servo2_angle = 23;
            Move(Steps, 5);
            //这个延时确保薅到位
            delay(100);
            Steps.Servo1_angle = 140;
            Steps.Servo2_angle = 13;
            Move(Steps, 5);
        }
        //这里之前的Steps{10,140,13,10,60,140}
        //校正机械臂对准螺栓正中
        Steps = {0, 135, 20, 15, 60, 140};
        Move(Steps, 5);
        delay(50);
        //抓住螺栓
        Steps.Servo0_angle = 100;
        Move(Steps, 5);
        delay(100);
        //拿出来
        Steps = {100, 160, 20, 50, 65, 140};
        Move(Steps, 10);
        delay(50);
        //挪动到中线
        Steps.Servo5_angle = 80;
        Move(Steps, 10);
        delay(50);
        //抬起
        Steps.Servo4_angle = 40;
        Move(Steps, 10);
        delay(50);
        break;
    }
    case 2: //左上
    {
        //继承初始化角度，转移到对应存储区
        angle_Setting Steps = prepare;
        //先移动底盘对准，再进行操作
        Steps.Servo5_angle = 93;
        Move(Steps, 1);
        delay(500);
        //微调位置
        Steps.Servo4_angle = 60;
        Steps.Servo1_angle = 100;
        Move(Steps, 1);
        Steps.Servo0_angle = 5;
        Steps.Servo3_angle = 10;
        Move(Steps, 5);
        delay(500);
        //机械爪靠近
        Steps.Servo1_angle = 130;
        Move(Steps, 5);
        delay(150);
        //薅螺栓X2
        for (int count = 0; count < 2; count++)
        {
            Steps.Servo1_angle = 100;
            Steps.Servo2_angle = 23;
            Move(Steps, 5);
            //这个延时确保薅到位
            delay(100);
            Steps.Servo1_angle = 130;
            Steps.Servo2_angle = 13;
            Move(Steps, 5);
        }
        //这里之前的Steps{10,110,13,10,60,93}
        //校正机械臂对准螺栓正中
        Steps = {0, 123, 20, 17, 65, 93};
        Move(Steps, 5);
        delay(50);
        //抓住螺栓
        Steps.Servo0_angle = 100;
        Move(Steps, 5);
        delay(100);
        //拿出来
        Steps = {100, 160, 20, 50, 65, 93};
        Move(Steps, 10);
        delay(50);
        //挪动到中线
        Steps.Servo5_angle = 80;
        Move(Steps, 10);
        delay(50);
        //抬起
        Steps.Servo4_angle = 40;
        Move(Steps, 10);
        delay(50);
        break;
    }
    case 3: //右上
    {
        //继承初始化角度，转移到对应存储区
        angle_Setting Steps = prepare;
        //先移动底盘对准，再进行操作
        Steps.Servo5_angle = 55;
        Move(Steps, 1);
        delay(500);
        //微调位置
        Steps.Servo4_angle = 60;
        Steps.Servo1_angle = 100;
        Move(Steps, 1);
        Steps.Servo0_angle = 5;
        Steps.Servo3_angle = 10;
        Move(Steps, 5);
        delay(500);
        //机械爪靠近
        Steps.Servo1_angle = 130;
        Move(Steps, 5);
        delay(150);
        //薅螺栓X2
        for (int count = 0; count < 2; count++)
        {
            Steps.Servo1_angle = 100;
            Steps.Servo2_angle = 23;
            Move(Steps, 5);
            //这个延时确保薅到位
            delay(100);
            Steps.Servo1_angle = 130;
            Steps.Servo2_angle = 13;
            Move(Steps, 5);
        }
        //这里之前的Steps{10,110,13,10,60,55}
        //校正机械臂对准螺栓正中
        Steps = {0, 125, 20, 17, 65, 55};
        Move(Steps, 5);
        delay(50);
        //抓住螺栓
        Steps.Servo0_angle = 100;
        Move(Steps, 5);
        delay(100);
        //拿出来
        Steps = {100, 160, 20, 50, 65, 55};
        Move(Steps, 10);
        delay(50);
        //挪动到中线
        Steps.Servo5_angle = 80;
        Move(Steps, 10);
        delay(50);
        //抬起
        Steps.Servo4_angle = 40;
        Move(Steps, 10);
        delay(50);
        break;
    }
    case 5: //右
    {
        //继承初始化角度，转移到对应存储区
        angle_Setting Steps = prepare;
        //先移动底盘对准，再进行操作
        Steps.Servo5_angle = 10;
        Move(Steps, 1);
        delay(500);
        //微调位置
        Steps.Servo4_angle = 60;
        Steps.Servo1_angle = 100;
        Move(Steps, 1);
        Steps.Servo0_angle = 5;
        Steps.Servo3_angle = 10;
        Move(Steps, 5);
        delay(500);
        //机械爪靠近
        Steps.Servo1_angle = 130;
        Move(Steps, 5);
        delay(150);
        //薅螺栓X2
        for (int count = 0; count < 2; count++)
        {
            Steps.Servo1_angle = 110;
            Steps.Servo2_angle = 23;
            Move(Steps, 5);
            //这个延时确保薅到位
            delay(100);
            Steps.Servo1_angle = 140;
            Steps.Servo2_angle = 13;
            Move(Steps, 5);
        }
        //这里之前的Steps{10,140,13,10,60,10}
        //校正机械臂对准螺栓正中
        Steps = {0, 135, 20, 15, 60, 10};
        Move(Steps, 5);
        delay(50);
        //抓住螺栓
        Steps.Servo0_angle = 100;
        Move(Steps, 5);
        delay(100);
        //拿出来
        Steps = {100, 160, 20, 50, 65, 10};
        Move(Steps, 10);
        delay(50);
        //挪动到中线
        Steps.Servo5_angle = 80;
        Move(Steps, 10);
        delay(50);
        //抬起
        Steps.Servo4_angle = 40;
        Move(Steps, 10);
        delay(50);
        break;
    }
    default:
        break;
    }
    //抓取完之后应该将机械臂置于刚好对准的状态
    if (WhichOne != 1)
    {
        angle_Setting focus_On = {100, 110, 10, 18, 10, 85};
        Move(focus_On, 1);
        delay(1000);
    }
}

//投放零件到装配体
void PutItem(uint8_t WhichOne)
{
    //使用Move()实现
    switch (WhichOne)
    {
    case 1:
    { //第一个投放,特殊投放
        angle_Setting Steps = Get_Angle_Setting_FromChain(ServoChain);
        Steps.Servo0_angle=0;
        delay(200);
        break;
    }
    //余下普通投放
    case 2:
    case 3:
    case 4:
    case 5:
    {
        //上一条务必务必是CatchItem()
        //阶段配置结构体
        angle_Setting Steps = {100, 110, 10, 18, 10, 85};
        Move(Steps, 1);
        delay(2000);
        //摸索投放口
        for (int countter = 0; countter < 3; countter++)
        {
            Steps.Servo2_angle = 20;
            Move(Steps, 20);
            delay(20);
            Steps.Servo1_angle += 2;
            Steps.Servo3_angle--;
            Move(Steps, 10);
            delay(20);
            Steps.Servo2_angle = 10;
            Move(Steps, 20);
            delay(20);
            Steps.Servo1_angle += 2;
            Steps.Servo3_angle--;
            Move(Steps, 10);
            delay(20);
        }
        Steps = {100, 120, 7, 18, 10, 80};
        Move(Steps, 1);
        delay(500);
        //推进去
        Steps = {80, 90, 10, 10, 30, 80};
        Move(Steps, 1);
        delay(500);
        //放开
        Steps.Servo0_angle = 0;
        Move(Steps, 1);
        delay(500);
        //抬头
        Steps.Servo1_angle = 30;
        Move(Steps, 1);
        delay(500);
    }
    default:
        break;
    }
}

//等待信息,在等到约定的信息之前一直delay
void Wait_For_signal()
{
    String Cmd;
    while (1)
    {
        if (Serial.available())
            //重点,从约定的'.'符号开始读取命令，滤掉杂波
            if (Serial.read() == '.')
            {
                //以约定的'\'结尾，中间的就是命令
                Cmd = Serial.readStringUntil('\\');
                Serial.println(Cmd);
            }
        if (Cmd == "Go.")
            break;
    }
}

//发送信息
void Send_signal()
{
    Serial.println(".Go.\\");
}

//设置阶段
void setup()
{
    //舵机串的设置，不可手动更改！！！！
    ServoChain[0].ThisServo.attach(Servo1);
    ServoChain[1].ThisServo.attach(Servo2);
    ServoChain[2].ThisServo.attach(Servo3);
    ServoChain[3].ThisServo.attach(Servo4);
    ServoChain[4].ThisServo.attach(Servo5);
    ServoChain[5].ThisServo.attach(Servo6);

    //预装填
    ServoChain[0].angle = 40;
    ServoChain[1].angle = 0;
    ServoChain[2].angle = 145;
    ServoChain[3].angle = 0;
    ServoChain[4].angle = 0;
    ServoChain[5].angle = 45;

    //初期读写,不然舵机内的值是未知的
    ServoChain[0].ThisServo.write(ServoChain[0].angle);
    ServoChain[1].ThisServo.write(ServoChain[1].angle);
    ServoChain[2].ThisServo.write(ServoChain[2].angle);
    ServoChain[3].ThisServo.write(ServoChain[3].angle);
    ServoChain[4].ThisServo.write(ServoChain[4].angle);
    ServoChain[5].ThisServo.write(ServoChain[5].angle);
    //初始舵机归位
    Serial.begin(9600);
    //复位延时
    delay(2000);
}

//主流程
void loop()
{
    //等待开始
    Wait_For_signal();
    resetServoChain(READY_TO_CATCH_ITEM);
    delay(1000);
    //开始信号
    Send_signal();
    //抓取测试
    for (int count = 1; count < 5; count++)
    {
        Wait_For_signal();
        Catch_Item(count);
            resetServoChain(READY_TO_CATCH_ITEM);
        delay(200);
        Send_signal();
    }
    //投放4个
    for (int count = 2; count < 6; count++)
    {
        delay(500);
            resetServoChain(READY_TO_CATCH_FROM_STORAGE);
        Wait_For_signal();
        Catch_Item_From_Storage(count);
        Send_signal();
        Wait_For_signal();
        PutItem(count);
        Send_signal();
    }
    delay(500);
    resetServoChain(READY_TO_CATCH_ITEM);
    Wait_For_signal();
    Catch_Item(5);
    delay(200);
    Send_signal();
    delay(500);
    Wait_For_signal();
    Catch_Item_From_Storage(1);
    Send_signal();
    Wait_For_signal();
    PutItem(1);
    Send_signal();
    resetServoChain(READY_TO_CATCH_FROM_STORAGE);

    while (1)
        ;
    //舵机复位
    resetServoChain(READY_TO_CATCH_FROM_STORAGE);
    delay(2000);
    Catch_Item_From_Storage(2);
    delay(2000);
    //舵机复位
    /*
    resetServoChain();
    delay(2000);
    Catch_Item_From_Storage(3);
    delay(4000);
    //舵机复位
    resetServoChain();
    delay(2000);
    Catch_Item_From_Storage(1);
    delay(4000);
    //舵机复位
    resetServoChain();
    delay(2000);
    Catch_Item_From_Storage(4);
    delay(1000);
    //*/
    PutItem(2);
    while (1)
        ;
    //等待信息
    //Wait_For_Signal();
    //循环抓取零件
    for (int count = 0; count < 5; count++)
    {
        Catch_Item(count);
        //抓取完毕后发送信息并等待
        //Send_Signal();
        //Wait_For_Signal();
    }

    //此处抓取部分完成，小车进入装配区域
    //理想状态应该是4个存储区放满，手上抓着一个
    //等待信息
    //Wait_For_Signal()
    //循环拿取存储并装配
    for (int count = 0; count < 5; count++)
    {
        //等待小车就位后从存储抓取零件
        //Wait_For_Signal()
        Catch_Item_From_Storage(count);
        //抓取完毕后发送信息并等待下层就绪
        //Send_Signal()
        //Wait_For_Signal()
        //循环装配
        PutItem(count);
        //发送信息表示投放完成
        //Send_Signal()
    }
}