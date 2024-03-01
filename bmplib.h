#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN_OF_LOC 512
#define _NO_BMP_FILE printf("文件不存在\n")
#define _BMP_COMPRESSED printf("文件已压缩，暂不支持\n")

//thanks:https://zhuanlan.zhihu.com/p/122959774

// 文件信息头结构体
typedef struct HFILE
{
    unsigned short bfType;        // 19778，必须是BM字符串，对应的十六进制为0x4d42,十进制为19778，否则不是bmp格式文件
    unsigned int   bfSize;        // 文件大小 以字节为单位(2-5字节)
    unsigned short bfReserved1;   // 保留，必须设置为0 (6-7字节)
    unsigned short bfReserved2;   // 保留，必须设置为0 (8-9字节)
    unsigned int   bfOffBits;     // 从文件头到像素数据的偏移  (10-13字节)
};

//图像信息头结构体
typedef struct HBMP
{
    unsigned int    biSize;          // 此结构体的大小 (14-17字节)
    long            biWidth;         // 图像的宽  (18-21字节)
    long            biHeight;        // 图像的高  (22-25字节)
    unsigned short  biPlanes;        // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)
    unsigned short  biBitCount;      // 一像素所占的位数，一般为24   (28-29字节)
    unsigned int    biCompression;   // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)
    unsigned int    biSizeImage;     // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)
    long            biXPelsPerMeter; // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)
    long            biYPelsPerMeter; // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)
    unsigned int    biClrUsed;       // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)
    unsigned int    biClrImportant;  // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)
} ;

//24位图像素信息结构体,即调色板
typedef struct _PIX {
    unsigned char rgbBlue;   //该颜色的蓝色分量  (值范围为0-255)
    unsigned char rgbGreen;  //该颜色的绿色分量  (值范围为0-255)
    unsigned char rgbRed;    //该颜色的红色分量  (值范围为0-255)
    unsigned char rgbReserved;// 保留，必须为0
};

typedef struct _XY {
    int x;
    int y;
};

typedef struct BMP {
    _XY xy;
    _PIX p;//值
    BMP* xp;//横坐标下
    BMP* xn;//横坐标上
    BMP* yp;//纵坐标下
    BMP* yn;//纵坐标上
};

typedef struct INFO {
    HFILE headfile;
    HBMP headinfo;
    FILE* loc_of_file;
};

//max str==500,wasted
void __rewritebmp(char* str) {
    int sum = 0;
    int a = 0;
    char ender[MAX_STR_LEN_OF_LOC];
    for (; *(str + a) != '\0'; a++) {
        ender[a+sum] = str[a];
        if (*(str + a) == '\\') {
            sum++;
            ender[a + sum] = '\\';
        }
    }
    ender[a + sum] = '\0';
    strcpy(str, ender);
}

