#pragma once
#include "MyImage.h"
#define MAX_CLUSTER 10

void Binarization(const CDoubleImage& imageIn, CByteImage& imageOut, int startH, int endH, int startS, int endS, int startV, int endV);
void Binarization_For_Red(const CDoubleImage& imageIn, CByteImage& imageOut, int startH, int endH, int startS, int endS, int startV, int endV);