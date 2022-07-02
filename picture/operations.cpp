#include <iostream>
#include <fstream>
#include <vector>
#include "bmp.h"
#include "operations.h"

using namespace std;

            // g - greyscale

bool greyscale(long width, long height, vector<vector<ARGBQUAD>>& pixelData) {

    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            ARGBQUAD& pixel = pixelData[h][w];
            int r = pixel.rgbtRed, g = pixel.rgbtGreen, b = pixel.rgbtBlue;     
            char avg = ((r + g + b) / 3);
            pixel.rgbtRed = avg; 
            pixel.rgbtGreen = avg; 
            pixel.rgbtBlue = avg;
        }
    }
    cout << "Greyscale complete!\n";
    return true;
}

            // r - reflect horizontally

bool reflect(long width, long height, vector<vector<ARGBQUAD>>& pixelData) {

    int heightMid = (height-1) / 2;
    for (int h=0; h<heightMid; h++) {
        for (int w=0; w<width; w++) {
            ARGBQUAD pixel = pixelData[h][w];
            pixelData[h][w] = pixelData[height-1 - h][w];
            pixelData[height-1 - h][w] = pixel;
        }
    }
    cout << "Reflection complete!\n";
    return true;
}

            // c - clockwise rotation

bool clockwiseRotate(long width, long height, vector<vector<ARGBQUAD>>& pixelData) {

    

}

            // e - edges
            // b - blur