//open a bmp file & read.
//useage:
// FILE* picture=openbmp("/*your bmp file location*/");
// then use other func
// need to use "\\"
// 打开一个bmp文件。
// 用法：
// FILE* picture=openbmp("/*文件地址*/");
// 然后用其他函数
// 需要使用"\\"
//
inline INFO openbmp(const char* loc) {
    INFO temp = {};
    FILE* bmp = fopen(loc, "rb");
    if (bmp == NULL) {
        _NO_BMP_FILE;
        return temp;//无文件
    }
        
    unsigned short  fileType;
    fread(&fileType, 1, sizeof(unsigned short), bmp);
    if (fileType != 0x4d42) {
        return temp;//非bmp1
    }
    fseek(bmp, 0, SEEK_SET);//(文件，偏移量（+），指针位置)
    fread(&temp.headfile.bfType,2, 1, bmp);
    fread(&temp.headfile.bfSize, 4, 1, bmp);
    fread(&temp.headfile.bfReserved1, 2, 1, bmp);
    fread(&temp.headfile.bfReserved2, 2, 1, bmp);
    fread(&temp.headfile.bfOffBits, 2, 1, bmp);
    
    //reserved
    fseek(bmp, 2, SEEK_CUR);
       
    fread(&temp.headinfo.biSize, 4, 1, bmp);
    fread(&temp.headinfo.biWidth, 4, 1, bmp);
    fread(&temp.headinfo.biHeight, 4, 1, bmp);
    fread(&temp.headinfo.biPlanes, 2, 1, bmp);
    fread(&temp.headinfo.biBitCount, 2, 1, bmp);
    fread(&temp.headinfo.biCompression, 4, 1, bmp);
    fread(&temp.headinfo.biSizeImage, 4, 1, bmp);
    fread(&temp.headinfo.biXPelsPerMeter, 4, 1, bmp);
    fread(&temp.headinfo.biYPelsPerMeter, 4, 1, bmp);
    fread(&temp.headinfo.biClrUsed, 4, 1, bmp);
    fread(&temp.headinfo.biClrImportant, 4, 1, bmp);
    temp.loc_of_file = bmp;//54以后（包含）
    if (temp.headinfo.biCompression != 0) {
        _BMP_COMPRESSED;
    }
    return temp;
}

void _bmpmalloc0(BMP* temp0, INFO* bmp, int x,int y) {//首相，文件,x,y
    if (x == 0 && y == 0) {
        return;
    }
    BMP* now = temp0;
    if (x == 0) {
        while (1) {
            if (now->yp == NULL) {//开新行
                BMP* temp1 = (BMP*)malloc(sizeof(BMP));//当前操作为temp1，now是yn
                now->yp = temp1;
                temp1->yn = now;
                temp1->xn = NULL;
                temp1->yp = NULL;
                temp1->xp = NULL;
                temp1->xy.x = x;
                temp1->xy.y = y;
                FILE* temp2 = bmp->loc_of_file;
                fseek(temp2, bmp->headinfo.biBitCount * 0.125 * (-x + (1+y) * bmp->headinfo.biWidth) * (-1), SEEK_END);
                fread(&(temp1->p.rgbRed), 1, 1, temp2);
                fread(&(temp1->p.rgbGreen), 1, 1, temp2);
                fread(&(temp1->p.rgbBlue), 1, 1, temp2);
                if (bmp->headinfo.biBitCount > 24)
                    fread(&(temp1->p.rgbReserved), 1, 1, temp2);
                else
                    temp0->p.rgbReserved = 0xff;
                return;
            }
            else {
                now = now->yp;
            }
        }
    }
    else {
        while (1) { 
            if (now->yp == NULL) {
                break;
            }
            else {
                now = now->yp;
            }
        }
    }

    int count = 0;
    while (1) {
        if (now->xp == NULL) {
            if (bmp->headinfo.biWidth > count) {
                BMP* temp1 = (BMP*)malloc(sizeof(BMP));//当前操作为temp1，now是xn
                now->xp = temp1;
                temp1->xn = now;
                temp1->xy.x = x;
                temp1->xy.y = y;
                FILE* temp2 = bmp->loc_of_file;
                fseek(temp2, bmp->headinfo.biBitCount  *0.125 * (-x + (1 + y) * bmp->headinfo.biWidth)*(-1), SEEK_END);
                fread(&(temp1->p.rgbRed), 1, 1, temp2);
                fread(&(temp1->p.rgbGreen), 1, 1, temp2);
                fread(&(temp1->p.rgbBlue), 1, 1, temp2);
                if (bmp->headinfo.biBitCount > 24)
                    fread(&(temp1->p.rgbReserved), 1, 1, temp2);
                else
                    temp0->p.rgbReserved = 0xff;
                if (y == 0) {//首行
                    temp1->yn = NULL;
                }
                else {//非首航
                    temp1->yn = now->yn->xp;
                    now->yn->xp->yp = temp1;
                }
                temp1->xp = NULL;
                temp1->yp = NULL;
                return;
            }

        }
        else {
            now = now->xp;
            count++;
        }
    }
}

