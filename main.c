#include "main.h"

struct input_event input;   //读取触摸屏数据需要用到的结构体
#define TOUCH_PATH "/dev/input/event0"

typedef struct touch_dev
{
    int touch_fd;
    int x_size;
    int y_size;
    float x_fold;
    float y_fold;
    int ABS_X_Buf;  //x坐标
    int ABS_Y_Buf;  //y坐标
    int BTN_TOUCH_Buf;  //按下弹起标志
//    unsigned char *display;
}touch_dev_t;


#pragma pack (1)
typedef struct tagBITMAPFILEHEADER
{ 
    unsigned short  bfType;             //2Bytes，必须为"BM"，即0x424D 才是Windows位图文件
    unsigned int    bfSize;             //4Bytes，整个BMP文件的大小
    unsigned short  bfReserved1;        //2Bytes，保留，为0
    unsigned short  bfReserved2;        //2Bytes，保留，为0
    unsigned int    bfOffBits;          //4Bytes，文件起始位置到图像像素数据的字节偏移量
} BMP_BITMAPFILEHEADER;
#pragma pack ()

#pragma pack(1)
typedef struct _tagBMP_INFOHEADER
{
    unsigned int    biSize;                     //4Bytes，INFOHEADER结构体大小，存在其他版本INFOHEADER，用作区分
    unsigned int    biWidth;                    //4Bytes，图像宽度（以像素为单位）
    unsigned int    biHeight;                   //4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
    unsigned short  biPlanes;                   //2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
    unsigned short  biBitCount;                 //2Bytes，图像像素位数, 1个像素占据多少位
    unsigned int    biCompression;              //4Bytes，0：不压缩，1：RLE8，2：RLE4
    unsigned int    biSizeImage;                //4Bytes，4字节对齐的图像数据大小
    unsigned int    biXPelsPerMeter;            //4 Bytes，用象素/米表示的水平分辨率
    unsigned int    biYPelsPerMeter;            //4 Bytes，用象素/米表示的垂直分辨率
    unsigned int    biClrUsed;                  //4 Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
    unsigned int    biClrImportant;             //4 Bytes，重要的调色板索引数，0：所有的调色板索引都重要
}BMP_INFOHEADER;
#pragma pack()

typedef struct bmp_dev
{
    int bmp_fd;     //bmp图片的文件描述符(文件编号)
    int width;      //bmp宽度
    int height;     //bmp高度
    int bitcount;   //bmp位深度( 一个占据多少个字节)
    unsigned char *data;    //存储已经添加了A(亮度)数据，颜色已经翻转的数据，图像垂直翻转
}bmp_dev_t;    //

typedef struct lcd_dev
{
    int lcd_fd;     //lcd液晶屏幕驱动文件的文件描述符
    int width;
    int height;
    int bitcount;
    unsigned char *display; //写入BMP图片颜色数据的内存
}lcd_dev_t;

#define FB0_PATH "/dev/fb0" //显示屏驱动文件路径

//初始化触摸屏函数
int touch_init(touch_dev_t *touch, lcd_dev_t *lcd)
{
    //1.打开触摸屏驱动文件
    touch->touch_fd = open(TOUCH_PATH, O_RDONLY);
    touch->x_size = 1024;
    touch->y_size = 600;
    touch->x_fold = 0.78125;//(lcd->width < touch->x_size)? (lcd->width / touch->x_size) : (touch->x_size / lcd->width) ;
    touch->y_fold = 0.8;//(lcd->height < touch->y_size)? (lcd->height / touch->y_size) : (touch->y_size / lcd->height) ;
    printf("x_fold = %f ,y_fold = %f\r\n",touch->x_fold ,touch->y_fold);
    touch->ABS_X_Buf = 0;
    touch->ABS_Y_Buf = 0;
    touch->BTN_TOUCH_Buf = 0;
    if( touch->touch_fd == -1)
    {
        printf("打开触摸屏驱动(%s)失败\r\n", TOUCH_PATH);
        return -1;
    }
    return 0;
}




//初始化lcd的参数
int lcd_init(lcd_dev_t *lcd)
{
    lcd->lcd_fd = open( FB0_PATH, O_RDWR);
    if( lcd->lcd_fd == -1)
    {
        perror("open display device error");
        return -1;
    }

    //配置lcd的参数
    lcd->width      = 800;
    lcd->height     = 480;
    lcd->bitcount   = 4;

    //lcd屏幕的驱动文件映射
    lcd->display = mmap( NULL, lcd->width * lcd->height * lcd->bitcount, PROT_WRITE|PROT_READ, MAP_SHARED, lcd->lcd_fd, 0);
    if( lcd->display == NULL)
    {
        perror("display device mmap error");
        return -2;
    }

    return 0;
}

