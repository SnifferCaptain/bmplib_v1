#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN_OF_LOC 512
#define _NO_BMP_FILE printf("�ļ�������\n")
#define _BMP_COMPRESSED printf("�ļ���ѹ�����ݲ�֧��\n")

//thanks:https://zhuanlan.zhihu.com/p/122959774

//QBMP is suggested

// �ļ���Ϣͷ�ṹ��
typedef struct HFILE
{
    unsigned short bfType;        // 19778��������BM�ַ�������Ӧ��ʮ������Ϊ0x4d42,ʮ����Ϊ19778��������bmp��ʽ�ļ�
    unsigned int   bfSize;        // �ļ���С ���ֽ�Ϊ��λ(2-5�ֽ�)
    unsigned short bfReserved1;   // ��������������Ϊ0 (6-7�ֽ�)
    unsigned short bfReserved2;   // ��������������Ϊ0 (8-9�ֽ�)
    unsigned int   bfOffBits;     // ���ļ�ͷ���������ݵ�ƫ��  (10-13�ֽ�)
};

//ͼ����Ϣͷ�ṹ��
typedef struct HBMP
{
    unsigned int    biSize;          // �˽ṹ��Ĵ�С (14-17�ֽ�)
    long            biWidth;         // ͼ��Ŀ�  (18-21�ֽ�)
    long            biHeight;        // ͼ��ĸ�  (22-25�ֽ�)
    unsigned short  biPlanes;        // ��ʾbmpͼƬ��ƽ��������Ȼ��ʾ��ֻ��һ��ƽ�棬���Ժ����1 (26-27�ֽ�)
    unsigned short  biBitCount;      // һ������ռ��λ����һ��Ϊ24   (28-29�ֽ�)
    unsigned int    biCompression;   // ˵��ͼ������ѹ�������ͣ�0Ϊ��ѹ���� (30-33�ֽ�)
    unsigned int    biSizeImage;     // ����������ռ��С, ���ֵӦ�õ��������ļ�ͷ�ṹ��bfSize-bfOffBits (34-37�ֽ�)
    long            biXPelsPerMeter; // ˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0 (38-41�ֽ�)
    long            biYPelsPerMeter; // ˵����ֱ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0 (42-45�ֽ�)
    unsigned int    biClrUsed;       // ˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ����� (46-49�ֽ�)
    unsigned int    biClrImportant;  // ˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��(50-53�ֽ�)
} ;

//24λͼ������Ϣ�ṹ��,����ɫ��
typedef struct _PIX {
    unsigned char rgbBlue;   //����ɫ����ɫ����  (ֵ��ΧΪ0-255)
    unsigned char rgbGreen;  //����ɫ����ɫ����  (ֵ��ΧΪ0-255)
    unsigned char rgbRed;    //����ɫ�ĺ�ɫ����  (ֵ��ΧΪ0-255)
    unsigned char rgbReserved;// ����������Ϊ0
};

typedef struct _XY {
    int x;
    int y;
};

typedef struct BMP {
    _XY xy;
    _PIX p;//ֵ
    BMP* xp;//��������
    BMP* xn;//��������
    BMP* yp;//��������
    BMP* yn;//��������
};

typedef struct INFO {
    HFILE headfile;
    HBMP headinfo;
    FILE* loc_of_file;
};

//bmp*���ڴ�ͼƬ����
typedef struct QBMP {
    short hight;
    short width;
    char type;//type=0�ǻҶ�256��type=1��rgb��type=2��rgba��
    unsigned char* bmp;
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

//set bmp ptr to file head
//��ͼƬָ���Ƶ���ͷ
inline void zero(BMP* bmp) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
}

//open a bmp file & read.
//useage:
// FILE* picture=openbmp("/*your bmp file location*/");
// then use other func
// need to use "\\"
// ��һ��bmp�ļ���
// �÷���
// FILE* picture=openbmp("/*�ļ���ַ*/");
// Ȼ������������
// ��Ҫʹ��"\\"
//
inline INFO openbmp(const char* loc) {
    INFO temp = {};
    FILE* bmp = fopen(loc, "rb");
    if (bmp == NULL) {
        _NO_BMP_FILE;
        return temp;//���ļ�
    }
        
    unsigned short  fileType;
    fread(&fileType, 1, sizeof(unsigned short), bmp);
    if (fileType != 0x4d42) {
        return temp;//��bmp1
    }
    fseek(bmp, 0, SEEK_SET);//(�ļ���ƫ������+����ָ��λ��)
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
    temp.loc_of_file = bmp;//54�Ժ󣨰�����
    if (temp.headinfo.biCompression != 0) {
        _BMP_COMPRESSED;
    }
    return temp;
}

