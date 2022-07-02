#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>
#include "subfunctions.h"
#include "bmp.h"

using namespace std;

bool writeFileHeaders(ofstream& newFile, BITMAPFILEHEADER& bH, BITMAPINFOHEADER& iH) {
    
    // Converts 24-bit images to 32-bit images. The size of the image needs to be modified.
    // Note that this has to be done after readFile, hence why it's here instead of readFileHeaders.

    if (iH.biBitCount == 24) {
        iH.biBitCount = 32;
        iH.biSizeImage = (iH.biSizeImage * 4) / 3;
        bH.bfSize = iH.biSizeImage + bH.bfOffBits;
    }

    newFile.write((char*)&bH, sizeof(BITMAPFILEHEADER));
    newFile.write((char*)&iH, sizeof(BITMAPINFOHEADER));
    
    int currPos = newFile.tellp();
    while (currPos < bH.bfOffBits) {
        newFile.write(0x00, sizeof(char));
    }
    return true;
}

bool writeFile(ofstream& newFile, int width, int height, vector<vector<ARGBQUAD>>& pixelData) {
    for (int h=0; h<height; h++) {
        for (int w=0; w<width; w++) {
            newFile.write((char*)&pixelData[h][w], sizeof(ARGBQUAD));
        }
    }
    return true;

}

bool readFileHeaders(ifstream& targetPicture, BITMAPFILEHEADER& bH, BITMAPINFOHEADER& iH) {
    // https://stackoverflow.com/questions/22192254/reading-binary-data-into-struct-with-ifstream
    targetPicture.read((char*)&bH, sizeof(BITMAPFILEHEADER));
    targetPicture.read((char*)&iH, sizeof(BITMAPINFOHEADER));
    if (bH.bfType != 0x4D42) {cout << "Target file is not a .bmp file!\n"; return false;}
    if (iH.biBitCount != 32 && iH.biBitCount != 24) {cout << "Target file's bitrate is not supported: must be 24 or 32 bits\n"; return false;}
    
    return true;
}

bool readFile(ifstream& targetPicture, int startPoint, int bitRate, int width, int height, vector<vector<ARGBQUAD>>& pixelData) {
    targetPicture.seekg(startPoint);
    
    // reads data from targetPicture into pixelData
    vector<ARGBQUAD> pixels(width);
    // padding for 24-bit pictures
    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    for (int h=0; h<height; h++) {
        // reads a single row

        if (bitRate == 32) {
            targetPicture.read((char*)&pixels[0], sizeof(ARGBQUAD) * width);
            pixelData.emplace_back(pixels);

        } else if (bitRate == 24) {
            // Converts the image to 32-bit pixels.
            // reads a single pixel, and sets the rgbtAlpha value to 0xFF.
            for (int w=0; w<width; w++) {
                ARGBQUAD pixel;
                targetPicture.read((char*)&pixel, sizeof(RGBTRIPLE));
                pixel.rgbtAlpha = 0xFF;
                pixels[w] = pixel;
            }
            // joins all the pixels together, and increments the file read pointer to account for padding.
            pixelData.emplace_back(pixels);
            int currPos = targetPicture.tellg();
            targetPicture.seekg(currPos + padding);
        }
    }

    return true;
}

void transcribeBinary(ifstream& targetPicture, ofstream& newFile, vector<unsigned char>& buffer, bool debugMode) {

    stringstream ss; 
    if (debugMode == false) {

        cout << "Transcribing without debug parameters;\n";

        for (long i=0; i<buffer.size(); i++) {
            ss << hex << uppercase << setfill('0') << setw(2) << (int)buffer[i] << " ";
            newFile << ss.rdbuf();
        }

        // Note: it's also possible to just do newFile << targetPicture.rdbuf().

        // rdbuf() returns a pointer to the stream buffer associated with the object.
        // A "stream buffer" is an IO device allowing read/write for raw data (bytes). A "stream" is an IO device for processed data.
        // The stream parses the data from the stream buffer.

    } else {
        long i = 0; long dipHeaderPos = 9999, byteArrPos = 9999; int bitrate;

        cout << "Transcribing in debug mode;\n";

        while (i < buffer.size()) {
            ss << hex << uppercase << setfill('0') << setw(2) << (int)buffer[i] << " ";
            switch(i) {
                case 1:
                    ss << string(11, ' ') << "BM\n"; break;
                case 5:
                    ss << string(5, ' ') << "Size of BMP file: " << (int)buffer[5] << (int)buffer[4] << (int)buffer[3] << (int)buffer[2]
                        << " = " << dec << (long)(buffer[5]*16777216 + buffer[4]*65536 + buffer[3]*256 + buffer[2]) << " bytes\n"; break;
                case 7: case 9:
                    ss << string(11, ' ') << "Unused\n"; break;
                case 13:
                    byteArrPos = buffer[13]*16777216 + buffer[12]*65536 + buffer[11]*256 + buffer[10];
                    ss << string(5, ' ') << "Offset where the pixel array can be found: " << dec << byteArrPos << " bytes\n";
                    ss << string(20, ' ') << "DIP HEADER\n"; break;
                case 17:
                    ss << string(5, ' ') << "Number of bytes in the DIP header (from this point): " << dec
                        << (long)(buffer[17]*16777216 + buffer[16]*65536 + buffer[15]*256 + buffer[14]) << " bytes\n"; break;
                case 21:
                    ss << string(5, ' ') << "Width in pixels: " << (int)buffer[21] << (int)buffer[20] << (int)buffer[19] << (int)buffer[18]
                        << " = " << dec << (long)(buffer[21]*16777216 + buffer[20]*65536 + buffer[19]*256 + buffer[18]) << "\n"; break;
                case 25:
                    ss << string(5, ' ') << "Height in pixels: " << (int)buffer[25] << (int)buffer[24] << (int)buffer[23] << (int)buffer[22]
                        << " = " << dec << (long)(buffer[25]*16777216 + buffer[24]*65536 + buffer[23]*256 + buffer[22]) << "\n"; break;
                case 27:
                    ss << string(11, ' ') << "No. of colour planes being used: " << dec << (int)(buffer[27]*256 + buffer[26]) << "\n"; break;
                case 29:
                    bitrate = (int)(buffer[29]*256 + buffer[28]);
                    ss << string(11, ' ') << "No. of bits per pixel: " << dec << bitrate << "\n"; break;
                case 33:
                    ss << string(5, ' ') << "Compression type being used: #" << dec << (int)buffer[30] << "\n"; break;
                case 37:
                    ss << string(5, ' ') << "Raw size of BMP file: " << (int)buffer[37] << (int)buffer[36] << (int)buffer[35] << (int)buffer[34]
                        << " = " << dec << (long)(buffer[37]*16777216 + buffer[36]*65536 + buffer[35]*256 + buffer[34]) << " bytes\n"
                        << string(16, ' ') << "Note that a dummy 0 can be given for BI_RGB (compression type 0) images.\n"; break;
            }
            if (i == byteArrPos - 1) {
                ss << "\n" << string(20, ' ') << "START OF BYTE ARRAY\n";
            }
            i++;
        }
        
        newFile << ss.rdbuf();
    }

}

