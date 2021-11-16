#include "OLED.hpp"
//宏定义
#define OLED_CMD 0
#define OLED_DATA 1
#define OLED_PAGE_POS_LEN 8
namespace std
{
    //配速，父类函数重载公开定义(照搬)
    void cus_OLED_SSD1306::speedLimit(uint32_t speed_threshold)
    {
        this->speed_threshold = speed_threshold;
    }
    //状态检查，父类函数重载公开定义(照搬)
    bool std::cus_OLED_SSD1306::if_Init_OK()
    {
        return Init_already;
    }
    //重设设备地址
    void cus_OLED_SSD1306::reDirection(uint8_t Aim_addr)
    {
        this->Aim_addr = Aim_addr;
        //定位设备
        focuson(Aim_addr);
        //检查是否唤醒
        for (int count = 0; count < CUS_IIC_WAIT_MAX_TIMES; count++)
        {
            if (OLED_Init())
                break;
            if (count + 1 == CUS_IIC_WAIT_MAX_TIMES)
            {
                Init_already = false;
                return;
            }
        }
        //0正常显示 1反色显示
        ColorTurn(false);

        //0正常显示 1翻转180度显示
        DisplayTurn(false);

        //完成初始化
        Init_already = true;
    }
    //屏幕信息设定
    bool cus_OLED_SSD1306::ScreenSetting(uint16_t ScreenX, uint16_t ScreenY, bool should_buffer_Init)
    {
        //预处理
        if (buffer != NULL)
        {
            free(buffer);
            buffer = NULL;
        }
        //设置基础信息
        SCREEN_X = ScreenX;
        BUFFER_PAGE_MAX = ScreenY / OLED_PAGE_POS_LEN;
        BUFFER_CHAR_COUNT_MAX_PER_PAGE = SCREEN_X / DEF_STD_ASCII_DATA_LENGTH;
        //归位光标
        cursor_chrCount_X = cursor_page = 0;

        //选择性初始化
        if (should_buffer_Init)
        {
            buffer = (uint8_t *)malloc(BUFFER_CHAR_COUNT_MAX_PER_PAGE * BUFFER_PAGE_MAX);
            if (buffer == NULL)
                return false;
        }
        if (!Clear())
            return false;
        if (!printf_at(0, 0, (const uint8_t *)"Screen init OK!!"))
            return false;
        return true;
    }

    //构造函数，父类优先初始化
    cus_OLED_SSD1306::cus_OLED_SSD1306() : cus_IIC_Wire()
    {
        //没有足够信息,啥都不用做,但是要处理buffer
        buffer = NULL;
    }
    //有参构造函数，父类优先初始化
    cus_OLED_SSD1306::cus_OLED_SSD1306(uint16_t SCL_Pin,
                                       uint16_t SDA_Pin,
                                       uint16_t ScreenX,
                                       uint16_t ScreenY,
                                       bool should_buffer_Init)
        : cus_IIC_Wire(SCL_Pin, SDA_Pin)
    {
        //初始化基础信息
        speedLimit(DEF_SSD1306_IIC_SPEED_THRESHOLD);
        //初始化指针为NULL
        buffer = NULL;
        //初始化各类信息为安全值
        SCREEN_X = 0;
        BUFFER_CHAR_COUNT_MAX_PER_PAGE = 0;
        BUFFER_PAGE_MAX = 0;
        //重定向设备地址
        reDirection(DEF_SSD1306_ADDR);
        //初始化屏幕信息
        ScreenSetting(ScreenX, ScreenY, should_buffer_Init);
    }
    //析构函数,释放buffer
    cus_OLED_SSD1306::~cus_OLED_SSD1306()
    {
        if (buffer != NULL)
            free(buffer);
    }

