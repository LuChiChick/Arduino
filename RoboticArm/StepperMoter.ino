#include "StepperMotor.hpp"
//14，19，27
namespace std
{
    //默认构造函数
    StepperMoter::StepperMoter()
    {
    }

    //构造函数
    StepperMoter::StepperMoter(uint8_t dir_Pin,
                               uint8_t pul_Pin,
                               uint8_t ena_Pin,
                               uint64_t pulse_Per_round,
                               double divider)
    {
        //引脚模式定义
        pinMode(dir_Pin, OUTPUT);
        pinMode(pul_Pin, OUTPUT);
        pinMode(ena_Pin, OUTPUT);
        //信息记录
        this->pulse_Per_round = pulse_Per_round;
        this->divider = (divider >= 0) ? divider : DEF_DIVIDER;
        this->pul_Pin = pul_Pin;
        this->dir_Pin = dir_Pin;
        this->ena_Pin = ena_Pin;
        //默认位置使能
        this->level_To_Enable = HIGH_TO_ENABLE;
        this->level_To_Clockwise = HIGH_TO_CLOCKWISE;
        digitalWrite(dir_Pin, level_To_Clockwise);
        digitalWrite(ena_Pin, level_To_Enable);
        //默认脉冲间隔阈值
        this->threshold = DEF_STEPPER_THRESHOLD_US;
        //数值初始化
        this->now_direction = CLOCKWISE;
        this->pulse_count = 0;
        this->angle = 0;
        this->isEnable = true;
    }

    //带使能方向的构造函数
    StepperMoter::StepperMoter(uint8_t dir_Pin,
                               uint8_t pul_Pin,
                               uint8_t ena_Pin,
                               uint64_t pulse_Per_round,
                               EnableType level_To_Enable,
                               DirectType level_To_Clockwise,
                               double divider)
    {
        //引脚模式定义
        pinMode(dir_Pin, OUTPUT);
        pinMode(pul_Pin, OUTPUT);
        pinMode(ena_Pin, OUTPUT);
        //信息记录
        this->pulse_Per_round = pulse_Per_round;
        this->divider = (divider >= 0) ? divider : DEF_DIVIDER;
        this->level_To_Enable = level_To_Enable;
        this->level_To_Clockwise = level_To_Clockwise;
        this->pul_Pin = pul_Pin;
        this->dir_Pin = dir_Pin;
        this->ena_Pin = ena_Pin;
        //使能
        digitalWrite(dir_Pin, level_To_Clockwise);
        digitalWrite(ena_Pin, level_To_Enable);
        //默认脉冲间隔阈值
        this->threshold = DEF_STEPPER_THRESHOLD_US;
        //数值初始化
        this->pulse_count = 0;
        this->now_direction = CLOCKWISE;
        this->angle = 0;
        this->isEnable = true;
    }

    //脉冲
    void StepperMoter::pulse()
    {
        pulse(1);
    }
    void StepperMoter::pulse(uint64_t pulse_count)
    {
        //检查启用
        if (!isEnable)
            return;
        for (uint64_t count = 0; count < pulse_count; count++)
        {

            //脉冲，更新内部计数
            digitalWrite(pul_Pin, HIGH);
            delayMicroseconds(threshold);
            digitalWrite(pul_Pin, LOW);
        }
        //计算并更新角度,脉冲计数/周期脉冲数=电机轴角度,转轴角度=电机轴/减速套件比
        if (now_direction == CLOCKWISE)
            this->pulse_count += pulse_count;
        else
            this->pulse_count -= pulse_count;
        //角度计算
        angle = ((this->pulse_count / pulse_Per_round) / divider) * 360.00;
    }
    //单纯的脉冲，也没有度数计算什么的
    void StepperMoter::justPulse()
    {
        justPulse(1);
    }
    //单纯的脉冲，也没有度数计算什么的
    void StepperMoter::justPulse(uint64_t pulse_count)
    {
        for (uint64_t count = 0; count < pulse_count; count++)
        {
            //脉冲
            digitalWrite(pul_Pin, HIGH);
            delayMicroseconds(threshold);
            digitalWrite(pul_Pin, LOW);
        }
    }
    //切换方向
    void StepperMoter::setDirection(Direction changeTo)
    {
        this->now_direction = changeTo;
        //使能方向
        if (now_direction == CLOCKWISE)
            digitalWrite(dir_Pin, this->level_To_Clockwise);
        else
            digitalWrite(dir_Pin, !this->level_To_Clockwise);
    }
    //设置默认脉冲间隔阈值 us
    void StepperMoter::setThreshold(uint64_t threshold)
    {
        this->threshold = threshold;
    }
    //转一圈
    void StepperMoter::turnAround()
    {
        turnRounds(1);
    }
    void StepperMoter::turnRounds(uint64_t turns)
    {
        //检查启用
        if (!isEnable)
            return;
        for (uint64_t count = 0; count < turns * this->divider; count++)
            pulse(this->pulse_Per_round);
    }
    //转到指定角度
    void StepperMoter::goTo(double angle)
    {
        //检查启用与角度
        if (!isEnable || this->angle == angle)
            return;
        //方向记录
        Direction preDir = now_direction;
        //检查方向
        if (angle > this->angle)
            setDirection(CLOCKWISE);
        else
            setDirection(ANTICLOCKWISE);

        //临时值，用于保存角度之差
        double angleTogo;
        //计算角度差
        angleTogo = angle - this->angle;
        if (angleTogo < 0)
            angleTogo = -angleTogo;

        pulse(angleTogo / 360.0 * pulse_Per_round * divider);
        //回到之前的方向
        setDirection(preDir);
    }
    //启用使能
    void StepperMoter::enable()
    {
        digitalWrite(this->ena_Pin, this->level_To_Enable);
        isEnable = true;
    }
    //关闭使能
    void StepperMoter::disable()
    {
        digitalWrite(this->ena_Pin, !this->level_To_Enable);
        isEnable = false;
    }
    //返回角度
    double StepperMoter::getAngle()
    {
        return angle;
    }
    //设置角度
    void StepperMoter::setAngle(double angle)
    {
        //检查启用与角度
        if (!isEnable)
            return;
        //更新信息
        this->angle = angle;
        pulse_count = angle / 360 * divider * pulse_Per_round;
    }
    //返回减速比信息(加装了减速器什么的)
    double StepperMoter::getDivider()
    {
        return this->divider;
    }
    //返回阈值
    uint64_t StepperMoter::getThreshold()
    {
        return this->threshold;
    }
    //返回方向
    Direction StepperMoter::getDirection()
    {
        return this->now_direction;
    }
    //返回圈脉冲数
    uint64_t StepperMoter::getPulse_Per_round()
    {
        return pulse_Per_round;
    }
}