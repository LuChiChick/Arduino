#include "defines.hpp"
#include "StepperMotor.hpp"
#include "OLED.hpp"
#include "Command.hpp"
#include "anglePage.hpp"

//步进电机管理组
extern std::StepperMoter moterGroup[6];
//屏幕
extern std::cus_OLED_SSD1306 OLED;
//缓存页
extern std::anglePage bufferPage;
//命令处理类
extern std::CommandHandler Console;
/**
 * 辗转相除法求最大公约数
 * @param a 第一个数
 * @param b 第二个数
 * @return 最大公约数
 */
uint64_t getGCD(uint64_t a, uint64_t b)
{
    return (a % b == 0) ? b : getGCD(b, a % b);
}

/**
 * 辗转相除法求最小公倍数
 * @param a 第一个数
 * @param b 第二个数
 * @return 最小公倍数
 */
uint64_t getLCM(uint64_t a, uint64_t b)
{
    return a / getGCD(a, b) * b;
}

void setup()
{
    //引脚信息记录
    {
        //步进电机0
        moterGroup[0] = std::StepperMoter(MOTER0_DIR,
                                          MOTER0_PUL,
                                          MOTER0_ENA,
                                          MOTER0_ROUND_PUL,
                                          MOTER0_ENA_TYPE,
                                          MOTER0_CLOCKWISE_TYPE,
                                          MOTER0_DIVIDER);
        //步进电机1
        moterGroup[1] = std::StepperMoter(MOTER1_DIR,
                                          MOTER1_PUL,
                                          MOTER1_ENA,
                                          MOTER1_ROUND_PUL,
                                          MOTER1_ENA_TYPE,
                                          MOTER1_CLOCKWISE_TYPE,
                                          MOTER1_DIVIDER);
        //步进电机2
        moterGroup[2] = std::StepperMoter(MOTER2_DIR,
                                          MOTER2_PUL,
                                          MOTER2_ENA,
                                          MOTER2_ROUND_PUL,
                                          MOTER2_ENA_TYPE,
                                          MOTER2_CLOCKWISE_TYPE,
                                          MOTER2_DIVIDER);
        //步进电机3
        moterGroup[3] = std::StepperMoter(MOTER3_DIR,
                                          MOTER3_PUL,
                                          MOTER3_ENA,
                                          MOTER3_ROUND_PUL,
                                          MOTER3_ENA_TYPE,
                                          MOTER3_CLOCKWISE_TYPE,
                                          MOTER3_DIVIDER);
        //步进电机4
        moterGroup[4] = std::StepperMoter(MOTER4_DIR,
                                          MOTER4_PUL,
                                          MOTER4_ENA,
                                          MOTER4_ROUND_PUL,
                                          MOTER4_ENA_TYPE,
                                          MOTER4_CLOCKWISE_TYPE,
                                          MOTER4_DIVIDER);
        //步进电机5
        moterGroup[5] = std::StepperMoter(MOTER5_DIR,
                                          MOTER5_PUL,
                                          MOTER5_ENA,
                                          MOTER5_ROUND_PUL,
                                          MOTER5_ENA_TYPE,
                                          MOTER5_CLOCKWISE_TYPE,
                                          MOTER5_DIVIDER);
    }
    //减速比初期设置
    moterGroup[0].setThreshold(80);
    moterGroup[1].setThreshold(400);
    moterGroup[2].setThreshold(80);
    moterGroup[3].setThreshold(80);
    moterGroup[4].setThreshold(80);
    moterGroup[5].setThreshold(80);
    //开启串口
    Serial.begin(115200);
}