//return a ptr to bmp start(0,0);
// need a ptr to INFO
// 4byte
// 返回图像的内存地址，用其他东西调用，信息为（0，0）
// 需要一个INFO的地址
// 4字节的颜色空间
//
BMP* bmpmalloc(INFO* bmp) {
    BMP* temp0 = (BMP*)malloc(sizeof(BMP));
    temp0->xy.x = 0;
    temp0->xy.y = 0;
    temp0->xn = NULL;
    temp0->yn = NULL;
    temp0->xp = NULL;
    temp0->yp = NULL;
    FILE* draw = bmp->loc_of_file;
    fseek(draw, bmp->headinfo.biBitCount*(-1), SEEK_END);
    fread(&(temp0->p.rgbRed), 1,1, draw);
    fread(&(temp0->p.rgbGreen), 1, 1, draw);
    fread(&(temp0->p.rgbBlue), 1, 1, draw);
    if (bmp->headinfo.biBitCount > 24)
        fread(&(temp0->p.rgbReserved), 1, 1, draw);
    else
        temp0->p.rgbReserved = 0xff;
    int t0 = 0, t1 = 0;
    for (; t0 < bmp->headinfo.biHeight; t0++) {//可多线程优化
        for (t1 = 0; t1 < bmp->headinfo.biWidth; t1++) {
            _bmpmalloc0(temp0, bmp, t1, t0);//xy
        }
    }
    fclose(bmp->loc_of_file);
    return temp0;
    
}

inline void _bmpfree0(BMP* bmp) {//整行
    while ((bmp->xp) != NULL) {
        bmp = bmp->xp;
        free(bmp->xn);
    }
    free(bmp);
    return;
}

//restall memory from an opened bmp
//回收内存
void bmpfree(BMP* bmp) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    while ((bmp->yp) != NULL) {//此处多线程优化
        bmp = bmp->yp;
        _bmpfree0(bmp->yn);
    }
    _bmpfree0(bmp);
    return;
}

// open a BMP file only, return as a BMP ptr
// useage:
// BMP* =bmpin(/*your file location*/);
// without "\\",just one is ok
// 仅打开一个bmp文件，返回BMP指针
// 不需要用双斜杠
//
BMP* bmpin(const char* loc) {
    INFO bmp = openbmp(loc);
    if (bmp.headfile.bfSize == 0) {
        return 0;
    }
    return (bmpmalloc(&bmp));
}

inline void _printcolor(int r, int g, int b) {
    printf("\033[38;5;%dm██", 36 * b + 6 * g + r + 16);
}

//quick view in consul
// 在控制台快速查看
void quickecho(BMP* bmp) {
    if (bmp == NULL) {
        _NO_BMP_FILE;
            return;
    }
    int t0=0;
    BMP* tempx = bmp;
    for (; t0 < bmp->xy.x; t0++) {//兼容无边界png与中间png
        bmp = bmp->xn;
    }
    for (t0 = 0; t0 < bmp->xy.y; t0++) {
        bmp = bmp->yn;
    }
    while ((bmp->yp) != NULL) {
        if (bmp->yp->xy.y >= bmp->xy.y) {//没问题
            tempx = bmp;
            while ((tempx->xp) != NULL) {
                if (tempx->xp->xy.x >= tempx->xy.x) {
                    _printcolor((int)(tempx->p.rgbRed) * 6 / 256, (int)(tempx->p.rgbGreen) * 6 / 256, (int)(tempx->p.rgbBlue) * 6 / 256);
                    tempx = tempx->xp;
                }
                else {//wbj
                    break;
                }
            }
            bmp = bmp->yp;
            printf("\n");
        }
        else {//无边界，退出时剩一行
            break;
        }
    }
    tempx = bmp;
    while ((tempx->xp) != NULL) {
        if (tempx->xp->xy.x >= tempx->xy.x) {
            _printcolor((int)(tempx->p.rgbRed) * 6 / 256, (int)(tempx->p.rgbGreen) * 6 / 256, (int)(tempx->p.rgbBlue) * 6 / 256);
            tempx = tempx->xp;
        }
        else {//wbj
            break;
        }
    }
    bmp = bmp->yp;
    printf("\033[0m\n");
}

