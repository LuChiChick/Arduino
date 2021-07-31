//库包含
#include <Wire.h>

//宏定义
#define LightToggle 0x66

//设置阶段
void setup()
{
  Serial.begin(9600);
  Wire.begin(0xCC);   // 注册设备地址为0xCC
  Wire.onReceive(ReceivedHandler); //注册中断处理函数为RecivedHandler,被呼叫时调用
}
//主循环
void loop()
{
  // 平常没事做
}

//中断处理
void ReceivedHandler(int n)
{
  for(int c=0;c<n;c++){
    Serial.print(Wire.read());
    Serial.print('\\');
  }
  Serial.print('\n');
}