    //初始化
    bool cus_OLED_SSD1306::OLED_Init()
    {
        //我知道这里的写法很蠢，但这样写这些临时数据在有效期结束后可以被分给其他空间

        if (!write(0x00)) //表明输入cmd
            return false;
        if (!write(0xAE)) //--turn off oled panel
            return false;
        if (!write(0x00)) //---set low column address
            return false;
        if (!write(0x10)) //---set high column address
            return false;
        if (!write(0x40)) //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
            return false;
        if (!write(0x81)) //--set contrast control register
            return false;
        if (!write(0xCF)) // Set SEG Output Current Brightness
            return false;
        if (!write(0xA1)) //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
            return false;
        if (!write(0xC8)) //Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
            return false;
        if (!write(0xA6)) //--set normal display
            return false;
        if (!write(0xA8)) //--set multiplex ratio(1 to 64)
            return false;
        if (!write(0x3f)) //--1/64 duty
            return false;
        if (!write(0xD3)) //-set display offset Shift Mapping RAM Counter (0x00~0x3F)
            return false;
        if (!write(0x00)) //-not offset
            return false;
        if (!write(0xd5)) //--set display clock divide ratio/oscillator frequency
            return false;
        if (!write(0x80)) //--set divide ratio, Set Clock as 100 Frames/Sec
            return false;
        if (!write(0xD9)) //--set pre-charge period
            return false;
        if (!write(0xF1)) //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
            return false;
        if (!write(0xDA)) //--set com pins hardware configuration
            return false;
        if (!write(0x12))
            return false;
        if (!write(0xDB)) //--set vcomh
            return false;
        if (!write(0x40)) //Set VCOM Deselect Level
            return false;
        if (!write(0x20)) //-Set Page Addressing Mode (0x00/0x01/0x02)
            return false;
        if (!write(0x02)) //
            return false;
        if (!write(0x8D)) //--set Charge Pump enable/disable
            return false;
        if (!write(0x14)) //--set(0x10) disable
            return false;
        if (!write(0xA4)) // Disable Entire Display On (0xa4/0xa5)
            return false;
        if (!write(0xA6)) // Disable Inverse Display On (0xa6/a7)
            return false;
        if (!write(0xAF)) // display ON
            return false;
        end(false);
        if (!Clear())
            return false;
        return true;
    }
    //设置要编辑的像素点坐标,X为0~127的刷新列,Y为0~8的刷新页
    bool cus_OLED_SSD1306::Set_Pos(uint16_t x_offest, uint8_t page_offest)
    {
        //设置像素点位置命令
        if (write(0x00))
            if (write(0xb0 + page_offest))
                if (write(((x_offest & 0xf0) >> 4) | 0x10))
                    if (write((x_offest & 0x0f)))
                    {
                        end(false);
                        return true;
                    }
        return false;
        /*
        //旧版
        if (OLED_WR_Byte(0xb0 + y, OLED_CMD))
            if (OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD))
                if (OLED_WR_Byte((x & 0x0f), OLED_CMD))
                    return true;
        return false;
        */
    }
    bool cus_OLED_SSD1306::buffer_roll()
    {
        //检查缓冲区信息
        if (this->buffer == NULL)
            return false;
        //重载使用方式
        uint8_t(*buffer)[BUFFER_CHAR_COUNT_MAX_PER_PAGE] = (uint8_t(*)[BUFFER_CHAR_COUNT_MAX_PER_PAGE])this->buffer;
        /*
        //旧版
        //整体上移
        for (int writenPage = 0; writenPage < BUFFER_PAGE_MAX - 1; writenPage++)
            for (int charCount = 0; charCount < BUFFER_CHAR_COUNT_MAX_PER_PAGE; charCount++)
                buffer[writenPage][charCount] = buffer[writenPage + 1][charCount];
        //清除最后一排
        for (int charCount = 0; charCount < BUFFER_CHAR_COUNT_MAX_PER_PAGE; charCount++)
            buffer[BUFFER_PAGE_MAX - 1][charCount] = 0x00;
        //更新屏幕
        for(int writenPage = 0; writenPage < BUFFER_PAGE_MAX; writenPage++ )
            for (int charCount = 0; charCount < BUFFER_CHAR_COUNT_MAX_PER_PAGE; charCount++)
                Putchar_at(charCount*6,writenPage,buffer[writenPage][charCount]);
        */

        //整体上移的同时,比对需要更新的区域并刷新
        //在重复字符较多的情况下比旧版快非常多
        bool changed;                                                               //字符是否发生交换，用来节省代码逻辑
        for (int writtenPage = 0; writtenPage < BUFFER_PAGE_MAX - 1; writtenPage++) //页滚动
        {
            //比对处理
            for (int charFocus = 0; charFocus < BUFFER_CHAR_COUNT_MAX_PER_PAGE; charFocus++) //行滚动
            {
                changed = false;
                //数据不同时
                if (buffer[writtenPage][charFocus] != buffer[writtenPage + 1][charFocus])
                {
                    //变换并标记changed
                    buffer[writtenPage][charFocus] = buffer[writtenPage + 1][charFocus];
                    changed = true;
                }
                //处理换行符

                if (buffer[writtenPage][charFocus] == '\n')
                {
                    //清除冗余数据避免风险
                    for (int count = charFocus + 1; count < BUFFER_CHAR_COUNT_MAX_PER_PAGE; count++)
                        buffer[writtenPage][count] = 0x00;
                    //设置光标位置
                    if (!Set_Pos(charFocus * DEF_STD_ASCII_DATA_LENGTH, writtenPage))
                        return false;
                    if (!write(0x40)) //data命令
                        return false;
                    for (int posCount = 0; posCount < SCREEN_X - DEF_STD_ASCII_DATA_LENGTH * charFocus; posCount++)
                    {
                        if (!write(0x00)) //写入空白
                            return false;
                    }
                    end(false); //结束通信
                    break;      //跳转到下一行
                }
                if (changed)
                {
                    //刷新字符
                    if (!Putchar_at(charFocus * DEF_STD_ASCII_DATA_LENGTH, writtenPage * OLED_PAGE_POS_LEN, buffer[writtenPage][charFocus]))
                        return false;
                }
            }
        }
        //处理最后一行，因为会复写，所以不用清除多余的垃圾数据
        //设置光标位置
        if (!Set_Pos(0, BUFFER_PAGE_MAX - 1))
            return false;
        if (!write(0x40)) //data命令
            return false;
        for (int posCount = 0; posCount < SCREEN_X; posCount++)
        {
            if (!write(0x00)) //写入空白
                return false;
        }
        end(false); //结束通信
        //重置X光标
        cursor_chrCount_X = 0;
        return true;
    }
    //清除缓冲区并重置光标
    void cus_OLED_SSD1306::Clear_buffer()
    {
        //转换使用方式
        uint8_t(*buffer)[BUFFER_CHAR_COUNT_MAX_PER_PAGE] = (uint8_t(*)[BUFFER_CHAR_COUNT_MAX_PER_PAGE])this->buffer;
        cursor_chrCount_X = cursor_page = 0;
        //初始化显存
        for (int lineCount = 0; lineCount < BUFFER_PAGE_MAX; lineCount++)
            for (int charCount = 0; charCount < BUFFER_CHAR_COUNT_MAX_PER_PAGE; charCount++)
                buffer[lineCount][charCount] = 0x00;
    }
    //向SSD1306写入一个字节,分为数据或命令
    bool cus_OLED_SSD1306::OLED_WR_Byte(uint8_t data, bool mode)
    {
        if (mode)
        {
            if (!write(0x40))
                return CUS_IIC_WRITE_FAILED;
        }
        else
        {
            if (!write(0x00))
                return CUS_IIC_WRITE_FAILED;
        }
        if (!write(data))
            return CUS_IIC_WRITE_FAILED;
        end(false); // stop transmitting
        return true;
    }
    //清理屏幕
    bool cus_OLED_SSD1306::Clear()
    {
        //清理缓冲区
        if (this->buffer != NULL)
            Clear_buffer();
        //清理屏幕
        for (uint8_t pageCount = 0; pageCount < BUFFER_PAGE_MAX; pageCount++)
        {
            /*
            //原始版本
            if (OLED_WR_Byte(0xb0 + i, OLED_CMD) == false) //设置页地址(0~7)
                return false;
            if (OLED_WR_Byte(0x00, OLED_CMD) == false) //设置显示位置—列低地址
                return false;
            if (OLED_WR_Byte(0x10, OLED_CMD) == false) //设置显示位置—列高地址
                return false;
            */
            //优化版本
            if (!write(0X00))
                return false;
            if (!write(0xb0 + pageCount))
                return false;
            if (!write(0x00))
                return false;
            if (!write(0x10))
                return false;
            end(false);
            //刷新行数
            if (!write(0x40))
                return false;
            for (uint8_t offest_x = 0; offest_x < SCREEN_X; offest_x++)
                if (!write(0x00))
                    return false;
            end(false);
        } //更新显示
        return true;
    }
    //屏幕反色显示
    bool cus_OLED_SSD1306::ColorTurn(bool YesOrNo)
    {
        if (YesOrNo == true)
            return OLED_WR_Byte(0xA7, OLED_CMD); //反色显示
        else
            return OLED_WR_Byte(0xA6, OLED_CMD); //正常显示
    }
    //屏幕上下颠倒
    bool cus_OLED_SSD1306::DisplayTurn(bool YesOrNo)
    {
        if (YesOrNo == true)
        {
            //反转显示
            if (OLED_WR_Byte(0xC0, OLED_CMD) == false)
                return false;
            return OLED_WR_Byte(0xA0, OLED_CMD);
        }
        else
        {
            //正常显示
            if (OLED_WR_Byte(0xC8, OLED_CMD) == false)
                return false;
            return OLED_WR_Byte(0xA1, OLED_CMD);
        }
    }
    //显示图片x_offest,y_offest显示坐标,sizex,sizey图片长宽,BMP_Arr：要显示的图片数组
    bool cus_OLED_SSD1306::DrawBMP_at(uint16_t x_offest, uint16_t y_offest, uint16_t sizex, uint8_t sizey, const uint8_t BMP_Arr[])
    {
        //转换使用方式
        uint8_t(*ArrToPrint)[sizex] = (uint8_t(*)[sizex])BMP_Arr;
        //计算是否需要偏移像素点分两行打印(即坐标是否能被PagePos整除)
        int pos_offest = y_offest % OLED_PAGE_POS_LEN;
        //确定打印宽度
        int print_Width = (sizex + x_offest >= SCREEN_X) ? SCREEN_X - x_offest : sizex;
        //滚动页面
        for (int writtenPageCount = 0; writtenPageCount < sizey / OLED_PAGE_POS_LEN; writtenPageCount++)
        {
            //越界审查
            if (writtenPageCount + y_offest / OLED_PAGE_POS_LEN >= BUFFER_PAGE_MAX)
                return true;
            //设置坐标
            if (!Set_Pos(x_offest, writtenPageCount + y_offest / OLED_PAGE_POS_LEN))
                return false;
            if (!write(0x40)) //表明写入data
                return false;

            //打印每行的数据
            for (int count = 0; count < print_Width; count++)
            {
                //处理第一行
                if (writtenPageCount == 0)
                {
                    if (!write((ArrToPrint[writtenPageCount][count]) << pos_offest)) //偏移pos_offest单位
                        return false;
                    //这个continue相当重要！！！！！！
                    continue;
                }
                //打印常规行,偏移8-pos_offest单位并承接上一行的数据
                if (!write((ArrToPrint[writtenPageCount][count]) << pos_offest | (ArrToPrint[writtenPageCount - 1][count]) >> (OLED_PAGE_POS_LEN - pos_offest)))
                    return false;
            }
            //结束一行的打印
            end(false);
            //处理最后一行，需要存在偏移，且不能越界
            if (pos_offest > 0 && writtenPageCount == (sizey / OLED_PAGE_POS_LEN) - 1 && writtenPageCount + 1 + y_offest / OLED_PAGE_POS_LEN < BUFFER_PAGE_MAX)
            {
                //设置坐标
                if (!Set_Pos(x_offest, writtenPageCount + 1 + y_offest / OLED_PAGE_POS_LEN))
                    return false;
                if (!write(0x40)) //表明写入data
                    return false;
                //打印每行最后一行数据
                for (int count = 0; count < print_Width; count++)
                {
                    if (!write((ArrToPrint[writtenPageCount][count]) >> (OLED_PAGE_POS_LEN - pos_offest)))
                        return false;
                }
                //结束一行的打印
                end(false);
            }
        }
        return true;
    }
    //从点阵数组显示文字,x,y为坐标
    bool cus_OLED_SSD1306::Putchar_at(uint16_t x_offest, uint16_t y_offest, const uint8_t chr)
    {
        //排查是否越界
        if (y_offest >= OLED_PAGE_POS_LEN * BUFFER_PAGE_MAX)
            return false;
        if (x_offest >= SCREEN_X)
            return false;
        //计算是否需要偏移像素点分两行打印(即坐标是否能被PagePos整除)
        int pos_offest = y_offest % OLED_PAGE_POS_LEN;
        //打印上半部分，若能整除就结束

        if (!Set_Pos(x_offest, y_offest / OLED_PAGE_POS_LEN))
            return false;
        if (!write(0x40)) //表明写入data
            return false;
        for (uint16_t i = 0; i < DEF_STD_ASCII_DATA_LENGTH; i++)
        {
            if (!write((std_ASCII_0806[chr - ' '][i]) << pos_offest)) //偏移pos_offest单位
                return false;
        }
        end(false);

        //判断是否需要打印下半行
        if (pos_offest > 0)
        {
            //处理临界打印
            if ((y_offest / OLED_PAGE_POS_LEN) + 1 == BUFFER_PAGE_MAX)
                return true;
            if (!Set_Pos(x_offest, (y_offest / OLED_PAGE_POS_LEN) + 1))
                return false;
            if (!write(0x40)) //表明写入data
                return false;
            for (uint16_t i = 0; i < DEF_STD_ASCII_DATA_LENGTH; i++)
            {
                if (!write((std_ASCII_0806[chr - ' '][i]) >> (OLED_PAGE_POS_LEN - pos_offest))) //反向偏移
                    return false;
            }
            end(false);
        }
        return true;
    }
    //基于屏幕管理的字符输出
    bool cus_OLED_SSD1306::Putchar(const uint8_t chr)
    {
        //检查缓冲区信息
        if (this->buffer == NULL)
            return false;

        uint8_t(*buffer)[BUFFER_CHAR_COUNT_MAX_PER_PAGE] = (uint8_t(*)[BUFFER_CHAR_COUNT_MAX_PER_PAGE])this->buffer;
        //缓存数据
        buffer[cursor_page][cursor_chrCount_X] = chr;
        //检查换行符
        if (chr == '\n')
        {
            //更新页面cursor_page
            if (cursor_page + 1 == BUFFER_PAGE_MAX)
            {
                if (!buffer_roll()) //滚动显存
                    return false;
            }
            else
                cursor_page++;
            //回退cursor_charCount_X;
            cursor_chrCount_X = 0;
            //结束返回
            return true;
        }

        //打印数据
        if (Putchar_at(cursor_chrCount_X * DEF_STD_ASCII_DATA_LENGTH, cursor_page * OLED_PAGE_POS_LEN, chr))
        {
            //移动光标
            cursor_chrCount_X++;
            //判断是否应该换行
            if (cursor_chrCount_X == BUFFER_CHAR_COUNT_MAX_PER_PAGE)
            {
                //判断是否超出最大页面,并更新cursor_page
                if (cursor_page + 1 == BUFFER_PAGE_MAX)
                {
                    if (!buffer_roll()) //滚动显存
                        return false;
                }
                else
                    cursor_page++;
                //更新cursor_x
                cursor_chrCount_X = 0;
            }
            return true;
        }
        else
            return false;
    }
    //基于显存管理的连续字符输出,避免命名为printf相关，编译器会错误,会陆续支持格式化输出
    bool cus_OLED_SSD1306::printf(const uint8_t *lpFormatString, ...)
    {
        //检查缓冲区信息
        if (this->buffer == NULL)
            return false;
        /*
            由于平台原因，可能无法通过va_list的方式获取参数，而只能手动查找函数被分配的栈堆地址
            技术参考:https://blog.csdn.net/qq_35031421/article/details/109193237
            这里留一个相关版本,和支持va_list的版本，完善情况可能不一致
        */
        /*
        {   //      不支持va_list的版本
            //参数地址指针,初始指向第一个参数
            void *lpArge = (uint8_t *)&lpFormatString;

            //轮询数据
            while (*lpFormatString != '\0')
            {
                if (*lpFormatString == '%')
                {
                    //筛查格式化标签
                    switch (*(lpFormatString + 1))
                    {
                    //8位整数型
                    case '8':
                        if (*(lpFormatString + 2) == 'i')
                        {
                            //寻找参数所在位置并打印数值
                            lpArge = ((uint8_t *)lpArge + sizeof(uint8_t *));
                            if (!PrintInteger(*(uint8_t *)lpArge))
                                return false;
                            //跃迁格式化字符串指针
                            lpFormatString += 3;
                        }
                        //查看是否到达末尾
                        if (*lpFormatString == '\0')
                            return true;
                        //完成
                        break;
                    case 'l':
                        if (*(lpFormatString + 2) == 'l')
                        {
                            if (*(lpFormatString + 3) == 'l')
                            {
                                //uint64_t
                                //寻找参数所在位置并打印数值
                                lpArge = ((uint8_t *)lpArge + sizeof(uint64_t *));
                                if (!PrintInteger(*(uint64_t *)lpArge))
                                    return false;
                                //跃迁格式化字符串指针
                                lpFormatString += 4;
                            }
                            else
                            {
                                //uint32_t
                                //寻找参数所在位置并打印数值
                                lpArge = ((uint8_t *)lpArge + sizeof(uint32_t *));
                                if (!PrintInteger(*(uint32_t *)lpArge))
                                    return false;
                                //跃迁格式化字符串指针
                                lpFormatString += 3;
                            }
                        }
                        else
                        {
                            //uint16_t
                            //寻找参数所在位置并打印数值
                            lpArge = ((uint8_t *)lpArge + sizeof(uint16_t *));
                            if (!PrintInteger(*(uint16_t *)lpArge))
                                return false;
                            //跃迁格式化字符串指针
                            lpFormatString += 2;
                        }
                        //查看是否到达末尾
                        if (*lpFormatString == '\0')
                            return true;
                        //完成
                        break;
                    //不在检测范围内的标签
                    default:
                        if (!Putchar(*(lpFormatString++))) //打印并更新指针
                            return false;
                        if (!Putchar(*(lpFormatString++))) //打印并更新指针
                            return false;
                    }
                }
                if (!Putchar(*(lpFormatString++))) //打印并更新指针
                    return false;
            }
            return true;
        }   
        */
        { //支持va_list的版本

            //初始化参数列表，指向不定参第一个参数
            va_list args;
            va_start(args, lpFormatString);

            //轮询数据
            while (*lpFormatString != '\0')
            {
                if (*lpFormatString == '%')
                {
                    //筛查格式化标签
                    switch (*(lpFormatString + 1))
                    {
                    //数型
                    case 'd':
                        if (!PrintInteger(va_arg(args, int)))
                            return false;
                        //跃迁格式化字符串指针
                        lpFormatString += 2;
                        //查看是否到达末尾
                        if (*lpFormatString == '\0')
                            return true;
                        //完成
                        break;
                    case 'f':
                        if (!PrintDecimal(va_arg(args, double)))
                            return false;
                        //跃迁格式化字符串指针
                        lpFormatString += 2;
                        //查看是否到达末尾
                        if (*lpFormatString == '\0')
                            return true;
                        //完成
                        break;
                    //不在检测范围内的标签
                    default:
                        break;
                    }
                }
                if (!Putchar(*(lpFormatString++))) //打印并更新指针
                    return false;
            }
            return true;
        }
    }
    //指定位置格式化输出,会陆续支持格式化输出，目前可以打印%f、%d
    bool cus_OLED_SSD1306::printf_at(uint16_t x_offest, uint16_t y_offest, const uint8_t *lpFormatString, ...)
    {
        //支持va_list的版本
        //初始化参数列表，指向不定参第一个参数
        va_list args;
        va_start(args, lpFormatString);
        //打印坐标记录
        uint16_t cursorX = x_offest;
        uint16_t cursorY = y_offest;
        //轮询数据
        while (*lpFormatString != '\0')
        {
            //行越界审查
            if (cursorX + DEF_STD_ASCII_DATA_LENGTH >= SCREEN_X)
                return true;
            //检查换行符
            if (*lpFormatString == '\n')
            {
                cursorX = x_offest;
                cursorY += OLED_PAGE_POS_LEN;
                //页越界审查
                if (cursorY >= BUFFER_PAGE_MAX * OLED_PAGE_POS_LEN)
                    return true;
                lpFormatString++;
                continue;
            }
            //筛查格式化标签
            if (*lpFormatString == '%')
            {
                switch (*(lpFormatString + 1))
                {
                //数型
                case 'd':
                    if (!PrintInteger_at(cursorX, cursorY, va_arg(args, int)))
                        return false;
                    //跃迁格式化字符串指针
                    lpFormatString += 2;
                    //查看是否到达末尾
                    if (*lpFormatString == '\0')
                        return true;
                    //完成
                    break;
                case 'f':
                    if (!PrintDecimal_at(cursorX, cursorY, va_arg(args, double)))
                        return false;
                    //跃迁格式化字符串指针
                    lpFormatString += 2;
                    //查看是否到达末尾
                    if (*lpFormatString == '\0')
                        return true;
                    //完成
                    break;
                //不在检测范围内的标签
                default:
                    break;
                }
            }
            if (!Putchar_at(cursorX, cursorY, *(lpFormatString++))) //打印并更新指针
                return false;
            cursorX += DEF_STD_ASCII_DATA_LENGTH;
        }
        return true;
    }
    //递归打印整数
    bool cus_OLED_SSD1306::PrintInteger(int num)
    {
        //处理正负号
        if (num < 0)
        {
            if (!Putchar('-'))
                return false;
            return PrintInteger(-num);
        }

        //处理常规值
        if (num >= 0 && num < 10)
            return Putchar('0' + num);
        else
        {
            if (PrintInteger(num / 10))
                return PrintInteger(num % 10);
            else
                return false;
        }
    }
    //指定位置递归打印整数
    bool cus_OLED_SSD1306::PrintInteger_at(uint16_t &x_offest, uint16_t &y_offest, int num)
    {
        //处理正负号
        if (num < 0)
        {
            if (!Putchar_at(x_offest, y_offest, '-'))
                return false;
            x_offest += DEF_STD_ASCII_DATA_LENGTH;
            return PrintInteger_at(x_offest, y_offest, -num);
        }

        //处理常规值
        if (num >= 0 && num < 10)
        {
            //检查行溢出
            if (x_offest + DEF_STD_ASCII_DATA_LENGTH >= SCREEN_X)
                return true;
            if (!Putchar_at(x_offest, y_offest, '0' + num))
                return false;
            x_offest += DEF_STD_ASCII_DATA_LENGTH;
            return true;
        }
        else
        {
            if (PrintInteger_at(x_offest, y_offest, num / 10))
                return PrintInteger_at(x_offest, y_offest, num % 10);
            else
                return false;
        }
    }
    bool cus_OLED_SSD1306::PrintDecimal(double num)
    {
        //处理符号
        if (num < 0)
        {
            if (!Putchar('-'))
                return false;
            num = -num;
        }
        //处理原来的整数部分
        if (!PrintInteger((int)(num * 1.0)))
            return false;
        num = num - (int)num;
        if (!Putchar('.'))
            return false;
        //扩大倍数转将小数部分换成整数来解决问题
        for (int count = 0; count < DEF_DECIMAL_HOLD_MAX; count++)
            num *= 10;
        //忽略多余的0
        while ((int)num % 10 == 0 && (int)num != 0)
            num = (int)num / 10;
        return PrintInteger((int)num);
    }
    //指定位置打印浮点数
    bool cus_OLED_SSD1306::PrintDecimal_at(uint16_t &x_offest, uint16_t &y_offest, double num)
    {
        //处理符号
        if (num < 0)
        {
            Putchar_at(x_offest, y_offest, '-');
            x_offest += DEF_STD_ASCII_DATA_LENGTH;
            num = -num;
        }
        //检查行溢出
        if (x_offest + DEF_STD_ASCII_DATA_LENGTH >= SCREEN_X)
            return true;
        //处理原来的整数部分
        if (!PrintInteger_at(x_offest, y_offest, (int)(num * 1.0)))
            return false;
        //检查行溢出
        if (x_offest + DEF_STD_ASCII_DATA_LENGTH >= SCREEN_X)
            return true;
        //整理数字
        num = num - (int)num;
        if (!Putchar_at(x_offest, y_offest, '.'))
            return false;
        //检查行溢出
        if (x_offest + DEF_STD_ASCII_DATA_LENGTH >= SCREEN_X)
            return true;
        x_offest += DEF_STD_ASCII_DATA_LENGTH;
        //扩大倍数转将小数部分换成整数来解决问题
        for (int count = 0; count < DEF_DECIMAL_HOLD_MAX; count++)
            num *= 10;
        //忽略多余的0
        while ((int)num % 10 == 0 && (int)num != 0)
            num = (int)num / 10;
        return PrintInteger_at(x_offest, y_offest, (int)num);
    }
}