void _bmpmalloc0(BMP* temp0, INFO* bmp, int x,int y) {//���࣬�ļ�,x,y
    if (x == 0 && y == 0) {
        return;
    }
    BMP* now = temp0;
    if (x == 0) {
        while (1) {
            if (now->yp == NULL) {//������
                BMP* temp1 = (BMP*)malloc(sizeof(BMP));//��ǰ����Ϊtemp1��now��yn
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
                BMP* temp1 = (BMP*)malloc(sizeof(BMP));//��ǰ����Ϊtemp1��now��xn
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
                if (y == 0) {//����
                    temp1->yn = NULL;
                }
                else {//���׺�
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
// ����ͼ����ڴ��ַ���������������ã���ϢΪ��0��0��
// ��Ҫһ��INFO�ĵ�ַ
// 4�ֽڵ���ɫ�ռ�
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
    for (; t0 < bmp->headinfo.biHeight; t0++) {//�ɶ��߳��Ż�
        for (t1 = 0; t1 < bmp->headinfo.biWidth; t1++) {
            _bmpmalloc0(temp0, bmp, t1, t0);//xy
        }
    }
    fclose(bmp->loc_of_file);
    return temp0;
    
}

inline void _bmpfree0(BMP* bmp) {//����
    while ((bmp->xp) != NULL) {
        bmp = bmp->xp;
        free(bmp->xn);
    }
    free(bmp);
    return;
}

//restall memory from an opened bmp
//�����ڴ�
void bmpfree(BMP* bmp) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    while ((bmp->yp) != NULL) {//�˴����߳��Ż�
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
// ����һ��bmp�ļ�������BMPָ��
// ����Ҫ��˫б��
//
BMP* bmpin(const char* loc) {
    INFO bmp = openbmp(loc);
    if (bmp.headfile.bfSize == 0) {
        return 0;
    }
    return (bmpmalloc(&bmp));
}

inline void _printcolor(int r, int g, int b) {
    printf("\033[38;5;%dm����", 36 * b + 6 * g + r + 16);
}

inline void _printgrey(unsigned char grey) {
    printf("\033[38;5;%dm����", 232 + ((int)grey / 12));
}

//quick view in consul
// �ڿ���̨���ٲ鿴
void quickecho(BMP* bmp) {
    if (bmp == NULL) {
        _NO_BMP_FILE;
            return;
    }
    int t0=0;
    for (; t0 < bmp->xy.x; t0++) {//�����ޱ߽�png���м�png
        bmp = bmp->xn;
    }
    for (t0 = 0; t0 < bmp->xy.y; t0++) {
        bmp = bmp->yn;
    }
    BMP* tempx = bmp;
    while ((bmp->yp) != NULL) {
        if (bmp->yp->xy.y >= bmp->xy.y) {//û����
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
        else {//�ޱ߽磬�˳�ʱʣһ��
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
//��ȡͼ��XY����ɫֵ
//����ֵ��unsigned int���÷�����0xff00aa00����=255����=0����=170��͸���ȣ�����û�У�=0
unsigned int getcolor(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
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
//��ȡͼ��XY�ĺ�
char getred(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
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
//��ȡͼ��XY����
char getgreen(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
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
//��ȡͼ��XY����
char getblue(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
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
//��ȡͼ��XY��͸���ȣ���͸���Ⱦ���0xff��
char getalpha(BMP* bmp, int x, int y) {
    if (bmp->xy.x != 0 || bmp->xy.y != 0) {//����
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

void _bmpoutrgba0(BMP* bmp, int num,FILE* file){//������ͼƬx����ĩβ������д
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
// ����һ��δѹ����bmp�ļ�
// ������
// bmpout(/*·��*/,/*BMPͼƬָ��*/);
// ·��ΪNULLʱ��Ĭ��Ϊ��ǰ����Ŀ¼
void bmpoutrgba(const char* location, BMP* bmp) {

    BMP* temp = bmp;
    if (temp->xy.x != 0 || temp->xy.y != 0) {//����
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
        else if (bmp->xp->xy.x <= bmp->xy.x) {//end
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

    unsigned int    biSize = 40;                      // �˽ṹ��Ĵ�С (14-17�ֽ�)
    long            biWidth = t0;                     // ͼ��Ŀ�  (18-21�ֽ�)
    long            biHeight = t1;                    // ͼ��ĸ�  (22-25�ֽ�)
    unsigned short  biPlanes = 1;                     // ��ʾbmpͼƬ��ƽ��������Ȼ��ʾ��ֻ��һ��ƽ�棬���Ժ����1 (26-27�ֽ�)
    unsigned short  biBitCount = 32;                  // һ������ռ��λ����һ��Ϊ24   (28-29�ֽ�)
    unsigned int    biCompression = 0;                // ˵��ͼ������ѹ�������ͣ�0Ϊ��ѹ���� (30-33�ֽ�)
    unsigned int    biSizeImage = t1 * t0 * 32;       // ����������ռ��С, ���ֵӦ�õ��������ļ�ͷ�ṹ��bfSize-bfOffBits (34-37�ֽ�)
    long            biXPelsPerMeter = 4096;           // ˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0 (38-41�ֽ�)
    long            biYPelsPerMeter = 4096 * t1 / t0; // ˵����ֱ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0 (42-45�ֽ�)
    unsigned int    biClrUsed = 0;                    // ˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ����� (46-49�ֽ�)
    unsigned int    biClrImportant = 0;               // ˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��(50-53�ֽ�)

    unsigned short bfType = 19778;           // 19778��������BM�ַ�������Ӧ��ʮ������Ϊ0x4d42,ʮ����Ϊ19778��������bmp��ʽ�ļ�
    unsigned int   bfSize = 54 + biSizeImage;// �ļ���С ���ֽ�Ϊ��λ(2-5�ֽ�)
    unsigned short bfReserved1 = 0;          // ��������������Ϊ0 (6-7�ֽ�)
    unsigned short bfReserved2 = 0;          // ��������������Ϊ0 (8-9�ֽ�)
    unsigned int   bfOffBits = 54;           // ���ļ�ͷ���������ݵ�ƫ��  (10-13�ֽ�)

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

//create a limitless bmp file
// for example (0,0) is neighour to (0,/*end*/)
// �����ޱ߽�bmp�ļ�
// ���磨0��0���루0��ĩβ������ 
void bmplilitless(BMP* bmp) {
    BMP* temp = bmp;//const bmp =0,0
    if (temp->xy.x != 0 || temp->xy.y != 0) {//����
        int t0 = 0;
        for (; t0 < bmp->xy.x; t0++) {
            bmp = bmp->xn;
        }
        for (t0 = 0; t0 < bmp->xy.y; t0++) {
            bmp = bmp->yn;
        }
    }
    BMP* len0=bmp;//                   ****************************************
    while (len0->xp != NULL) {
        if (len0->xp->xy.x <= len0->xy.x) {
            break;
        }
        len0 = len0->xp;
    }
    BMP* len1 = bmp;
    while ((len1->yp) != NULL  && (len0->yp) != NULL) {
        if (len1->yp->xy.y <= len1->xy.y || len0->yp->xy.y <= len0->xy.y) {
            break;
        }
        len0->xp = len1;
        len1->xn = len0;
        len1 = len1->yp;
        len0 = len0->yp;
    }
    len0->xp = len1;
    len1->xn = len0;//quit len0
    BMP* hig0 = bmp;
    while ((len1->xp) != NULL && (hig0->xp) != NULL) {
        if (len1->xp->xy.x <= len1->xy.x || hig0->xp->xy.x <= hig0->xy.x) {
            break;
        }
        len1->yp = hig0;
        hig0->yn = len1;
        len1 = len1->xp;
        hig0 = hig0->xp;
    }
    len1->yp = hig0;
    hig0->yn = len1;
}

void qbmpecho(QBMP* qbmp) {
    if (qbmp == NULL)return;
    if (qbmp->bmp == NULL)return;
    int t0 = 0,t1;
    if (qbmp->type == 1) {
        for (; t0 < qbmp->hight; t0++) {
            for (t1 = 0; t1 < qbmp->width; t1++) {
                _printcolor((int)(*(qbmp->bmp +3*(t1 + t0 * qbmp->width))) * 6 / 256, (int)(*(qbmp->bmp +1+ 3 * (t1 + t0 * qbmp->width))) * 6 / 256, (int)(*(qbmp->bmp + 2+3 * (t1 + t0 * qbmp->width))) * 6 / 256);
            }
            printf("\n");
        }
    }
    else if (qbmp->type == 0) {
        for (; t0 < qbmp->hight; t0++) {
            for (t1 = 0; t1 < qbmp->width; t1++) {
                _printgrey(*(qbmp->bmp + t1 + t0 * qbmp->width));
            }
            printf("\n");
        }
    }
    else if (qbmp->type == 2) {//Ŀǰ��rgb��ȫһ����
        for (; t0 < qbmp->hight; t0++) {
            for (t1 = 0; t1 < qbmp->width; t1++) {
                _printcolor((int)(*(qbmp->bmp + 4 * (t1 + t0 * qbmp->width))) * 6 / 256, (int)(*(qbmp->bmp + 1 + 4 * (t1 + t0 * qbmp->width))) * 6 / 256, (int)(*(qbmp->bmp + 2 + 4 * (t1 + t0 * qbmp->width))) * 6 / 256);
            }
            printf("\n");
        }
    }
}

//malloc in a more efficent way
//but requires suffitient ram space
// use qbmpfree() to free
//�Ը����١�������ķ�ʽ�����ڴ�
//���Ƕ��ڴ�������һ����Ҫ��
//��qbmpfree�����ͷ��ڴ�
QBMP* qbmpmalloc(INFO* bmp) {
    QBMP* output = (QBMP*)malloc(sizeof(QBMP));
    output->hight = bmp->headinfo.biHeight;
    output->width = bmp->headinfo.biWidth;
    fseek(bmp->loc_of_file, bmp->headfile.bfOffBits, SEEK_SET);//�������ݿ�ͷ
    if (bmp->headinfo.biBitCount == 8) {//ԭ����ͼ
        output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width);//b
        int temph = 0;
        output->type = 0;
        for (; temph < output->hight; temph++) {
            int tempw = 0;
            for (; tempw < output->width; tempw++) {
                fseek(bmp->loc_of_file, -1 * (-tempw + (temph + 1) * output->width), SEEK_END);//��������
                fread(output->bmp +(tempw + temph * output->width), 1, 1, bmp->loc_of_file);
            }
        }
    }
    else if(bmp->headinfo.biBitCount == 32){
        output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width * 4);//rgba
        int temph = 0;
        output->type = 2;
        for (; temph < output->hight; temph++) {
            int tempw = 0;
            for (; tempw < output->width; tempw++) {
                fseek(bmp->loc_of_file, -4 * (-tempw + (temph + 1) * output->width), SEEK_END);//��������
                fread(output->bmp + 4 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
                fread(output->bmp + 1 + 4 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
                fread(output->bmp + 2 + 4 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
                fread(output->bmp + 3 + 4 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
            }
        }
    }
    else {// ��ͨͼƬ
        output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width * 3);//rgb
        int temph = 0;
        output->type = 1;
        for (; temph <output->hight; temph++) {
            int tempw = 0;
            for (; tempw <output->width; tempw++) {
                fseek(bmp->loc_of_file, -3*(-tempw + (temph+1) * output->width), SEEK_END);//��������
                fread(output->bmp + 3 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
                fread(output->bmp + 1 + 3 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
                fread(output->bmp + 2 + 3 * (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
            }
        }
    }
    return output;
}

inline void qbmpfree(QBMP* qbmp) {
    free(qbmp->bmp);//                    ******************ͬ�����ڴ�������⣬�ݲ����*********************
    qbmp->bmp = NULL;
    free(qbmp);
    return;
}

//reform into grey bmp
//����Ϊ��ɫͼ��
//use qbmpfree()
QBMP* gbmpmalloc(INFO* bmp) {
    QBMP* output = (QBMP*)malloc(sizeof(QBMP));
    output->hight = bmp->headinfo.biHeight;
    output->width = bmp->headinfo.biWidth;
    fseek(bmp->loc_of_file, bmp->headfile.bfOffBits, SEEK_SET);//�������ݿ�ͷ
    unsigned char r, g, b, a;
    output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width);
    if (bmp->headinfo.biBitCount == 8) {//ԭ����ͼ
        output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width);//b
        int temph = 0;
        output->type = 0;
        for (; temph < output->hight; temph++) {
            int tempw = 0;
            for (; tempw < output->width; tempw++) {
                fseek(bmp->loc_of_file, -1 * (-tempw + (temph + 1) * output->width), SEEK_END);//��������
                fread(output->bmp + (tempw + temph * output->width), 1, 1, bmp->loc_of_file);
            }
        }
    }
    else if (bmp->headinfo.biBitCount == 32) {
        output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width);//rgba
        int temph = 0;
        output->type = 0;
        for (; temph < output->hight; temph++) {
            int tempw = 0;
            for (; tempw < output->width; tempw++) {
                fseek(bmp->loc_of_file, -4 * (-tempw + (temph + 1) * output->width), SEEK_END);//�������ݣ���ʧalpha��
                fread(&r, 1, 1, bmp->loc_of_file);
                fread(&g, 1, 1, bmp->loc_of_file);
                fread(&b, 1, 1, bmp->loc_of_file);
                fread(&a, 1, 1, bmp->loc_of_file);
                *(output->bmp + (tempw + temph * output->width)) = (unsigned char)((r + g + b) / 3);
            }
        }
    }
    else {// ��ͨͼƬ
        output->bmp = (unsigned char*)malloc(sizeof(unsigned char) * output->hight * output->width * 3);//rgb
        int temph = 0;
        output->type = 0;
        for (; temph < output->hight; temph++) {
            int tempw = 0;
            for (; tempw < output->width; tempw++) {
                fseek(bmp->loc_of_file, -3 * (-tempw + (temph + 1) * output->width), SEEK_END);//��������   **********************wenti******************
                fread(&r, 1, 1, bmp->loc_of_file);
                fread(&g, 1, 1, bmp->loc_of_file);
                fread(&b, 1, 1, bmp->loc_of_file);
                *(output->bmp + (tempw + temph * output->width)) = ((int)(r + g + b-1) / 3);
            }
        }
    }
    return output;
}

QBMP* qbmpin(const char* loc) {
    INFO bmp = openbmp(loc);
    if (bmp.headfile.bfSize == 0) {
        return 0;
    }
    QBMP* ender = qbmpmalloc(&bmp);
    fclose(bmp.loc_of_file);
    return (ender);
}

QBMP* gbmpin(const char* loc) {
    INFO bmp = openbmp(loc);
    if (bmp.headfile.bfSize == 0) {
        return 0;
    }
    QBMP* ender = gbmpmalloc(&bmp);
    fclose(bmp.loc_of_file);
    return (ender);
}

//save a qbmp file in location as xxx.bmp with rgba
//��qbmp���浽location��ַ��xxx.bmp����rgbaͨ��
void qbmpoutrgba(const char* location, QBMP* qbmp) {
    unsigned int    biSize = 40;                                 // �˽ṹ��Ĵ�С (14-17�ֽ�)
    long            biWidth = qbmp->width;                       // ͼ��Ŀ�  (18-21�ֽ�)
    long            biHeight = qbmp->hight;                      // ͼ��ĸ�  (22-25�ֽ�)
    unsigned short  biPlanes = 1;                                // ��ʾbmpͼƬ��ƽ��������Ȼ��ʾ��ֻ��һ��ƽ�棬���Ժ����1 (26-27�ֽ�)
    unsigned short  biBitCount = 32;                             // һ������ռ��λ����һ��Ϊ24   (28-29�ֽ�)
    unsigned int    biCompression = 0;                           // ˵��ͼ������ѹ�������ͣ�0Ϊ��ѹ���� (30-33�ֽ�)
    unsigned int    biSizeImage = biWidth * biHeight * 32;       // ����������ռ��С, ���ֵӦ�õ��������ļ�ͷ�ṹ��bfSize-bfOffBits (34-37�ֽ�)
    long            biXPelsPerMeter = 4096;                      // ˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0 (38-41�ֽ�)
    long            biYPelsPerMeter = 4096 * biHeight / biWidth; // ˵����ֱ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0 (42-45�ֽ�)
    unsigned int    biClrUsed = 0;                               // ˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ����������Ϊ0�Ļ�����˵��ʹ�����е�ɫ����� (46-49�ֽ�)
    unsigned int    biClrImportant = 0;                          // ˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ��(50-53�ֽ�)

    unsigned short bfType = 19778;           // 19778��������BM�ַ�������Ӧ��ʮ������Ϊ0x4d42,ʮ����Ϊ19778��������bmp��ʽ�ļ�
    unsigned int   bfSize = 54 + biSizeImage;// �ļ���С ���ֽ�Ϊ��λ(2-5�ֽ�)
    unsigned short bfReserved1 = 0;          // ��������������Ϊ0 (6-7�ֽ�)
    unsigned short bfReserved2 = 0;          // ��������������Ϊ0 (8-9�ֽ�)
    unsigned int   bfOffBits = 54;           // ���ļ�ͷ���������ݵ�ƫ��  (10-13�ֽ�)

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

    int t1 = biHeight - 1, t0;
    for (; t1 > -1; t1--) {
        for (t0 = 0; t0 < biWidth; t0++) {
            fwrite(qbmp->bmp + 4 * (t0 + t1 * biWidth), 1, 1, file);
            fwrite(qbmp->bmp + 1 + 4 * (t0 + t1 * biWidth), 1, 1, file);
            fwrite(qbmp->bmp + 2 + 4 * (t0 + t1 * biWidth), 1, 1, file);
            fwrite(qbmp->bmp + 3 + 4 * (t0 + t1 * biWidth), 1, 1, file);
        }
    }
    fclose(file);
}

//# causion # the function will replace the original qbmp file
//#ע��#�˺������滻����QBMP���ڴ�
//regulate a qbmp file to an ideal size
//useage: QBMP* new_qbmp_file=regulate(old qbmp location , target width, target hight);
//��qbmp��С��ʽ��
//�÷���QBMP* ��qbmp�ڴ�=regulate����ת����qbmp��ַ��Ŀ���ȣ�Ŀ��߶ȣ�;
void regulate(QBMP* old, unsigned int width, unsigned int hight) {
    QBMP* output = (QBMP*)malloc(sizeof(QBMP));
    output->hight = hight-1;
    output->width = width-1;
    output->type = old->type;
    if (output->type == 1) {
        output->bmp = (unsigned char*)malloc(hight * width * 3 * sizeof(unsigned char));
    }
    else if (output->type == 2) {
        output->bmp = (unsigned char*)malloc(hight * width * 4 * sizeof(unsigned char));
    }
    else if (output->type == 0) {
        output->bmp = (unsigned char*)malloc(hight * width * sizeof(unsigned char));
    }
    else return;
    double h, w, crossh, crossw;
    unsigned int temp0 = 0, temp1;
    for (; temp0 < hight; temp0++) {
        h = (double)temp0 / (double)hight;
        for (temp1 = 0; temp1 < width; temp1++) {
            w = (double)temp1 / (double)width;
            crossw = (w * old->width) - (int)(w * old->width);
            crossh = (h * old->hight) - (int)(h * old->hight);
            if (output->type == 1) {//rgb
                *(output->bmp + 3 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 3 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 3 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 3 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 3 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//r
                *(output->bmp + 1 + 3 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 1 + 3 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 1 + 3 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 1 + 3 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 1 + 3 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//g
                *(output->bmp + 2 + 3 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 2 + 3 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 2 + 3 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 2 + 3 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 2 + 3 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//b

            }
            else if (output->type == 2) {
                *(output->bmp + 4 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 4 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 4 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 4 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 4 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//r
                *(output->bmp + 1 + 4 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 1 + 4 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 1 + 4 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 1 + 4 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 1 + 4 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//g
                *(output->bmp + 2 + 4 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 2 + 4 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 2 + 4 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 2 + 4 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 2 + 4 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//b
                *(output->bmp + 3 + 4 * (temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + 3 + 4 * ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + 3 + 4 * ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + 3 + 4 * ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + 3 + 4 * ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//b

            }
            else if (output->type == 0) {
                *(output->bmp +(temp1 + temp0 * output->width)) = (unsigned int)((1. - crossw) * (1 - crossh) * *(old->bmp + ((int)(w * old->width) + (old->width * (int)(h * old->hight)))) + crossh * (1 - crossw) * (*(old->bmp + ((int)(w * old->width) + old->width * (1 + (int)(h * old->hight))))) + crossw * (1 - crossh) * (*(old->bmp + ((int)(w * old->width) + 1 + old->width * ((int)(h * old->hight))))) + crossh * crossw * (*(old->bmp + ((int)(w * old->width) + 1 + old->width * (1 + (int)(h * old->hight))))));//r

            }
        }
    }

    //replace;
    old->hight = output->hight;
    old->width = output->width;
    free(old->bmp);
    old->bmp = output->bmp;
    free(output);
    return;
}

//************��װ***************
//����ӵĹ��ܣ�ĳ������ȣ��Ż�getred�Ⱥ�����ֱ�ӽ���xyֵ�������ƶ���ʱָ�룬��Ҫ�����˷��������²���Ҫ�ݹ�
//              ����ͼƬ�ϳ�һ�Ż�Ӱ̹�˵�BMP�汾(̫���ˣ����ڣ�)��


//                   |
//                   |
//shit mountain X 1 vvv
//                   v
//first one is the photo shown in black background while the second is the white background
// remember to free this BMP file, using freebmp()
//��һ���Ǻڱ���ͼ���ڶ����ǰױ���ͼ
//�ǵ���freebmp()�ͷ��ڴ�
QBMP* qmirage_tank(QBMP* bmpb, QBMP* bmpw) {
    /*
    1�Ź�ʽ���ϰ�ɫ���ǰ׵ף��°��Ǻڵ�
    128+cw/2=ca+256(1-a)(1)
    cb/2=ca

    �⣺
    a=128+(cb-cw)/2(256)
    c=256cb/(256+cb-cw)

    ��ͨ������alpha������ѡa��rgb��������ɫ�������Ϊalpha�Ļ�׼
    */
    unsigned char a;
    unsigned char c;
    QBMP* output = (QBMP*)malloc(sizeof(QBMP));
    output->type = 2;
    unsigned char bsizec, wsizec;
    int ctempr, ctempg, ctempb;//�������תint

    //��ʽ��ʼ��
    if (bmpb->type == 0) {
        bsizec = 1;
    }
    else if (bmpb->type == 1) {
        bsizec = 3;
    }
    else if (bmpb->type == 2) {
        bsizec = 4;
    }
    if (bmpw->type == 0) {
        wsizec = 1;
    }
    else if (bmpw->type == 1) {
        wsizec = 3;
    }
    else if (bmpw->type == 2) {
        wsizec = 4;
    }

    if (bmpw->width >= bmpb->width) {//w is larger
        output->width = bmpw->width;
        output->hight = bmpw->hight;
    }
    else {
        output->width = bmpb->width;
        output->hight = bmpb->hight;
    }

    //�㷨                                              **********************ac����,��regulate�ع�**********************
    //                ò��b����ɫ�Ƿ���
    if (bsizec == 1) {
        if (wsizec == 1) {//�ڰ�&&�ڰ�
            if (bmpw->width >= bmpb->width) {//w is larger
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpb->width * (float)temp1 / output->width) - (bmpb->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpb->hight * (float)temp0 / output->hight) - (bmpb->hight * (int)temp0 / output->hight);
                        //alpha
                        ctempr = (1 - soothx - soothy) * (*(bmpb->bmp + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����r
                        a = (unsigned char)(128 + (ctempr - *(bmpw->bmp + temp1 + temp0 * bmpw->width)) / 2);
                        *(output->bmp+3 +4*( temp1 + temp0 * output->width)) = a;
                        c = (256 * ctempr) / (256 + ctempr - *(bmpw->bmp + 2 + temp1 + temp0 * bmpw->width));//b max
                        *(output->bmp + 4 * (temp1 + temp0 * output->width)) = c;
                        *(output->bmp +1+ 4 * (temp1 + temp0 * output->width)) = c;
                        *(output->bmp +2+ 4 * (temp1 + temp0 * output->width)) = c;
                    }
                }
            }
            else {
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpw->width * (float)temp1 / output->width) - (bmpw->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpw->hight * (float)temp0 / output->hight) - (bmpw->hight * (int)temp0 / output->hight);
                        //alpha
                        ctempr = (1 - soothx - soothy) * (*(bmpw->bmp + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����r
                        a = (unsigned char)(128 + (*(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr) / 2);
                        *(output->bmp + 4*(3+temp1 + temp0 * output->width)) = a;
                        c = (256 * *(bmpw->bmp + temp1 + temp0 * bmpw->width)) / (256 + *(bmpw->bmp + temp1 + temp0 * bmpw->width) - ctempr);//b max
                        *(output->bmp + 3 +4*( temp1 + temp0 * output->width)) = c;
                    }
                }
            }
        }
        else{//b=�ڰף�w=rgb/rgba
            if (bmpw->width >= bmpb->width) {//w is larger
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpb->width * (float)temp1 / output->width) - (bmpb->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpb->hight * (float)temp0 / output->hight) - (bmpb->hight * (int)temp0 / output->hight);
                        //r
                        ctempr = (1 - soothx - soothy) * (*(bmpb->bmp + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����r
                        a = (unsigned char)(128 + (ctempr - *(bmpw->bmp + temp1 + temp0 * bmpw->width)) / 2);
                        *(output->bmp +4*( temp1 + temp0 * output->width)) = a;
                        //g
                        a = (unsigned char)(128 + (ctempr - *(bmpw->bmp + 1 + temp1 + temp0 * bmpw->width)) / 2);
                        *(output->bmp + 1 +4*( temp1 + temp0 * output->width)) = a;
                        //b
                        a = (unsigned char)(128 + (ctempr - *(bmpw->bmp + 2 + temp1 + temp0 * bmpw->width)) / 2);
                        *(output->bmp + 2 +4*( temp1 + temp0 * output->width)) = a;

                        if (*(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + ctempr - *(bmpw->bmp + temp1 + temp0 * bmpw->width));//r max
                        }
                        else if (*(output->bmp + temp1 + temp0 * output->width) < *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + 1 + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + ctempr - *(bmpw->bmp + 1 + temp1 + temp0 * bmpw->width));//g max
                        }
                        else {
                            c = (256 * ctempr) / (256 + ctempr - *(bmpw->bmp + 2 + temp1 + temp0 * bmpw->width));//b max
                        }
                        *(output->bmp + 3 +4*( temp1 + temp0 * output->width)) = c;
                    }
                }
            }
            else {
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpw->width * (float)temp1 / output->width) - (bmpw->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpw->hight * (float)temp0 / output->hight) - (bmpw->hight * (int)temp0 / output->hight);
                        //r
                        ctempr = (1 - soothx - soothy) * (*(bmpw->bmp + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����r
                        a = (unsigned char)(128 + (*(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr) / 2);
                        *(output->bmp + 4*(temp1 + temp0 * output->width)) = a;
                        //g
                        ctempg = (1 - soothx - soothy) * (*(bmpw->bmp + 1 + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + 1 + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + 1 + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����g
                        a = (unsigned char)(128 + (*(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempg) / 2);
                        *(output->bmp + 1 + 4*(temp1 + temp0 * output->width)) = a;
                        //b
                        ctempb = (1 - soothx - soothy) * (*(bmpw->bmp + 2 + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + 2 + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + 2 + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����g
                        a = (unsigned char)(128 + (*(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempb) / 2);
                        *(output->bmp + 2 + 4*(temp1 + temp0 * output->width)) = a;

                        if (*(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c =(256 * ctempr) / (256 + *(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr);//r max
                        }
                        else if (*(output->bmp + temp1 + temp0 * output->width) < *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + 1 + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempg) / (256 + *(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempg);//g max
                        }
                        else {
                            c = (256 * ctempb) / (256 + *(bmpw->bmp + temp1 + temp0 * bmpw->width) - ctempb);//b max
                        }
                        *(output->bmp + 3 +4*( temp1 + temp0 * output->width)) = c;
                    }
                }
            }
        }
    }
    else {
        if (wsizec == 1) {//rgba/rgb&&�ڰ�
            if (bmpw->width >= bmpb->width) {//w is larger
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpb->width * (float)temp1 / output->width) - (bmpb->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpb->hight * (float)temp0 / output->hight) - (bmpb->hight * (int)temp0 / output->hight);
                        //r
                        ctempr = (1 - soothx - soothy) * (*(bmpb->bmp + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����r
                        a = (unsigned char)(128 + (ctempr - *(bmpw->bmp + temp1 + temp0 * bmpw->width)) / 2);//modify0
                        *(output->bmp +4*( temp1 + temp0 * output->width)) = a;
                        //g
                        ctempg = (1 - soothx - soothy) * (*(bmpb->bmp + 1 + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + 1 + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + 1 + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����g
                        a = (unsigned char)(128 + (ctempg - *(bmpw->bmp + temp1 + temp0 * bmpw->width)) / 2);
                        *(output->bmp + 1 + 4*(temp1 + temp0 * output->width)) = a;
                        //b
                        ctempb = (1 - soothx - soothy) * (*(bmpb->bmp + 2 + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + 2 + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + 2 + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����g
                        a = (unsigned char)(128 + (ctempb - *(bmpw->bmp + temp1 + temp0 * bmpw->width)) / 2);
                        *(output->bmp + 2 +4*( temp1 + temp0 * output->width)) = a;

                        if (*(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + ctempr - *(bmpw->bmp + temp1 + temp0 * bmpw->width));//r max modify0
                        }
                        else if (*(output->bmp + temp1 + temp0 * output->width) < *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + 1 + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempg) / (256 + ctempg - *(bmpw->bmp + temp1 + temp0 * bmpw->width));//g max
                        }
                        else {
                            c = (256 * ctempb) / (256 + ctempb - *(bmpw->bmp + temp1 + temp0 * bmpw->width));//b max
                        }
                        *(output->bmp + 3 +4*( temp1 + temp0 * output->width)) = c;
                    }
                }
            }
            else {
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpw->width * (float)temp1 / output->width) - (bmpw->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpw->hight * (float)temp0 / output->hight) - (bmpw->hight * (int)temp0 / output->hight);
                        //r
                        ctempr = (1 - soothx - soothy) * (*(bmpw->bmp + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����r
                        a = (unsigned char)(128 + (*(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr) / 2);
                        *(output->bmp + 4*(temp1 + temp0 * output->width)) = a;
                        //g
                        a = (unsigned char)(128 + (*(bmpb->bmp + 1 + temp1 + temp0 * bmpb->width) - ctempr) / 2);
                        *(output->bmp + 1 +4*( temp1 + temp0 * output->width)) = a;
                        //b
                        a = (unsigned char)(128 + (*(bmpb->bmp + 2 + temp1 + temp0 * bmpb->width) - ctempr) / 2);
                        *(output->bmp + 2 + 4*(temp1 + temp0 * output->width)) = a;

                        if (*(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + *(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr);//r max
                        }
                        else if (*(output->bmp + temp1 + temp0 * output->width) < *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + 1 + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + *(bmpb->bmp + 1 + temp1 + temp0 * bmpb->width) - ctempr);//g max
                        }
                        else {
                            c = (256 * ctempr) / (256 + *(bmpw->bmp + 2 + temp1 + temp0 * bmpw->width) - ctempr);//b max
                        }
                        *(output->bmp + 3 +4*( temp1 + temp0 * output->width)) = c;
                    }
                }
            }
        }
        else {//b=rgba/rgb��w=rgb/rgba
            if (bmpw->width >= bmpb->width) {//w is larger
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpb->width * (float)temp1 / output->width) - (bmpb->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpb->hight * (float)temp0 / output->hight) - (bmpb->hight * (int)temp0 / output->hight);
                        //r
                        ctempr = (1 - soothx - soothy) * (*(bmpb->bmp + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����r
                        a = (unsigned char)(128 + (ctempr - *(bmpw->bmp +wsizec*( temp1 + temp0 * bmpw->width))) / 2);
                        *(output->bmp + 4*(temp1 + temp0 * output->width)) = a;
                        //g
                        ctempg = (1 - soothx - soothy) * (*(bmpb->bmp + 1 + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + 1 + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + 1 + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����g
                        a = (unsigned char)(128 + (ctempg - *(bmpw->bmp + 1 + wsizec*(temp1 + temp0 * bmpw->width))) / 2);
                        *(output->bmp + 1 +4*( temp1 + temp0 * output->width)) = a;
                        //b
                        ctempb = (1 - soothx - soothy) * (*(bmpb->bmp + 2 + bsizec * (temp1 + temp0 * bmpb->width))) + soothx * (*(bmpb->bmp + 2 + bsizec * (1 + temp1 + temp0 * bmpb->width))) + soothy * (*(bmpb->bmp + 2 + bsizec * (temp1 + (1 + temp0) * bmpb->width)));//�����g
                        a = (unsigned char)(128 + (ctempb - *(bmpw->bmp + 2 + wsizec*(temp1 + temp0 * bmpw->width))) / 2);
                        *(output->bmp + 2 +4*( temp1 + temp0 * output->width)) = a;

                        if (*(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + ctempr - *(bmpw->bmp + wsizec*(temp1 + temp0 * bmpw->width)));//r max
                        }
                        else if (*(output->bmp + temp1 + temp0 * output->width) < *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + 1 + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempg) / (256 + ctempg - *(bmpw->bmp + 1 + wsizec*(temp1 + temp0 * bmpw->width)));//g max
                        }
                        else {
                            c = (256 * ctempb) / (256 + ctempb - *(bmpw->bmp + 2 +wsizec*( temp1 + temp0 * bmpw->width)));//b max
                        }
                        *(output->bmp + 3 + 4*(temp1 + temp0 * output->width)) = c;
                    }
                }
            }
            else {
                output->bmp = (unsigned char*)malloc(4 * output->hight * output->width);
                int temp0 = 0, temp1;
                float soothx, soothy;
                for (; temp0 < output->hight; temp0++) {
                    for (temp1 = 0; temp1 < output->width; temp1++) {
                        soothx = (bmpw->width * (float)temp1 / output->width) - (bmpw->width * (int)temp1 / output->width);//c(x)=(1-soothx)now+soothx(now+1)   same as y
                        soothy = (bmpw->hight * (float)temp0 / output->hight) - (bmpw->hight * (int)temp0 / output->hight);
                        //r
                        ctempr = (1 - soothx - soothy) * (*(bmpw->bmp + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����r
                        a = (unsigned char)(128 + (*(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr) / 2);
                        *(output->bmp + 4*(temp1 + temp0 * output->width)) = a;
                        //g
                        ctempg = (1 - soothx - soothy) * (*(bmpw->bmp + 1 + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + 1 + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + 1 + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����g
                        a = (unsigned char)(128 + (*(bmpb->bmp + 1 + temp1 + temp0 * bmpb->width) - ctempg) / 2);
                        *(output->bmp + 1 + 4*(temp1 + temp0 * output->width)) = a;
                        //b
                        ctempb = (1 - soothx - soothy) * (*(bmpw->bmp + 2 + bsizec * (temp1 + temp0 * bmpw->width))) + soothx * (*(bmpw->bmp + 2 + bsizec * (1 + temp1 + temp0 * bmpw->width))) + soothy * (*(bmpw->bmp + 2 + bsizec * (temp1 + (1 + temp0) * bmpw->width)));//�����g
                        a = (unsigned char)(128 + (*(bmpb->bmp + 2 + temp1 + temp0 * bmpb->width) - ctempb) / 2);
                        *(output->bmp + 2 + 4*(temp1 + temp0 * output->width)) = a;

                        if (*(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempr) / (256 + *(bmpb->bmp + temp1 + temp0 * bmpb->width) - ctempr);//r max
                        }
                        else if (*(output->bmp + temp1 + temp0 * output->width) < *(output->bmp + 1 + temp1 + temp0 * output->width) && *(output->bmp + 1 + temp1 + temp0 * output->width) > *(output->bmp + 2 + temp1 + temp0 * output->width)) {
                            c = (256 * ctempg) / (256 + *(bmpb->bmp + 1 + temp1 + temp0 * bmpb->width) - ctempg);//g max
                        }
                        else {
                            c = (256 * ctempb) / (256 + *(bmpw->bmp + 2 + temp1 + temp0 * bmpw->width) - ctempb);//b max
                        }
                        *(output->bmp + 3 +4*( temp1 + temp0 * output->width)) = c;
                    }
                }
            }
        }
    }

    return output;
}



#undef _NO_BMP_FILE
#undef MAX_STR_LEN_OF_LOC