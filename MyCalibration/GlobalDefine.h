#pragma once
#include <cstdlib>
#include "opencv2/opencv.hpp"
#include <iostream>
#include <assert.h>

using namespace std;
using namespace cv;


#define PI 3.1415926
const double  LIMIT = 1e-4;

typedef enum
{
	PERSPECTIVE,
	LATITUDE_LONGTITUDE,
}distMapMode;

typedef enum
{
	STEREOGRAPHIC,
	EQUIDISTANCE,
	EQUISOLID,
	ORTHOGONAL
}camMode;


enum CorrectType
{
	Forward,
	//means correct the distorted image by mapping the pixels on the origin image
	//to the longitude-latitude rectified image, there may be some pixels on the
	//rectified image which have no corresponding origin pixel. 
	Reverse,
	//means correct the distorted image by reverse mapping, that is from the rectified 
	//image to the origin distorted image, this method can be sure for that every pixels
	//on the rectified image have its corresponding origin pixel.
};
