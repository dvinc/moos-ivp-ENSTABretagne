/***************************************************************************************************************:')

CvDraw.c

Drawing functions for IplImage.
2 types of IplImage format should be used in the functions :
_ a default RGB color format than can be created using CreateDefaultColorCvImg() or cvLoadImage("file.ppm", 1).
_ a default gray format than can be created using CreateDefaultGrayCvImg() or cvLoadImage("file.pgm", 0).
Note that you can use most of the OpenCV functions with such IplImage formats.

Fabrice Le Bars

Created : 2009-03-27

Version status : To test and validate

***************************************************************************************************************:)*/

#include "CvDraw.h"
