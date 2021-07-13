//实验时用6口接了一个LED
#define LedPin 6


void setup()
{
    //通信板子之间Serial的频率要相同
    //两块板子Tx与Rx口要反接，并且两块板子需要将GND接到一起
    Serial.begin(9600);
    pinMode(6, OUTPUT);
}

//等待约定的信息,这里是".Go.\",若没有等到则一直delay
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

//发送约定的信息,这里是".Go.\"
void Send_signal()
{
    Serial.println(".Go.\\");
}

void loop()
{
    /*上位机测试
    Wait_For_signal();
    digitalWrite(LedPin,HIGH);
    Wait_For_signal();
    digitalWrite(LedPin,LOW);
    //*/
    ///*下位机测试
    Send_signal();
    while (1)
        ;
    //*/
}