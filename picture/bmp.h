#ifndef BMP_H
#define BMP_H

// BMP-related data types based on Microsoft's own

            // ACTUAL DATA TYPE SIZES: https://docs.microsoft.com/en-us/cpp/cpp/data-type-ranges?view=msvc-170

            // char, signed char: 1 byte
            // short, unsigned short: 2 bytes
            // int, unsigned int: 4 bytes
            // long, unsigned long: 4 bytes
            // long long, unsigned long long: 8 bytes

#include <stdint.h>

// Adapted from http://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx.

typedef struct
{
    unsigned short   bfType;
    unsigned long  bfSize;
    unsigned short   bfReserved1;
    unsigned short   bfReserved2;
    unsigned long  bfOffBits;
} __attribute__((__packed__))
BITMAPFILEHEADER;

 // Adapted from http://msdn.microsoft.com/en-us/library/dd183376(VS.85).aspx.

typedef struct
{
    unsigned long  biSize;
    long   biWidth;
    long   biHeight;
    unsigned short   biPlanes;
    unsigned short   biBitCount;
    unsigned long  biCompression;
    unsigned long  biSizeImage;
    long   biXPelsPerMeter;
    long   biYPelsPerMeter;
    unsigned long  biClrUsed;
    unsigned long  biClrImportant;
} __attribute__((__packed__))
BITMAPINFOHEADER;

// Adapted from http://msdn.microsoft.com/en-us/library/aa922590.aspx.

typedef struct
{
    char  rgbtBlue;
    char  rgbtGreen;
    char  rgbtRed;
} __attribute__((__packed__))
RGBTRIPLE;

typedef struct
{
    char  rgbtBlue;
    char  rgbtGreen;
    char  rgbtRed;
    char  rgbtAlpha;
} __attribute__((__packed__))
ARGBQUAD;

#endif