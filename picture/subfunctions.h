#ifndef SUBFUNCTIONS
#define SUBFUNCTIONS

#include <iosfwd>
#include <fstream>
#include <vector>
#include "bmp.h"

bool writeFileHeaders(std::ofstream& newFile, BITMAPFILEHEADER& bH, BITMAPINFOHEADER& iH);
bool writeFile(std::ofstream& newFile, int width, int height, std::vector<std::vector<ARGBQUAD>>& pixelData);
bool readFileHeaders(std::ifstream& targetPicture, BITMAPFILEHEADER& bH, BITMAPINFOHEADER& iH);
bool readFile(std::ifstream& targetPicture, int startPoint, int bitRate, int width, int height, std::vector<std::vector<ARGBQUAD>>& pixelData);
void transcribeBinary(std::ifstream& targetPicture, std::ofstream& newFile, std::vector<unsigned char>& buffer, bool debugMode);
bool openFiles(std::ifstream& targetPicture, std::string targetPicturePath, std::ofstream& newFile, std::string newFilePath, std::string operationType);

#endif