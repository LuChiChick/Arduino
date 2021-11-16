#ifndef __STEPPER_MOTOR__
#define __STEPPER_MOTOR__

#define DEF_DIVIDER 1
#define DEF_STEPPER_THRESHOLD_US 0
//使能接线枚举类型
enum EnableType
{
    LOW_TO_ENABLE,
    HIGH_TO_ENABLE
};
//转向接线枚举类型
enum DirectType
{
    LOW_TO_CLOCKWISE,
    HIGH_TO_CLOCKWISE
};
//方向枚举类型
enum Direction
{
    ANTICLOCKWISE,
    CLOCKWISE
};
namespace std
{
    //步进电机类
    class StepperMoter
    {
    protected:
        //引脚
        uint8_t dir_Pin;
        uint8_t pul_Pin;
        uint8_t ena_Pin;
        //使能类型记录
        EnableType level_To_Enable;
        //方向类型记录
        DirectType level_To_Clockwise;
        bool isEnable;
        //分步信息
        uint64_t pulse_Per_round;
        //脉冲计数器
        double pulse_count;
        //减速比信息(加装了减速器什么的)
        double divider;
        //内部信息记录
        double angle;
        Direction now_direction;
        //脉冲间歇阈值
        uint64_t threshold;

    public:
        //默认构造函数
        StepperMoter();
        /**
        * 构造函数
        * @param dir_Pin 方向引脚
        * @param pul_Pin 脉冲引脚
        * @param ena_Pin 使能引脚
        * @param pulse_Per_round 分步值
        * @param divider 减速比
        */
        StepperMoter(uint8_t dir_Pin,
                     uint8_t pul_Pin,
                     uint8_t ena_Pin,
                     uint64_t pulse_Per_round,
                     double divider);
        /**
        * 带使能方向的构造函数
        * @param dir_Pin 方向引脚
        * @param pul_Pin 脉冲引脚
        * @param ena_Pin 使能引脚
        * @param pulse_Per_round 分步值
        * @param level_To_Enable 使能类型记录
        * @param level_To_Clockwise 顺时针旋转类型记录
        * @param divider 减速比
        */
        StepperMoter(uint8_t dir_Pin,
                     uint8_t pul_Pin,
                     uint8_t ena_Pin,
                     uint64_t pulse_Per_round,
                     EnableType level_To_Enable,
                     DirectType level_To_Clockwise,
                     double divider);
        //单纯的脉冲，也没有度数计算什么的
        void justPulse();
        /**
         * 单纯的脉冲指定次数
         * @param pulse_count 指定脉冲次数
         */
        void justPulse(uint64_t pulse_count);
        //脉冲一次
        void pulse();
        /**
        * 脉冲指定次数
        * @param pulse_count 脉冲计数
        */
        void pulse(uint64_t pulse_count);
        /**
        * 切换方向
        * @param changeTo 新的方向枚举Direction类型
        */
        void setDirection(Direction changeTo);
        /**
        * 设置默认脉冲间隔阈值 us
        * @param threshold 阈值
        */
        void setThreshold(uint64_t threshold);
        //转圈
        void turnAround();
        /**
        * 旋转指定圈
        * @param turns 圈计数
        */
        void turnRounds(uint64_t turns);
        /**
         * 旋转到指定角度
         * @param angle 指定的角度
         */
        void goTo(double angle);
        //启用使能
        void enable();
        //关闭使能
        void disable();
        //返回角度
        double getAngle();
        /**
         * 设置角度
         * @param angle 新设置的角度
         */
        void setAngle(double angle);
        //返回减速比信息(加装了减速器什么的)
        double getDivider();
        //返回阈值
        uint64_t getThreshold();
        //返回圈脉冲数
        uint64_t getPulse_Per_round();
        //返回方向
        Direction getDirection();
    };
}

#endif