void showbinary(const char* loc) {
    FILE* fp = fopen(loc, "rb");
    int t0 = 0;
    for (; t0 < 400; t0++) {
        unsigned char lev;
        fread(&lev, 1, 1, fp);
        printf("%X ", lev);
    }
}

//get (x,y)color
// returns an unsigned int of rgba, such as 0xff00aa00 means r=255,g=0,b=170,a=0
//获取图像XY的颜色值
//返回值是unsigned int，用法例如0xff00aa00，红=255，绿=0，蓝=170，透明度（或者没有）=0
unsigned int getcolor(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    for (; x > 0; x--) {
        bmp = bmp->xp;
    }
    for (; y > 0; y--) {
        bmp = bmp->yp;
    }
    unsigned int ender=0;
    ender += ((unsigned int)bmp->p.rgbRed << 24);
    ender += ((unsigned int)bmp->p.rgbGreen << 16);
    ender += ((unsigned int)bmp->p.rgbBlue << 8);
    ender += (unsigned int)bmp->p.rgbRed;
    return ender;
}

//get (x,y)red
//获取图像XY的红
char getred(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    for (; x > 0; x--) {
        bmp = bmp->xp;
    }
    for (; y > 0; y--) {
        bmp = bmp->yp;
    }
    return (unsigned char)(bmp->p.rgbRed); 
}

//get (x,y)green
//获取图像XY的绿
char getgreen(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    for (; x > 0; x--) {
        bmp = bmp->xp;
    }
    for (; y > 0; y--) {
        bmp = bmp->yp;
    }
    return (unsigned char)(bmp->p.rgbGreen);
}

//get (x,y)blue
//获取图像XY的蓝
char getblue(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    for (; x > 0; x--) {
        bmp = bmp->xp;
    }
    for (; y > 0; y--) {
        bmp = bmp->yp;
    }
    return (unsigned char)(bmp->p.rgbBlue);
}

//get (x,y)alpha
//获取图像XY的透明度（无透明度就是0xff）
char getalpha(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    for (; x > 0; x--) {
        bmp = bmp->xp;
    }
    for (; y > 0; y--) {
        bmp = bmp->yp;
    }
    return (unsigned char)(bmp->p.rgbReserved);
}

void _bmpoutrgba0(BMP* bmp, int num,FILE* file){//传入是图片x的最末尾，反着写                     **********************未知问题***************
    int t0 = num;
    BMP* temp = bmp;
    while (t0--) {
        fwrite(&temp->p.rgbRed, 1, 1, file);
        fwrite(&temp->p.rgbGreen, 1, 1, file);
        fwrite(&temp->p.rgbBlue, 1, 1, file);
        fwrite(&temp->p.rgbReserved, 1, 1, file);
        temp = temp->xn;
    }
}

