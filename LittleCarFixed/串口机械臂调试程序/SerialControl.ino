#include <Servo.h>
#include <string.h>
#define Servo1 10
#define Servo2 A2
#define Servo3 A3
#define Servo4 A0
#define Servo5 A1
#define Servo6 7
struct Servos
{
    Servo ThisServo;
    uint8_t angle;
};

Servos ServoChain[6];
void Roll_to(Servos AimServo,uint8_t angle){
    if(AimServo.angle<angle){
        uint8_t times=(angle-AimServo.angle)/10;
        for(int count=0;count<times;count++){
            AimServo.ThisServo.write(AimServo.angle+10*times);
            delay(100);
        }
        AimServo.angle=angle;
        AimServo.ThisServo.write(angle);
    }
    else{
        uint8_t times=(AimServo.angle-angle)/10;
        for(int count=0;count<times;count++){
            AimServo.ThisServo.write(AimServo.angle-10*times);
            delay(100);
        }
        AimServo.angle=angle;
        AimServo.ThisServo.write(angle);
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
    //舵机初始角度,迫切需要修改！！！！！！！
    ServoChain[0].angle = 0;
    ServoChain[1].angle = 0;
    ServoChain[2].angle = 0;
    ServoChain[3].angle = 0;
    ServoChain[4].angle = 0;
    ServoChain[5].angle = 0;
    ServoChain[0].ThisServo.write(ServoChain[1].angle);
    ServoChain[1].ThisServo.write(ServoChain[2].angle);
    ServoChain[2].ThisServo.write(ServoChain[3].angle);
    ServoChain[3].ThisServo.write(ServoChain[4].angle);
    ServoChain[4].ThisServo.write(ServoChain[5].angle);
    ServoChain[5].ThisServo.write(ServoChain[6].angle);
    Serial.begin(9600);
}
String Cmd1, Cmd2;
uint8_t Aim = 0;
void loop()
{
    if (Serial.available() > 0)
    {
        Cmd1 = Serial.readStringUntil(' ');
        Cmd2 = Serial.readString();
    }
    if (Cmd1 == "set")
    {
        uint8_t angle;
        sscanf(Cmd2.c_str(), "%d", &angle);
        //ServoChain[Aim].ThisServo.write(angle);
        //ServoChain[Aim].angle = angle;
        Roll_to(ServoChain[Aim],angle);
        Serial.print("[Servo#");
        Serial.print(Aim);
        Serial.print("]:set to ");
        Serial.print(ServoChain[Aim].angle);
        Serial.println("°");
    }
    if (Cmd1 == "to")
    {
        uint8_t Aim_New;
        sscanf(Cmd2.c_str(), "%d", &Aim_New);
        if (Aim_New <= 5 && Aim_New >= 0)
            Aim = Aim_New;
        Serial.print("Switch to Servo#");
        Serial.println(Aim);
    }
    if (Cmd1 == "show")
        if (Cmd2 == "list\n")
        {
            for (int count = 0; count < 6; count++)
            {
                Serial.print("[Servo#");
                Serial.print(count);
                Serial.print("]:");
                Serial.print(ServoChain[count].angle);
                Serial.println("°");
            }
        }

    Cmd1 = Cmd2 = " ";
}