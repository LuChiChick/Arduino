#ifndef __ANGLE_PAGE__
#define __ANGLE_PAGE__

#include "StepperMotor.hpp"

namespace std
{
    struct angleInfo
    {
        double angleGroup[6];   //角度组
        uint64_t pulseGroup[6]; //脉冲组
        bool tagGroup[6];       //标志组
        Direction dirGroup[6];  //方向组
        angleInfo *nextPage;
    };

    class anglePage
    {
    private:
        //链表页
        angleInfo *startPage;
        //页数据计数
        int pageCount;
        //最新角度缓存
        double latestAngles[6];

    public:
        //构造函数
        anglePage();
        /**
         * 重定义操作符,写入页
         * @param arr 输入的角度值所在数组
         */
        anglePage &operator<<(const double *arr);
        /**
         * 重定义操作符,输出页
         * @param arr 输出的角度值所在数组
         */
        anglePage &operator>>(double *arr);
        /**
         * 检查链表是否为空
         * @return 是否为空
         */
        bool isEmpty();
        uint64_t *getPulseGroup();
    };
}

#endif