//create an unimpressed BMP file.
//usage:
// bmpout(/*your target location*/,/*your BMP image ptr*/);
// NULL location will atuomatically set to current dir
// 创建一个未压缩的bmp文件
// 方法：
// bmpout(/*路径*/,/*BMP图片指针*/);
// 路径为NULL时，默认为当前程序目录
void bmpoutrgba(const char* location, BMP* bmp) {

    BMP* temp = bmp;
    if (temp->xy.x != 0 || temp->xy.y != 0) {//归零
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    int t0 = 1, t1 = 1;//xy
    while (1) {
        if (bmp->xp == NULL) {//end
            break;
        }
        else if (bmp->xp->xy.x <= bmp->xy.x) {//end                ***************可能问题****************
            break;
        }
        else {
            t0++;
            bmp = bmp->xp;
        }
    }
    while (1) {
        if (bmp->yp == NULL) {//end
            break;
        }
        else if (bmp->yp->xy.y <= bmp->xy.y) {//end
            break;
        }
        else {
            t1++;
            bmp = bmp->yp;
        }
    }

    unsigned int    biSize = 40;                      // 此结构体的大小 (14-17字节)
    long            biWidth = t0;                     // 图像的宽  (18-21字节)
    long            biHeight = t1;                    // 图像的高  (22-25字节)
    unsigned short  biPlanes = 1;                     // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1 (26-27字节)
    unsigned short  biBitCount = 32;                  // 一像素所占的位数，一般为24   (28-29字节)
    unsigned int    biCompression = 0;                // 说明图象数据压缩的类型，0为不压缩。 (30-33字节)
    unsigned int    biSizeImage = t1 * t0 * 32;       // 像素数据所占大小, 这个值应该等于上面文件头结构中bfSize-bfOffBits (34-37字节)
    long            biXPelsPerMeter = 4096;           // 说明水平分辨率，用象素/米表示。一般为0 (38-41字节)
    long            biYPelsPerMeter = 4096 * t1 / t0; // 说明垂直分辨率，用象素/米表示。一般为0 (42-45字节)
    unsigned int    biClrUsed = 0;                    // 说明位图实际使用的彩色表中的颜色索引数（设为0的话，则说明使用所有调色板项）。 (46-49字节)
    unsigned int    biClrImportant = 0;               // 说明对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要。(50-53字节)

    unsigned short bfType = 19778;           // 19778，必须是BM字符串，对应的十六进制为0x4d42,十进制为19778，否则不是bmp格式文件
    unsigned int   bfSize = 54 + biSizeImage;// 文件大小 以字节为单位(2-5字节)
    unsigned short bfReserved1 = 0;          // 保留，必须设置为0 (6-7字节)
    unsigned short bfReserved2 = 0;          // 保留，必须设置为0 (8-9字节)
    unsigned int   bfOffBits = 54;           // 从文件头到像素数据的偏移  (10-13字节)

    FILE* file = fopen(location, "wb");
    fwrite(&bfType, sizeof(bfType), 1, file);
    fwrite(&bfSize, sizeof(bfSize), 1, file);
    fwrite(&bfReserved1, sizeof(bfReserved1), 1, file);
    fwrite(&bfReserved2, sizeof(bfReserved2), 1, file);
    fwrite(&bfOffBits, sizeof(bfOffBits), 1, file);

    fwrite(&biSize, sizeof(biSize), 1, file);
    fwrite(&biWidth, sizeof(biWidth), 1, file);
    fwrite(&biHeight, sizeof(biHeight), 1, file);
    fwrite(&biPlanes, sizeof(biPlanes), 1, file);
    fwrite(&biBitCount, sizeof(biBitCount), 1, file);
    fwrite(&biCompression, sizeof(biCompression), 1, file);
    fwrite(&biSizeImage, sizeof(biSizeImage), 1, file);
    fwrite(&biXPelsPerMeter, sizeof(biXPelsPerMeter), 1, file);
    fwrite(&biYPelsPerMeter, sizeof(biYPelsPerMeter), 1, file);
    fwrite(&biClrUsed, sizeof(biClrUsed), 1, file);
    fwrite(&biClrImportant, sizeof(biClrImportant), 1, file);


    while (t1--) {
        _bmpoutrgba0(bmp, t0,file);
        bmp = bmp->yn;
    }
    fclose(file);

}

//************封装***************
//待添加的功能：写入bmp，颜色通道分离（用掩码）

#undef _NO_BMP_FILE
#undef MAX_STR_LEN_OF_LOC
