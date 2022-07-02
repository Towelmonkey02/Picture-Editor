#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include "subfunctions.h"
#include "operations.h"
#include "bmp.h"

using namespace std;

        // Operation types:
            // p - process picture
                // g - greyscale
                // r - reflect horizontally
                // c - clockwise rotation
                // e - edges
                // b - blur

            // t - transcribe existing picture to readable binary-hex data
                // debug option: d - formats binary-hex data text nicely so you can see where everything is

int main(int argc, char* argv[]) {
    if ((argc != 4) && (argc != 5)) {cout << "Correct usage: ./analyser [path of target picture] [path of new file] [operation type] [option]\n"; return 0;}

                // PROCESS USER INPUTS 

    string targetPicturePath = argv[1], newFilePath = argv[2], operationType = argv[3]; char option = '\0';
    if (argc == 5) {option = argv[4][0];}

                // OPEN THE FILES. (note that output is in append mode, so existing files will be appended to)

    ifstream targetPicture; ofstream newFile;
    if (not openFiles(targetPicture, targetPicturePath, newFile, newFilePath, operationType)) {return 0;}

                // OPERATION: BINARY TRANSCRIPTION

    if (operationType == "t") {
            // Initialise a buffer vector with the data of targetPicture inside it, in raw bytes.  
        vector<unsigned char> buffer(istreambuf_iterator<char>(targetPicture), {});
        if (option == 'd') {
            transcribeBinary(targetPicture, newFile, buffer, true);
        } else {;
            transcribeBinary(targetPicture, newFile, buffer, false);
        }
    } 

                // OPERATION: IMAGE PROCESSING

    else if (operationType == "p") {

        BITMAPFILEHEADER fH; BITMAPINFOHEADER iH;
        if (not readFileHeaders(targetPicture, fH, iH)) {return 0;}

        long width = iH.biWidth, height = iH.biHeight;
        vector<vector<ARGBQUAD>> pixelData;
        if (not readFile(targetPicture, (int)fH.bfOffBits, (int)iH.biBitCount, width, height, pixelData)) {return 0;}
        
        switch(option) {
            case 'g': greyscale(width, height, pixelData); break;
            case 'r': reflect(width, height, pixelData); break;
        }

        long width = iH.biWidth, height = iH.biHeight;
        writeFileHeaders(newFile, fH, iH);
        writeFile(newFile, width, height, pixelData);

    }


    targetPicture.close();
    newFile.close();
    return 1;
}