bool openFiles(ifstream& targetPicture, string targetPicturePath, ofstream& newFile, string newFilePath, string operationType) {
    targetPicture.open(targetPicturePath, ios::binary);
    if (targetPicture.is_open() == false) {cout << "Failed to open target picture!\n"; return false;}
    if (operationType == "t" || operationType == "p") {
        newFile.open(newFilePath, ios::binary | ios::in | ios::app);
        // ios::app is necessary to create the file if it doesn't exist yet.
        // https://stackoverflow.com/questions/23967697/if-file-exist-work-with-it-if-no-create-it
    } else {
        cout << "Invalid operation type!\n"; return false;
    }
    if (newFile.is_open() == false) {cout << "Failed to open new file!\n"; return false;}
    return true;
}



    // https://stackoverflow.com/questions/5420317/reading-and-writing-binary-file explains that:

        // TYPE STD::CHAR is equivalent to TYPE STD::BYTE, and has been in use for C/C++ for over 40 years. In fact, the latter was
        // only introduced into the language in 2017. 
        // You can use a std::vector<unsigned char> as an array (vector) of bytes to store raw data.

        // One way of constructing vectors is using std::vector<type> name(ITERATOR AT FIRST, ITERATOR AFTER LAST). This particular
        // constructor automatically reads the data from FIRST to LAST into the vector.

    // https://stackoverflow.com/questions/27406789/confused-about-usage-of-stdistreambuf-iterator and 
    // https://stackoverflow.com/questions/26824281/the-difference-betwen-istreambuf-iterator-and-istream-iterator explain that:

        // std::istreambuf_iterator<type>(source) is an iterator for a stream buffer object. This means that data extraction is unformatted.
        // std::istream_iterator, on the other hand, provides formatted extraction.

        // std::copy(ITERATOR AT FIRST, ITERATOR AFTER LAST, ITERATOR AT DESTINATION FIRST).
        // ITERATOR AT FIRST is given by std::istreambuf_iterator(file stream). When the iterator reaches the end of the stream, it changes
        // into an "end-of-stream iterator" (an iterator which basically says "I'm done here, you can discard me now"). Turns out, default-
        // constructed stream iterators are also "end-of-stream iterator"s.
        // With std::istreambuf_iterator<type>() i.e. leave the source empty, you can construct an end-of-stream iterator, and so tell the
        // std::copy to stop when the actual iterator becomes equal to the end-of-stream iterator.
        
        // https://www.geeksforgeeks.org/ways-copy-vector-c/
        // vectorname.assign(ITERATOR AT FIRST, ITERATOR AFTER LAST).

        // https://stackoverflow.com/questions/5420317/reading-and-writing-binary-file
        // std::vector<type> name(std::istreambuf_iterator<type>(file stream), {}).
        // I guess {} is another way of setting ITERATOR AFTER LAST as an end-of-stream iterator.


    
        // https://itecnote.com/tecnote/c-how-to-convert-byte-array-to-hex-string-in-visual-c/
        // Sets the stringstream's output format to hex, then uppercase, then width to 2 (i.e. 2 characters) where
        // the fill character is '0'.
            // https://stackoverflow.com/questions/62703952/default-positioning-of-fill-characters-for-streams:
            // apparently the fill character always starts on the left.


    // IN SUMMARY, THREE WAYS TO COPY INTO VECTOR:
        // std::copy(ITERATOR AT FIRST, ITERATOR AFTER LAST, ITERATOR AT DESTINATION FIRST)
        // std::vector<type> name(ITERATOR AT FIRST, end-of-stream iterator)
        // vectorname.assign(ITERATOR AT FIRST, ITERATOR AFTER LAST)