#ifndef __OLED_SSD1306__
#define __OLED_SSD1306__
//相关库包含
#include "Custom_i2c.hpp"
//避免警告的宏定义
#define str (const uint8_t *)
extern "C"
{
//变参函数必须头文件
#include <stdarg.h>
}
//宏定义
#define DEF_SSD1306_ADDR 0X3C
//默认延时
#define DEF_SSD1306_IIC_SPEED_THRESHOLD 5
//默认小数点精度
#define DEF_DECIMAL_HOLD_MAX 5
//默认字符集数据长度
#define DEF_STD_ASCII_DATA_LENGTH 6
namespace std
{
    /*
        cus_IIC_Wire 派生类,用于驱动SSD1306-OLED
        最后一个参数决定是否开辟一个显存区域以支持控制台输出模式(非常好用)
        若不开启，则只能使用_at结尾的输出函数，通过坐标的方式驱动屏幕
        (**重要**)如果需要开辟显存区域
        一定要确保启动文件中有足够的heap空间,这是因为屏幕像素管理是基于动态内存分配的
        每个ASCII字符占点阵为6*8，即占用1页(8像素)高度，6像素宽度
        对于128*64的屏幕来说，共有8页每页128像素列，最多容纳168个字符
        需要分配168字节的可用heap空间，当然也可以裁剪屏幕使用更小的heap
    */
    class cus_OLED_SSD1306 : protected cus_IIC_Wire
    {
    protected:
        //类内常量定义
        uint8_t BUFFER_PAGE_MAX;
        uint8_t BUFFER_CHAR_COUNT_MAX_PER_PAGE;
        uint8_t SCREEN_X;
        //光标信息,用于管理点阵字符
        uint8_t cursor_chrCount_X;
        uint8_t cursor_page;
        //显存区域指针,需要妥善管理
        uint8_t *buffer;
        //滚动缓冲区数据，删除最顶部的数据
        bool buffer_roll();
        //清除缓冲区
        void Clear_buffer();
        //向SSD1306写入一个字节,分为数据或命令
        bool OLED_WR_Byte(uint8_t data, bool mode);
        //初始化
        bool OLED_Init();
        //设置要编辑的像素点坐标
        bool Set_Pos(uint16_t x_offest, uint8_t page_offest);
        //递归打印整数
        bool PrintInteger(int num);
        //递归打印浮点数
        bool PrintDecimal(double num);
        //指定位置递归打印整数
        bool PrintInteger_at(uint16_t &x_offest, uint16_t &y_offest, int num);
        //指定位置打印浮点数
        bool PrintDecimal_at(uint16_t &x_offest, uint16_t &y_offest, double num);

    public:
        //无参构造
        cus_OLED_SSD1306();
        //有参构造
        cus_OLED_SSD1306(uint16_t SCL_Pin,
                         uint16_t SDA_Pin,
                         uint16_t ScreenX,
                         uint16_t ScreenY,
                         bool should_buffer_Init);
        //析构函数
        ~cus_OLED_SSD1306();
        //==========================重新公开父类函数======================
        //指定速度阈值
        void speedLimit(uint32_t speed_threshold);
        //状态检查
        bool if_Init_OK();
        //===============================================================
        //重定向设备地址
        void reDirection(uint8_t Aim_addr);
        //屏幕信息设定
        bool ScreenSetting(uint16_t ScreenX, uint16_t ScreenY, bool should_buffer_Init);
        //清理屏幕
        bool Clear();
        //是否反色显示
        bool ColorTurn(bool YesOrNo);
        //屏幕上下颠倒
        bool DisplayTurn(bool YesOrNo);
        //指定位置输出图片,x_offest,y_offest显示坐标;sizex,sizey图片长宽;BMP_Arr：要显示的图片数组
        bool DrawBMP_at(uint16_t x_offest, uint16_t y_offest, uint16_t sizex, uint8_t sizey, const uint8_t BMP_Arr[]);
        //从点阵数组显示文字,x_offest,page_offest为坐标,不在显存管理内
        bool Putchar_at(uint16_t x_offest, uint16_t y_offest, const uint8_t chr);
        //基于显存管理的字符输出
        bool Putchar(const uint8_t chr);
        //基于显存管理的连续字符输出,会陆续支持格式化输出目前可以打印%f、%d
        bool printf(const uint8_t *lpFormatString, ...);
        //指定位置格式化输出,会陆续支持格式化输出，目前可以打印%f、%d
        bool printf_at(uint16_t x_offest, uint16_t y_offest, const uint8_t *lpFormatString, ...);
    };

