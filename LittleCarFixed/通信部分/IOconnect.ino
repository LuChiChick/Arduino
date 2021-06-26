#define LedPin 6
void setup()
{
  Serial.begin(9600);
  pinMode(6,OUTPUT);
}

void Wait_For_signal()
{
    String Cmd;
    while(1){
        if(Serial.available())
            //重点,从约定的'.'符号开始读取命令，滤掉杂波
            if(Serial.read()=='.'){
                //以约定的'\'结尾，中间的就是命令
                Cmd=Serial.readStringUntil('\\');
                Serial.println(Cmd);
            }
        if(Cmd=="Go.")
            break;
    }
}

void Send_signal(){
    Serial.println(".Go.\\");
}


void loop()
{
  /*Uno测试
  Wait_For_signal();
  digitalWrite(LedPin,HIGH);
  Wait_For_signal();
  digitalWrite(LedPin,LOW);
  //*/
  ///*
  Send_signal();
  while(1);
  //*/
}