//将图片显示在LCD
int lcd_display_bmp(lcd_dev_t *lcd, bmp_dev_t *bmp, int x, int y)
{
    //7.写入BMP图片信息  
    unsigned char *lcd_display = &lcd->display[4 *(y*lcd->width+x)];    //确定屏幕的写入位置 通过y*lcd->width+x确定写入的像素点的位置
    unsigned char *pic         = bmp->data;                             //定义一个第三方的指针，指向保存bmp颜色数据的内存

    x = x > 800 ? 800 : x;
    x = x < 0   ? 0   : x;
    y = y > 480 ? 480 : y;
    y = y < 0   ? 0   : y;   

    int width  = ( x + bmp->width > lcd->width ) ? (lcd->width - x) : ( bmp->width);    //确定bmp图片实际需要显示的宽度
    int height = ( y+ bmp->height> lcd->height) ?  (lcd->height- y) : (bmp->height);    //确定bmp图片实际需要显示的高度

    int i=0;
    for( i =0; i< height; i++)
    {
        memcpy( lcd_display, pic, width*4);     //读取bmp图片信息，写入lcd屏幕，写入实际需要显示一行的数据
        lcd_display += lcd->width*4;            //将LCD屏幕的写入位置移动到下一行
        pic += bmp->width*4;                    //将bmp图片的读取位置移动到下一行
    } 
}

