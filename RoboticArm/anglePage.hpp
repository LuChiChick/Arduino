#ifndef __ANGLE_PAGE__
#define __ANGLE_PAGE__

namespace std
{
    struct angleInfo
    {
        double angles[6];
        angleInfo *nextPage;
    };

    class anglePage
    {
    private:
        //链表页
        angleInfo *startPage;
        int pageCount;

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
    };
}

#endif