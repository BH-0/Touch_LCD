#include "lcd.h"

struct input_event input;   //读取触摸屏数据需要用到的结构体

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
