#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <fstream>
#include <vector>

bool greyscale(long width, long height, std::vector<std::vector<ARGBQUAD>>& pixelData);
bool reflect(long width, long height, std::vector<std::vector<ARGBQUAD>>& pixelData);
bool clockwiseRotate(long width, long height, vector<vector<ARGBQUAD>>& pixelData);

#endif