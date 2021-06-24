#ifndef __MPU6050__
#define __MPU6050__
#include <Wire.h>

//平台为ArduinoUno/Arduino Nano
//需要用到I2C通信,data数据引脚接A4,clock时钟引脚接A5
//Mega下data脚为D20,clock脚为D21
//Processing可视化参考:https://www.howtomechatronics.com

namespace std
{
    const char MPU_Addr = 0x68;//MPU6050的I2C地址
    class MPU6050
    {
    private:
        float AccX, AccY, AccZ;                                         //各轴加速度,默认最小时(值为-32768)表示2g
        float AccErrX, AccErrY, AccErrZ;                                //各轴加速度误差
        float GyroX, GyroY, GyroZ;                                      //各轴角速度,单位°/s
        float GyroErrX, GyroErrY, GyroErrZ;                             //各轴角速度误差
        /*
            //加权积分时使用
            //float accAngleX, accAngleY,gyroAngleX, gyroAngleY, gyroAngleZ; //各加速度几何增量
        */
        float roll, pitch, yaw;                                         //欧拉转角
        float elapsedTime, currentTime, previousTime;                   //时基数据
    protected:
        void GetData();                                                 //获取所有数值数值
        void WeakUP();                                                  //唤醒MPU6050

    public:
        MPU6050();         //构造函数
        void Correction(); //误差校正
        
        float Get_roll() //返回绕X轴旋转的角度
        {
            GetData();
            return roll;
        }

        float Get_yaw()//返回绕Z轴的角度
        {
            GetData();
            return yaw;
        }

        float Get_pitch()//返回绕Y轴的角度
        {
            GetData();
            return pitch;
        }

    };

};


//构造函数定义
std::MPU6050::MPU6050()
{
    //唤醒MPU
    WeakUP();
    //初始化各数据为0
    AccX = AccY = AccZ = 0;
    AccErrX = AccErrY = AccErrZ = 0;
    GyroX = GyroY = GyroZ = 0;
    GyroErrX = GyroErrY = GyroErrZ = 0;
    /*
        //加权积分时使用
        //gyroAngleX=gyroAngleY=0;
    */
    yaw=roll=pitch=0;
    //记录当前时间以备后续积分运算
    currentTime = millis();
    //初始数据校正
    Correction();
}


//唤醒MPU6050
void std::MPU6050::WeakUP()
{
    Wire.begin();                          //开始I2C通信
    Wire.beginTransmission(std::MPU_Addr); //开始与MPU6050通信
    Wire.write(0x6B);                      // 定位到断点寄存器6B
    Wire.write(0x00);                      // 向6B寄存器写入00来唤醒MPU6050
    Wire.endTransmission(true);            //结束通信
}


//通过在静止状态下收集200组数据求平均值来确定各轴数据误差,并消除地球自转的影响
//仅限小车等运动距离不大的物体,若运动距离过大可能需要重新校准
void std::MPU6050::Correction()
{
    //初始化各数据为0
    AccX = AccY = AccZ = 0;
    AccErrX = AccErrY = AccErrZ = 0;
    GyroX = GyroY = GyroZ = 0;
    GyroErrX = GyroErrY = GyroErrZ = 0;
    /*
        //加权积分时使用
        //gyroAngleX=gyroAngleY=0;
    */
    yaw=roll=pitch=0;
    for (int c = 0; c < 200; c++)
    {
        Wire.beginTransmission(std::MPU_Addr);
        //0x3B指向X轴加速度计(AccX)的首地址,每个加速度计数据顺序排列,每个数据占2个8位寄存器共16位
        //也就是需要向地址0x3B开始顺序读取6个8位并两两拼接得到各轴数据
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(std::MPU_Addr, 6, true);
        AccErrX += (Wire.read() << 8 | Wire.read()) / 16384.0;
        AccErrY += (Wire.read() << 8 | Wire.read()) / 16384.0;
        AccErrZ += (Wire.read() << 8 | Wire.read()) / 16384.0;
    }
    AccErrX /= 200;
    AccErrY /= 200;
    AccErrZ /= 200;
    for (int c = 0; c < 200; c++)
    {
        Wire.beginTransmission(std::MPU_Addr);
        Wire.write(0x43); //X轴角速度首地址(GryoX),同上为顺序6个8字节数据
        Wire.endTransmission(false);
        Wire.requestFrom(std::MPU_Addr, 6, true);             
        GyroErrX += (Wire.read() << 8 | Wire.read()) / 131.0;
        GyroErrY += (Wire.read() << 8 | Wire.read()) / 131.0;
        GyroErrZ += (Wire.read() << 8 | Wire.read()) / 131.0;
    }
    GyroErrX /= 200;
    GyroErrY /= 200;
    GyroErrZ /= 200;
    Wire.endTransmission(true);
    //记录当前时间以备后续积分运算
    currentTime = millis();
}

void std::MPU6050::GetData()
{
    Wire.beginTransmission(std::MPU_Addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(std::MPU_Addr, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 - AccErrX;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 - AccErrY;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 - AccErrZ;
    Wire.beginTransmission(std::MPU_Addr);
    Wire.write(0x43); 
    Wire.endTransmission(false);
    Wire.requestFrom(std::MPU_Addr, 6, true);
    GyroX = (Wire.read() << 8 | Wire.read()) / 131.0 - GyroErrX;
    GyroY = (Wire.read() << 8 | Wire.read()) / 131.0 - GyroErrY;
    GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0 - GyroErrZ;
    Wire.endTransmission(true);

    //开始从收集到的数据中计算欧拉转角

    /*
        //加权积分时使用计算
        //X Y轴加速度几何转角
        accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI);      
        accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI); 
    */

    //处理时基数据
    previousTime = currentTime;                        //初始化时或上一次计算时的时间被记录到previousTime里
    currentTime = millis();                            // 获取当前时间
    elapsedTime = (currentTime - previousTime) / 1000; // 经过的时间为二者差值,除以1000得到秒
    //积分求欧拉转角

    //直接积分法,数据不如下方加权积分敏感,但数值稳定
    yaw += GyroZ * elapsedTime;
    roll += GyroX*elapsedTime;
    pitch+=GyroY*elapsedTime;
    //加权积分,可选的积分方式之一,数值敏感但抖动
    /*
        //需要解放前面的声明
        gyroAngleX = gyroAngleX + GyroX * elapsedTime;     // 角速度乘以时间间隔即为角度增量,当时间间隔很短时这个增量是接近积分的
        gyroAngleY = gyroAngleY + GyroY * elapsedTime;      //同理
        //权重滤波,对波动较大的accAngle分权重为0.04
        roll = 0.96 * gyroAngleX + 0.04 * accAngleX;//roll和pitch比yaw更灵敏,但是有波动
        pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
    */
}

#endif

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    std::MPU6050 MPU;
    while (1)
    {
        Serial.print(MPU.Get_pitch());
        Serial.print("/");
        Serial.println(MPU.Get_yaw());
    }
}