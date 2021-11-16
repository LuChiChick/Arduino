#include "anglePage.hpp"

namespace std
{
    anglePage::anglePage()
    {
        //置零
        startPage = NULL;
        //分配空间
        startPage = (angleInfo *)malloc(sizeof(angleInfo));

        //保险设置
        if (startPage != NULL)
            startPage->nextPage = NULL;
        //计数清零
        pageCount = 0;
    }
    //存入链表
    anglePage &anglePage::operator<<(const double *arr)
    {
        //目标链表
        angleInfo *AimPage = startPage;

        //寻找末尾
        while (1)
            if (AimPage->nextPage == NULL)
                break;
            else
                AimPage = AimPage->nextPage;
        //存入数组
        for (int count = 0; count < 6; count++)
            AimPage->angles[count] = arr[count];
        //更新计数
        pageCount++;
        //分配新空间
        AimPage->nextPage = (angleInfo *)malloc(sizeof(angleInfo));
        //处理链表末尾
        AimPage = AimPage->nextPage;
        AimPage->nextPage = NULL;
        return *this;
    }
    //存入取出
    anglePage &anglePage::operator>>(double *arr)
    {
        //目标链表
        angleInfo *AimPage = startPage;
        //处理末尾
        if (AimPage->nextPage == NULL || pageCount == 0)
            return *this;
        //存入数组
        for (int count = 0; count < 6; count++)
            arr[count] = AimPage->angles[count];
        //更新计数
        pageCount--;
        //处理链表头
        startPage = AimPage->nextPage;
        free(AimPage);
        return *this;
    }

    //检查链表是否为空
    bool anglePage::isEmpty()
    {
        if (pageCount == 0)
            return true;
        else
            return false;
    }
}