    //ASCII字符集6*8点阵
    /************************************6*8的点阵************************************/
    const uint8_t std_ASCII_0806[][6] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // sp
        {0x00, 0x00, 0x00, 0x2f, 0x00, 0x00}, // !
        {0x00, 0x00, 0x07, 0x00, 0x07, 0x00}, // "
        {0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14}, // #
        {0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12}, // $
        {0x00, 0x62, 0x64, 0x08, 0x13, 0x23}, // %
        {0x00, 0x36, 0x49, 0x55, 0x22, 0x50}, // &
        {0x00, 0x00, 0x05, 0x03, 0x00, 0x00}, // '
        {0x00, 0x00, 0x1c, 0x22, 0x41, 0x00}, // (
        {0x00, 0x00, 0x41, 0x22, 0x1c, 0x00}, // )
        {0x00, 0x14, 0x08, 0x3E, 0x08, 0x14}, // *
        {0x00, 0x08, 0x08, 0x3E, 0x08, 0x08}, // +
        {0x00, 0x00, 0x00, 0xA0, 0x60, 0x00}, // ,
        {0x00, 0x08, 0x08, 0x08, 0x08, 0x08}, // -
        {0x00, 0x00, 0x60, 0x60, 0x00, 0x00}, // .
        {0x00, 0x20, 0x10, 0x08, 0x04, 0x02}, // /
        {0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
        {0x00, 0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
        {0x00, 0x42, 0x61, 0x51, 0x49, 0x46}, // 2
        {0x00, 0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
        {0x00, 0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
        {0x00, 0x27, 0x45, 0x45, 0x45, 0x39}, // 5
        {0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
        {0x00, 0x01, 0x71, 0x09, 0x05, 0x03}, // 7
        {0x00, 0x36, 0x49, 0x49, 0x49, 0x36}, // 8
        {0x00, 0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
        {0x00, 0x00, 0x36, 0x36, 0x00, 0x00}, // :
        {0x00, 0x00, 0x56, 0x36, 0x00, 0x00}, // ;
        {0x00, 0x08, 0x14, 0x22, 0x41, 0x00}, // <
        {0x00, 0x14, 0x14, 0x14, 0x14, 0x14}, // =
        {0x00, 0x00, 0x41, 0x22, 0x14, 0x08}, // >
        {0x00, 0x02, 0x01, 0x51, 0x09, 0x06}, // ?
        {0x00, 0x32, 0x49, 0x59, 0x51, 0x3E}, // @
        {0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C}, // A
        {0x00, 0x7F, 0x49, 0x49, 0x49, 0x36}, // B
        {0x00, 0x3E, 0x41, 0x41, 0x41, 0x22}, // C
        {0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
        {0x00, 0x7F, 0x49, 0x49, 0x49, 0x41}, // E
        {0x00, 0x7F, 0x09, 0x09, 0x09, 0x01}, // F
        {0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
        {0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
        {0x00, 0x00, 0x41, 0x7F, 0x41, 0x00}, // I
        {0x00, 0x20, 0x40, 0x41, 0x3F, 0x01}, // J
        {0x00, 0x7F, 0x08, 0x14, 0x22, 0x41}, // K
        {0x00, 0x7F, 0x40, 0x40, 0x40, 0x40}, // L
        {0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
        {0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
        {0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
        {0x00, 0x7F, 0x09, 0x09, 0x09, 0x06}, // P
        {0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
        {0x00, 0x7F, 0x09, 0x19, 0x29, 0x46}, // R
        {0x00, 0x46, 0x49, 0x49, 0x49, 0x31}, // S
        {0x00, 0x01, 0x01, 0x7F, 0x01, 0x01}, // T
        {0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
        {0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
        {0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
        {0x00, 0x63, 0x14, 0x08, 0x14, 0x63}, // X
        {0x00, 0x07, 0x08, 0x70, 0x08, 0x07}, // Y
        {0x00, 0x61, 0x51, 0x49, 0x45, 0x43}, // Z
        {0x00, 0x00, 0x7F, 0x41, 0x41, 0x00}, // [
        {0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55}, // 55
        {0x00, 0x00, 0x41, 0x41, 0x7F, 0x00}, // ]
        {0x00, 0x04, 0x02, 0x01, 0x02, 0x04}, // ^
        {0x00, 0x40, 0x40, 0x40, 0x40, 0x40}, // _
        {0x00, 0x00, 0x01, 0x02, 0x04, 0x00}, // '
        {0x00, 0x20, 0x54, 0x54, 0x54, 0x78}, // a
        {0x00, 0x7F, 0x48, 0x44, 0x44, 0x38}, // b
        {0x00, 0x38, 0x44, 0x44, 0x44, 0x20}, // c
        {0x00, 0x38, 0x44, 0x44, 0x48, 0x7F}, // d
        {0x00, 0x38, 0x54, 0x54, 0x54, 0x18}, // e
        {0x00, 0x08, 0x7E, 0x09, 0x01, 0x02}, // f
        {0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C}, // g
        {0x00, 0x7F, 0x08, 0x04, 0x04, 0x78}, // h
        {0x00, 0x00, 0x44, 0x7D, 0x40, 0x00}, // i
        {0x00, 0x40, 0x80, 0x84, 0x7D, 0x00}, // j
        {0x00, 0x7F, 0x10, 0x28, 0x44, 0x00}, // k
        {0x00, 0x00, 0x41, 0x7F, 0x40, 0x00}, // l
        {0x00, 0x7C, 0x04, 0x18, 0x04, 0x78}, // m
        {0x00, 0x7C, 0x08, 0x04, 0x04, 0x78}, // n
        {0x00, 0x38, 0x44, 0x44, 0x44, 0x38}, // o
        {0x00, 0xFC, 0x24, 0x24, 0x24, 0x18}, // p
        {0x00, 0x18, 0x24, 0x24, 0x18, 0xFC}, // q
        {0x00, 0x7C, 0x08, 0x04, 0x04, 0x08}, // r
        {0x00, 0x48, 0x54, 0x54, 0x54, 0x20}, // s
        {0x00, 0x04, 0x3F, 0x44, 0x44, 0x20}, // t
        {0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
        {0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
        {0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
        {0x00, 0x44, 0x28, 0x10, 0x28, 0x44}, // x
        {0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C}, // y
        {0x00, 0x44, 0x64, 0x54, 0x4C, 0x44}, // z
        {0x14, 0x14, 0x14, 0x14, 0x14, 0x14}, // horiz lines
    };
    //整活图像区，放心，基于编译器机制，在某个元素未被使用时是不会被编译的
    //也就是，图库尽管扩充，不会占用多余的宝贵flash空间

    //8x8
    const uint8_t LaughingFace[8] = {0x04, 0x22, 0x44, 0x40, 0x40, 0x44, 0x22, 0x04};
    //100x100
    const unsigned char EmmmCat[1300] = {
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X7F, 0X3F, 0XBF, 0X3F, 0X3F, 0XBF, //
        0X3F, 0XBF, 0X3F, 0X3F, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X3F, //
        0X9F, 0XCF, 0XAF, 0XE7, 0XB1, 0XFC, 0XBA, 0X6E, 0X3D, 0X16, 0X3E, 0XF6, 0XDC, 0X79, 0XE1, 0XA7, //
        0X5F, 0X9F, 0X3F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X3F, 0X3F, 0X9F, 0XDF, 0XAF, 0XE7, 0XD3, //
        0XF9, 0X5D, 0XF5, 0X3D, 0X6D, 0XF9, 0XB3, 0XEB, 0X47, 0X8F, 0X23, 0X01, 0X5C, 0X20, 0X4F, 0X30, //
        0X0E, 0X21, 0X0E, 0X11, 0X4E, 0X90, 0X6F, 0X90, 0X2A, 0X54, 0X21, 0X83, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X1F, 0XC7, //
        0XB1, 0XFC, 0XAE, 0XFF, 0X2A, 0X1F, 0X0D, 0X06, 0X03, 0X01, 0X80, 0X00, 0X00, 0X10, 0X00, 0X80, //
        0X01, 0X43, 0X0F, 0X05, 0X0F, 0X17, 0X1D, 0X1F, 0X0A, 0X1F, 0X1E, 0X0B, 0X1E, 0X17, 0X1D, 0X1E, //
        0X0B, 0X0F, 0X82, 0X01, 0X01, 0X88, 0X00, 0X40, 0X00, 0X21, 0X07, 0X1F, 0XFD, 0X77, 0XDC, 0XF1, //
        0X42, 0X96, 0X3F, 0XFF, 0XFE, 0XE1, 0X00, 0X15, 0XAA, 0X51, 0XAE, 0X11, 0X00, 0XC4, 0XFE, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0X47, 0X90, 0X7E, 0XEB, 0XBE, 0XFF, 0X03, 0X00, 0X08, 0X80, 0X00, 0X02, 0X20, 0X00, 0X10, 0X00, //
        0X04, 0X00, 0X80, 0X00, 0X08, 0XC0, 0XA0, 0XE0, 0XBA, 0XFC, 0XAC, 0X7C, 0X35, 0X1C, 0X38, 0X00, //
        0X01, 0XC0, 0XF0, 0X60, 0XE1, 0X00, 0X10, 0X00, 0X00, 0X00, 0X10, 0X00, 0X00, 0X08, 0X00, 0X00, //
        0X40, 0X03, 0X0F, 0X75, 0XFF, 0XEE, 0XBA, 0XE0, 0X07, 0X3F, 0XFF, 0XFE, 0XFE, 0X3E, 0X1E, 0XDE, //
        0X1F, 0X9F, 0X1F, 0X3F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XC0, 0X1A, 0X57, 0XFD, 0XAF, 0XFF, 0XD0, 0X00, 0X00, 0X08, 0X00, 0X80, //
        0X04, 0X00, 0X40, 0X02, 0X20, 0X00, 0X10, 0X00, 0X00, 0X0A, 0X0F, 0X1D, 0X37, 0X0D, 0X01, 0XC0, //
        0XE0, 0X40, 0XC4, 0X20, 0X00, 0X00, 0X11, 0X01, 0X00, 0XC0, 0XE0, 0XB4, 0XE0, 0XF1, 0XA0, 0XF0, //
        0X60, 0XC1, 0XC0, 0X80, 0X08, 0X00, 0X00, 0XC9, 0XFF, 0XB6, 0XFF, 0X6D, 0X92, 0X00, 0XFF, 0XFF, //
        0XFE, 0XF8, 0XF1, 0XF2, 0XF1, 0XF2, 0XF1, 0XFC, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0XF8, 0XC5, 0X17, 0X5E, 0XFB, 0XAF, //
        0XFE, 0XF0, 0X40, 0X00, 0X08, 0X00, 0X00, 0X88, 0X00, 0X04, 0X00, 0X80, 0X02, 0X40, 0X01, 0X20, //
        0X00, 0X10, 0X00, 0X00, 0X03, 0X41, 0X09, 0X00, 0X00, 0X42, 0X00, 0X50, 0XBD, 0XFF, 0XD6, 0X7F, //
        0X01, 0X04, 0X00, 0X01, 0X83, 0X07, 0X1A, 0XFF, 0XED, 0XBE, 0XF5, 0X5F, 0X96, 0X4F, 0XE1, 0XF2, //
        0XFC, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X1F, 0XC7, 0X70, //
        0XFE, 0XD5, 0X7F, 0X3B, 0X0E, 0X01, 0X00, 0X40, 0X04, 0X20, 0X00, 0X00, 0X08, 0X00, 0X40, 0X00, //
        0X08, 0X00, 0X80, 0X00, 0X08, 0X00, 0X40, 0X02, 0X20, 0X00, 0X00, 0X04, 0X40, 0X00, 0X10, 0X00, //
        0X03, 0X05, 0X0F, 0X3F, 0X2D, 0X3C, 0X08, 0X00, 0X80, 0X00, 0X54, 0XEF, 0XBE, 0XF7, 0XFD, 0X03, //
        0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X01, //
        0X58, 0XEF, 0XBD, 0XF7, 0X6E, 0XC3, 0XC1, 0X40, 0XE0, 0X60, 0XE8, 0XE0, 0XA4, 0X00, 0X00, 0X02, //
        0X00, 0X20, 0X00, 0X04, 0X80, 0X00, 0X10, 0X00, 0X04, 0X00, 0X40, 0X00, 0X04, 0X00, 0X40, 0X04, //
        0X00, 0X00, 0X40, 0X08, 0X01, 0X00, 0X00, 0X44, 0X00, 0X02, 0X00, 0X00, 0X20, 0X00, 0X00, 0XFF, //
        0XBE, 0XEB, 0X7F, 0XD2, 0X00, 0X3F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFE, 0XFC, 0XF1, 0XF2, 0XF7, 0X4B, 0X07, 0XFD, 0XD7, 0X7F, 0XFD, 0X83, 0X08, //
        0X00, 0X02, 0X80, 0X01, 0X20, 0X00, 0X00, 0X04, 0X00, 0X20, 0X00, 0X10, 0X02, 0X00, 0X00, 0X10, //
        0X00, 0X82, 0X00, 0X20, 0X00, 0X04, 0X00, 0X00, 0X22, 0X00, 0X00, 0X08, 0X00, 0X82, 0X00, 0X01, //
        0X20, 0X00, 0X00, 0X6D, 0XFF, 0XAD, 0XFF, 0XAA, 0X04, 0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X02, 0X00, 0XFF, 0XDD, //
        0X77, 0XFF, 0X48, 0X00, 0X00, 0X08, 0X00, 0X00, 0X88, 0X00, 0X00, 0X21, 0X00, 0X08, 0X00, 0X00, //
        0X42, 0X00, 0X21, 0X00, 0X00, 0X10, 0X00, 0X08, 0X00, 0X00, 0X81, 0X10, 0X00, 0X04, 0X00, 0X81, //
        0X00, 0X20, 0X00, 0X08, 0X00, 0X04, 0X00, 0XBB, 0XFF, 0X6B, 0XFE, 0X55, 0X80, 0XBF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XE0, 0X0A, 0XFF, 0XAB, 0XFF, 0XDD, 0X00, 0X00, 0X80, 0X11, 0X00, 0XC0, 0XE0, 0XB0, 0XE8, //
        0X70, 0XF2, 0X58, 0XF8, 0XA8, 0X7C, 0X58, 0X74, 0XB8, 0X78, 0XAA, 0X78, 0XB9, 0X68, 0X78, 0X58, //
        0XF8, 0X34, 0X58, 0XF0, 0XB8, 0XF0, 0XE0, 0X80, 0X22, 0X00, 0X00, 0X22, 0XFF, 0XF7, 0X5D, 0XF7, //
        0X08, 0XE2, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XF0, 0XCA, 0X0D, 0X3F, 0X6B, 0X3E, 0X58, 0X74, 0X3C, 0X57, //
        0X3F, 0X85, 0XCE, 0XF1, 0XF9, 0XFC, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, //
        0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XF8, 0XC2, 0X8F, 0X3E, 0X2B, 0X7E, 0X2C, 0X78, 0X3C, 0X6A, //
        0X1F, 0XBD, 0X87, 0XCD, 0XE2, 0XFC, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, //
        0X0F, 0X0F, 0X0F, 0X0F};                                                                        //
    //80*60
    const uint8_t LieDownCat[640] = {
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, 0XC0, 0XC0, //
        0XE0, 0XE0, 0XF0, 0XF0, 0XF8, 0XF8, 0XF8, 0XFC, 0XFC, 0XFC, 0X7C, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, //
        0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0X7E, 0XFC, 0XFC, 0XFC, 0XF8, 0XF8, 0XF0, 0XF0, 0XE0, //
        0XE0, 0XC0, 0XC0, 0X80, 0X00, 0X80, 0X80, 0XC0, 0XC0, 0XE0, 0XC0, 0XC0, 0X80, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0XC0, 0XE0, 0XF0, 0XF8, 0XFC, 0XFE, 0XFE, 0X7F, 0X3F, 0X1F, 0X1F, //
        0X0F, 0X07, 0X07, 0X03, 0X01, 0X01, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC0, 0XF0, 0XF8, 0XFC, //
        0XFC, 0XFE, 0X7E, 0XFE, 0XFC, 0XFC, 0XF8, 0XF0, 0XF0, 0XE0, 0XC0, 0XC1, 0XC3, 0XC3, 0XC7, 0XC7, //
        0XEF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X0F, 0X0F, 0X1F, 0X3F, 0X7F, 0XFE, 0XFC, 0XFC, //
        0XF8, 0XF0, 0XF0, 0XE0, 0XC0, 0X80, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0XE0, 0XF8, 0XFE, 0XFF, 0XFF, 0XFF, 0X3F, 0X0F, 0X03, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0XE0, 0XF8, 0XF8, 0XFC, 0XFE, 0XFF, 0X7F, 0X3F, 0X1F, //
        0X07, 0X01, 0X00, 0X00, 0X01, 0X01, 0X03, 0X07, 0X07, 0X07, 0X07, 0X07, 0X07, 0X07, 0X07, 0X07, //
        0X07, 0X07, 0X07, 0X07, 0X03, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X01, //
        0X03, 0X07, 0X0F, 0X1F, 0X7F, 0XFF, 0XFF, 0XFE, 0XF8, 0XE0, 0X80, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X80, 0XF0, 0XFC, 0XFE, 0XFF, 0XFF, 0X3F, 0X0F, 0X07, 0X01, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XC0, 0XE0, 0XF0, 0XF0, 0XE0, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0XE0, 0XE0, 0XE0, 0XE0, 0XC0, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X1F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0XE0, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X03, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XF0, 0XC0, 0X80, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X61, 0XFB, 0XFB, 0XF9, 0XF8, 0XF0, 0XE0, //
        0XF0, 0XE0, 0XE0, 0XE0, 0XE0, 0XE0, 0XE3, 0XC7, 0X07, 0X03, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0XE0, 0XF0, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X01, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X03, 0X0F, 0X1F, 0X3F, 0X7F, 0XFF, 0XFE, 0XFC, 0XF8, 0XF0, 0XF0, 0XE0, 0XE0, //
        0XC0, 0XC0, 0X80, 0X80, 0X01, 0X03, 0X07, 0X0F, 0X1F, 0X1F, 0X1F, 0X1F, 0X0E, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X03, 0X03, 0X07, 0X07, 0X07, //
        0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X0F, 0X07, 0X03, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X80, 0X80, //
        0XC0, 0XE0, 0XF0, 0XFC, 0XFF, 0XFF, 0XFF, 0X3F, 0X0F, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, 0X01, 0X03, 0X03, 0X07, 0X07, 0X0F, //
        0X0F, 0X0F, 0X1F, 0X1F, 0X3F, 0X3F, 0X3F, 0X7E, 0X7E, 0X7E, 0X7C, 0XFC, 0XFC, 0XFC, 0XF8, 0XF8, //
        0XF8, 0XF8, 0XF8, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, 0XF0, //
        0XF0, 0XF0, 0XF0, 0XF8, 0XF8, 0XF8, 0XF8, 0XFC, 0XFC, 0XFC, 0X7E, 0X7E, 0X7E, 0X3F, 0X3F, 0X1F, //
        0X1F, 0X0F, 0X07, 0X07, 0X03, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X01, //
        0X01, 0X01, 0X01, 0X01, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, 0X03, //
        0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
        0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, //
    };
}
#endif