//点动获取触摸屏坐标
int get_touch_coordinates(touch_dev_t *touch)
{
        read( touch->touch_fd, &input, sizeof(input));    //阻塞性函数

        //3.处理从触摸屏驱动文件中读取数据
        // printf("事件类型:%d\r\n", input.type);
        // printf("事件代码:%d\r\n", input.code);
        // printf("发生的值:%d\r\n", input.value);
        if( input.type == EV_ABS && input.code == ABS_X)
        {
            touch->ABS_X_Buf = input.value * touch->x_fold;
            printf("x(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
        }else if( input.type == EV_ABS && input.code == ABS_Y)
        {
            touch->ABS_Y_Buf = input.value * touch->y_fold;
            printf("y(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
        }else if( input.type == EV_KEY && input.code == BTN_TOUCH)
        {
            touch->BTN_TOUCH_Buf = input.value;  
            printf("B(%d)\r\n",touch->BTN_TOUCH_Buf);  
        }
        return  touch->BTN_TOUCH_Buf;
}

//滑动获取触摸屏坐标
int get_touch_coordinates_x(touch_dev_t *touch)
{
    static int x,y = 0;
    static unsigned char next_bit = 0;
    read( touch->touch_fd, &input, sizeof(input));    //阻塞性函数

    if( input.type == EV_ABS && input.code == ABS_X)
    {
        x = input.value * touch->x_fold;
//        printf("x(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
    }else if( input.type == EV_ABS && input.code == ABS_Y)
    {
        y = input.value * touch->y_fold;
//        printf("y(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
    }else if( input.type == EV_KEY && input.code == BTN_TOUCH)
    {
        touch->BTN_TOUCH_Buf = input.value;    
        printf("B(%d)\r\n",touch->BTN_TOUCH_Buf); 

    } 

    if(touch->BTN_TOUCH_Buf != 0)
    {
        if(touch->ABS_X_Buf != x && touch->ABS_Y_Buf != y || next_bit > 2) //要有滑动才画点
        {   
            next_bit = 0;
            touch->ABS_X_Buf = x; 
            touch->ABS_Y_Buf = y;
            printf("x(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
        }else
        {
            next_bit ++;
        }
    }else
    {
        x = 0;
        y = 0;
        next_bit = 0;
    }


    return  touch->BTN_TOUCH_Buf;
}

//画点函数
//point_size: 点宽度
//sRGB
int lcd_display_point(lcd_dev_t *lcd, int point_size,int sRGB, int x, int y)
{
    unsigned char *lcd_display;

    x = x > lcd->width ? lcd->width : x;
    x = x < 0   ? 0   : x;
    y = y > lcd->height ? lcd->height : y;
    y = y < 0   ? 0   : y;  

    if(x == 0 || y == 0)
        return 0;

    lcd_display = &lcd->display[4 *(y*lcd->width+x)];    //确定屏幕的写入位置 通过y*lcd->width+x确定写入的像素点的位置
    unsigned char sRGB_buf[4] = {(sRGB>>24)&0xff, (sRGB>>16)&0xff, (sRGB>>8)&0xff, sRGB&0xff};  //拆分颜色

    memcpy(lcd_display, sRGB_buf, 4);


    return 1;
}

//卸载lcd
void lcd_uninit(lcd_dev_t *lcd)
{
    if(lcd->display == NULL ) return;
    printf("p->%p\r\n",lcd->display);
    munmap( lcd->display, lcd->height * lcd->width * lcd->bitcount);
    close( lcd->lcd_fd);
}

//初始化图片
int bmp_init( bmp_dev_t *bmp, const char *BmpPath)
{
    BMP_BITMAPFILEHEADER  bitmap;
    BMP_INFOHEADER        info;

    //1.打开BMP图片
    bmp->bmp_fd = open(BmpPath,O_RDONLY);
    if( bmp->bmp_fd == -1)
    {
        printf("打开图片失败\r\n");
        return -1;
    }

    //2.读取BMP头数据
    read( bmp->bmp_fd, &bitmap, sizeof( bitmap));   //位图信息(图片文件的类型)
    read( bmp->bmp_fd, &info,   sizeof( info));     //头信息(图片的宽高)
    printf("BMP图片的宽:%d 高:%d\r\n", info.biWidth, info.biHeight);
    printf("一个像素%d个字节\r\n", info.biBitCount/8);

    //3.读取颜色数据
    bmp->width       = info.biWidth;
    bmp->height      = info.biHeight;
    bmp->bitcount    = info.biBitCount / 8; //判断图片位深度

    return 0;
}

//图片数据处理
void bmp_readdata( bmp_dev_t *bmp)
{
    unsigned char *bmp_buff = malloc( bmp->width * bmp->height * bmp->bitcount);   //申请堆空间存放bmp图片信息
    read( bmp->bmp_fd, bmp_buff, bmp->width * bmp->height * bmp->bitcount);                //读取数据

     //4.填充A数据，颜色赋值
    unsigned char * tmp_buff = malloc( bmp->width * bmp->height * 4);
    if( bmp->bitcount == 3 ) //24位位图
    {
        int bit;
        for(  bit= 0; bit < bmp->width * bmp->height ; bit++)
        {
            tmp_buff[ 4 * bit]     = bmp_buff[ 3 * bit ]; 
            tmp_buff[ 4 * bit + 1] = bmp_buff[ 3 * bit + 1];
            tmp_buff[ 4 * bit + 2] = bmp_buff[ 3 * bit + 2];
            tmp_buff[ 4 * bit + 3] = 0x00;
        }
    }
    else    //其他位深度的图片
    {


    }

     //5.垂直翻转
    int x,y;
    bmp->data = malloc( bmp->width * bmp->height * 4);
#if 1
    for( y = 0; y< bmp->height; y++)
    {
        for( x = 0; x < bmp->width ; x++)
        {
            bmp->data[4*(bmp->width*y+x)]    = tmp_buff[ 4*(bmp->width*(bmp->height-y-1)+x)];
            bmp->data[4*(bmp->width*y+x)+1]  = tmp_buff[ 4*(bmp->width*(bmp->height-y-1)+x)+1];
            bmp->data[4*(bmp->width*y+x)+2]  = tmp_buff[ 4*(bmp->width*(bmp->height-y-1)+x)+2];
            bmp->data[4*(bmp->width*y+x)+3]  = tmp_buff[ 4*(bmp->width*(bmp->height-y-1)+x)+3];
        }
    }
#else
    for( y = 0 ; y < bmp->height ; y++)
        memcpy( &bmp->data[4*bmp->width*y], &tmp_buff[4*bmp->width*(bmp->height-1-y)] , bmp->width*4);
#endif

    free(bmp_buff);
    free(tmp_buff);

    return ;
}

void bmp_uninit( bmp_dev_t *bmp)
{
    free(bmp->data);
    close( bmp->bmp_fd);
}


int main()
{
    lcd_dev_t lcd;
    bmp_dev_t bmp;
    touch_dev_t touch_dev_user;

    lcd_init(&lcd); //初始化屏幕

    bmp_init(&bmp, "b.bmp");    //初始化bmp图片
    bmp_readdata(&bmp);         //读取，修改bmp图片的信息
    touch_init(&touch_dev_user,&lcd); //初始化触摸屏    
    lcd_display_bmp( &lcd, &bmp, 0,0);  //显示bmp图片
   
    bmp_uninit( &bmp);          //显示完图片后，必须调用该函数，销毁申请bmp的堆空间
    
     while(1)
    {
        get_touch_coordinates_x(&touch_dev_user); //获取触摸坐标
        lcd_display_point(&lcd, 1, 0x00ffffff, touch_dev_user.ABS_X_Buf, touch_dev_user.ABS_Y_Buf); //画点
        
    }

    lcd_uninit( &lcd);  //卸载lcd
    return 0;
}