void loop()
{
    //屏幕相关设置
    OLED.speedLimit(0);
    OLED.Clear();
    OLED.printf(str "System Online\n");
    OLED.printf(str "Waitting for Cmd....\n");
    while (1)
    {
        switch (Console.nextCommand())
        {
        case CLEAR_SCREEN: //清理屏幕
        {
            OLED.Clear();
            OLED.printf(str "Waitting for Cmd....\n");
            break;
        }
        case MOTER_INFO: //显示电机信息
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
                MoterID = 0xFF;
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                OLED.printf(str "-<Moter %d Info>-\n", MoterID);
                OLED.printf(str "angle:%f\n", moterGroup[MoterID].getAngle());
                OLED.printf(str "divider:%f\n", moterGroup[MoterID].getDivider());
                OLED.printf(str "threshold:%d\n", moterGroup[MoterID].getThreshold());
                break;
            }
            default:
                OLED.printf(str "Unknown MoterID\n");
            }

            break;
        }
        case MOTERS_INFO: //显示所有电机信息
        {
            OLED.printf(str "----------\n");
            //循环打印电机信息
            for (int count = 0; count < 6; count++)
                OLED.printf(str "Moter%d:%f deg.\n", count, moterGroup[count].getAngle());
            break;
        }
        case MOTER_PULSE: //电机脉冲
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
                MoterID = 0xFF;
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                int pulseCount;
                //异常处理
                if (Console.nextInt(&pulseCount) != NEXT_INT_OK)
                {
                    OLED.printf(str "Error Input\n");
                    break;
                }
                OLED.printf(str "+M%d P%d ...", MoterID, pulseCount);
                if (pulseCount > 0)
                    moterGroup[MoterID].pulse(pulseCount);
                else
                {
                    moterGroup[MoterID].setDirection(ANTICLOCKWISE);
                    moterGroup[MoterID].pulse(-pulseCount);
                }
                OLED.printf(str "Done!\n");
                moterGroup[MoterID].setDirection(CLOCKWISE);
                break;
            }
            default:
                OLED.printf(str "Unknown MoterID\n");
            }
            break;
        }
        case MOTER_TURN_RUNDS: //电机转圈
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
            {
                OLED.printf(str "Error Input\n");
                break;
            }
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                int turnCount;
                //异常处理
                if (Console.nextInt(&turnCount) != NEXT_INT_OK)
                {
                    OLED.printf(str "Error Input\n");
                    break;
                }
                OLED.printf(str "+M%d T%d ...", MoterID, turnCount);
                if (turnCount > 0)
                    moterGroup[MoterID].turnRounds(turnCount);
                else
                {
                    moterGroup[MoterID].setDirection(ANTICLOCKWISE);
                    moterGroup[MoterID].turnRounds(-turnCount);
                }
                OLED.printf(str "Done!\n");
                moterGroup[MoterID].setDirection(CLOCKWISE);
                break;
            }

            default:
                OLED.printf(str "Unknown MoterID\n");
            }
            break;
        }
        case SET_MOTER_THRESHOLD: //设置电机最小阈值
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
            {
                OLED.printf(str "Error Input\n");
                break;
            }
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                int threshold;
                //异常处理
                if (Console.nextInt(&threshold) != NEXT_INT_OK)
                {
                    OLED.printf(str "Error Input\n");
                    break;
                }
                moterGroup[MoterID].setThreshold(threshold);
                OLED.printf(str "+SM%d T%d\n", MoterID, threshold);

                break;
            }
            default:
                OLED.printf(str "Unknown MoterID\n");
            }
            break;
        }
        case ENABLE_MOTER: //启用电机
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
            {
                OLED.printf(str "Error Input\n");
                break;
            }
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                moterGroup[MoterID].enable();
                OLED.printf(str "+M%d Ena\n", MoterID);
                break;
            }
            default:
                OLED.printf(str "Unknown MoterID\n");
            }
            break;
        }
        case ENABLE_ALL_MOTER: //启用所有电机
        {
            for (int count = 0; count < 6; count++)
                moterGroup[count].enable();
            OLED.printf(str "+All Moter Ena.\n");
            break;
        }
        case DISABLE_MOTER: //禁用电机
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
            {
                OLED.printf(str "Error Input\n");
                break;
            }
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                moterGroup[MoterID].disable();
                OLED.printf(str "+M%d D_Ena\n", MoterID);
                break;
            }
            default:
                OLED.printf(str "Unknown MoterID\n");
            }
            break;
        }
        case DISABLE_ALL_MOTER: //禁用所有电机
        {
            for (int count = 0; count < 6; count++)
                moterGroup[count].disable();
            OLED.printf(str "+All Moter D_Ena.\n");
            break;
        }
        case WRITE_PAGE: //页写入
        {
            //写入缓存
            bufferPage << Console.getBuffer();
            Serial.println("ok");
            break;
        }
        case RUN_PAGE: //执行指定数目页缓存
        {
            int pageCount;
            //异常处理
            if (Console.nextInt(&pageCount) != NEXT_INT_OK)
            {
                OLED.printf(str "Error Input\n");
                break;
            }
            OLED.printf(str "+R%dP\n", pageCount);
            //脉冲集合
            uint64_t pulseTargetGroup[6]; //脉冲目标组

            bool tagGroup[6] = {0};   //标志位
            double tempAngle, arr[6]; //角度相关数值
            uint64_t minPulseCount;   //最小脉冲计数
            //循环处理所有链表
            for (int count = 0; count < pageCount || !bufferPage.isEmpty(); count++)
            {
                //取出数据
                bufferPage >> arr;
                //循环处理信息
                for (int count = 0; count < 6; count++)
                {
                    //计算目标角度差值
                    tempAngle = arr[count] - moterGroup[count].getAngle();
                    //处理方向
                    if (tempAngle < 0)
                    {
                        moterGroup[count].setDirection(ANTICLOCKWISE);
                        tempAngle = -tempAngle;
                    }
                    else
                        moterGroup[count].setDirection(CLOCKWISE);
                    //计算目标脉冲数
                    pulseTargetGroup[count] = tempAngle / 360.0 * moterGroup[count].getPulse_Per_round() * moterGroup[count].getDivider();
                    //更新角度
                    moterGroup[count].setAngle(arr[count]);
                }
                //清理标志位
                for (int count = 0; count < 6; count++)
                    tagGroup[count] = false;

                for (int count = 0; count < 6; count++)
                {
                    minPulseCount = 0;
                    //初始化值
                    for (int count = 0; count < 6; count++)
                    {
                        if (tagGroup[count] != true)
                        {
                            //排查不需要移动的值
                            if (pulseTargetGroup[count] == 0)
                            {
                                tagGroup[count] = true;
                                continue;
                            }
                            //填充一个拟定的最小值
                            minPulseCount = pulseTargetGroup[count];
                            break;
                        }
                    }
                    //加速
                    if (minPulseCount == 0)
                        continue;
                    //找最小值
                    for (int count = 0; count < 6; count++)
                        if (tagGroup[count] != true)
                            if (pulseTargetGroup[count] < minPulseCount)
                                minPulseCount = pulseTargetGroup[count];
                    //全部脉冲最小次数
                    for (uint64_t outterCount = 0; outterCount < minPulseCount; outterCount++)
                        for (int innerCount = 0; innerCount < 6; innerCount++)
                            if (tagGroup[innerCount] != true)
                                moterGroup[innerCount].justPulse();
                    //全部减去最小脉冲数
                    for (int count = 0; count < 6; count++)
                    {
                        //标记
                        if ((pulseTargetGroup[count] -= minPulseCount) == 0)
                            tagGroup[count] = true;
                    }
                }
            }

            OLED.printf(str "Done.\n");
            Serial.println("ok");
            break;
            Serial.println("ok");
            break;
        }
        case RUN_ALL_PAGE: //执行所有的页换岑
        {
            OLED.printf(str "+RAP....");
            //脉冲集合
            uint64_t pulseTargetGroup[6]; //脉冲目标组

            bool tagGroup[6] = {0};   //标志位
            double tempAngle, arr[6]; //角度相关数值
            uint64_t minPulseCount;   //最小脉冲计数
            //循环处理所有链表
            while (!bufferPage.isEmpty())
            {
                //取出数据
                bufferPage >> arr;
                //循环处理信息
                for (int count = 0; count < 6; count++)
                {
                    //计算目标角度差值
                    tempAngle = arr[count] - moterGroup[count].getAngle();
                    //处理方向
                    if (tempAngle < 0)
                    {
                        moterGroup[count].setDirection(ANTICLOCKWISE);
                        tempAngle = -tempAngle;
                    }
                    else
                        moterGroup[count].setDirection(CLOCKWISE);
                    //计算目标脉冲数
                    pulseTargetGroup[count] = tempAngle / 360.0 * moterGroup[count].getPulse_Per_round() * moterGroup[count].getDivider();
                    //更新角度
                    moterGroup[count].setAngle(arr[count]);
                }

                //清理标志位
                for (int count = 0; count < 6; count++)
                    tagGroup[count] = false;

                for (int count = 0; count < 6; count++)
                {
                    minPulseCount = 0;
                    //初始化值
                    for (int count = 0; count < 6; count++)
                    {
                        if (tagGroup[count] != true)
                        {
                            //排查不需要移动的值
                            if (pulseTargetGroup[count] == 0)
                            {
                                tagGroup[count] = true;
                                continue;
                            }
                            //填充一个拟定的最小值
                            minPulseCount = pulseTargetGroup[count];
                            break;
                        }
                    }
                    //加速
                    if (minPulseCount == 0)
                        continue;
                    //找最小值
                    for (int count = 0; count < 6; count++)
                        if (tagGroup[count] != true)
                            if (pulseTargetGroup[count] < minPulseCount)
                                minPulseCount = pulseTargetGroup[count];
                    //全部脉冲最小次数
                    for (uint64_t outterCount = 0; outterCount < minPulseCount; outterCount++)
                        for (int innerCount = 0; innerCount < 6; innerCount++)
                            if (tagGroup[innerCount] != true)
                                moterGroup[innerCount].justPulse();
                    //全部减去最小脉冲数
                    for (int count = 0; count < 6; count++)
                    {
                        //标记
                        if ((pulseTargetGroup[count] -= minPulseCount) == 0)
                            tagGroup[count] = true;
                    }
                }
            }

            OLED.printf(str "Done.\n");
            Serial.println("ok");
            break;
        }
        case MOTER_GO_TO_ANGLE: //电机转到指定角度
        {
            int MoterID;
            //异常处理
            if (Console.nextInt(&MoterID) != NEXT_INT_OK)
            {
                OLED.printf(str "Error Input\n");
                break;
            }
            //筛选电机ID
            switch (MoterID)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            {
                double angle;
                //异常处理
                if (Console.nextDouble(&angle) != NEXT_DOUBLE_OK)
                {
                    OLED.printf(str "Error Input\n");
                    break;
                }
                OLED.printf(str "+M%d G%f ...", MoterID, angle);
                moterGroup[MoterID].goTo(angle);
                OLED.printf(str "Done!\n");
                break;
            }

            default:
                OLED.printf(str "Unknown MoterID\n");
            }
            break;
        }
        case MOTERS_GO_TO_ANGLE: //所有电机转到指定角度组
        {
            //脉冲集合
            uint64_t pulseTargetGroup[6]; //脉冲目标组

            bool tagGroup[6] = {0}; //标志位
            OLED.printf(str "MSG....");

            double tempAngle, doubleInput;

            //循环处理信息
            for (int count = 0; count < 6; count++)
            {
                //获取下一次输入
                if (Console.nextDouble(&doubleInput) != NEXT_DOUBLE_OK)
                {
                    OLED.printf(str "next double wrong!\n");
                    break;
                }
                //计算目标角度差值
                tempAngle = doubleInput - moterGroup[count].getAngle();
                //处理方向
                if (tempAngle < 0)
                {
                    moterGroup[count].setDirection(ANTICLOCKWISE);
                    tempAngle = -tempAngle;
                }
                else
                    moterGroup[count].setDirection(CLOCKWISE);
                //计算目标脉冲数
                pulseTargetGroup[count] = tempAngle / 360.0 * moterGroup[count].getPulse_Per_round() * moterGroup[count].getDivider();
                //更新角度
                moterGroup[count].setAngle(doubleInput);
            }
            // uint64_t leastCommonMultiple; //最小公倍数脉冲计算

            // for (int count = 1; count < 6; count++)
            // {
            //     if (pulseTargetGroup[count - 1] == 0 || pulseTargetGroup[count] == 0)
            //         continue;
            //     else(leastCommonMultiple=getLCM(leastCommonMultiple,pulseTargetGroup[count]));
            // }

            uint64_t minPulseCount;

            for (int count = 0; count < 6; count++)
            {
                minPulseCount = 0;
                //初始化值
                for (int count = 0; count < 6; count++)
                {
                    if (tagGroup[count] != true)
                    {
                        //排查不需要移动的值
                        if (pulseTargetGroup[count] == 0)
                        {
                            tagGroup[count] = true;
                            continue;
                        }
                        //填充一个拟定的最小值
                        minPulseCount = pulseTargetGroup[count];
                        break;
                    }
                }
                //加速
                if (minPulseCount == 0)
                    continue;
                //找最小值
                for (int count = 0; count < 6; count++)
                    if (tagGroup[count] != true)
                        if (pulseTargetGroup[count] < minPulseCount)
                            minPulseCount = pulseTargetGroup[count];
                //全部脉冲最小次数
                for (uint64_t outterCount = 0; outterCount < minPulseCount; outterCount++)
                    for (int innerCount = 0; innerCount < 6; innerCount++)
                        if (tagGroup[innerCount] != true)
                            moterGroup[innerCount].justPulse();
                //全部减去最小脉冲数
                for (int count = 0; count < 6; count++)
                {
                    //标记
                    if ((pulseTargetGroup[count] -= minPulseCount) == 0)
                        tagGroup[count] = true;
                }
            }

            OLED.printf(str "Done.\n");
            break;
        }
        case UNKNOWN_INPUT: //未知输入处理
        default:
            OLED.printf(str "Unknown Cmd\n");
        }
    }
}
