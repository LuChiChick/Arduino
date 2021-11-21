#include "anglePage.hpp"
#include "OLED.hpp"
//屏幕
extern std::cus_OLED_SSD1306 OLED;
//步进电机管理组
extern std::StepperMoter moterGroup[6];
//缓存页
std::anglePage bufferPage;

namespace std
{
    //构造函数
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
        //初始化角度
        for (int count = 0; count < 6; count++)
        {
            latestAngles[count] = moterGroup[count].getAngle();
            //顺手输出下信息——————[调试用]
            OLED.printf(str "Moter%d:%f deg.\n", count, latestAngles[count]);
        }
    }

    //存入链表
    anglePage &anglePage::operator<<(const double *arr)
    {
        //目标链表
        angleInfo *AimPage = startPage;
        //寻找末尾
        while (1)
        {
            if (AimPage->nextPage == NULL)
                break; //如果达到末尾退出循环
            else
                AimPage = AimPage->nextPage;
        }

        //相关信息计算
        double tempAngle; //临时角度
        for (int count = 0; count < 6; count++)
        {
            //计算目标角度差值
            tempAngle = arr[count] - latestAngles[count];
            //处理方向
            if (tempAngle < 0)
            {
                AimPage->dirGroup[count] = ANTICLOCKWISE;
                tempAngle = -tempAngle;
            }
            else
                AimPage->dirGroup[count] = CLOCKWISE;
            //计算目标脉冲数
            AimPage->pulseGroup[count] = tempAngle / 360.0 * moterGroup[count].getPulse_Per_round() * moterGroup[count].getDivider();

            //标志组计算
            if (AimPage->pulseGroup[count] == 0)
                AimPage->tagGroup[count] = true;
            else
                AimPage->tagGroup[count] = false;
            //存入角度组
            AimPage->angleGroup[count] = arr[count];
            //更新最新角度
            latestAngles[count] = arr[count];
        }

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
            arr[count] = AimPage->angleGroup[count];
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

    //返回角度组
    uint64_t *anglePage::getPulseGroup()
    {
        //目标链表
        angleInfo *AimPage = startPage;
        //处理末尾
        if (AimPage->nextPage == NULL || pageCount == 0)
            return NULL;
        else
            return AimPage->pulseGroup;
    }

}