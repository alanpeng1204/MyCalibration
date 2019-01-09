#pragma once
#include "GlobalDefine.h"

#define PI 3.1415926

class SphericalMappingParam
{
public:
	SphericalMappingParam() {};
	static int CalculateParam(Mat img);
	static bool getCircleParatemer(Point2i& c, int& r);
	static void checkVarify();

	static const double FOV;
private:
	static void On_N_Change(int N_value, void* param);
	static void On_Threshold_Change(int Threshold_value, void* parmas); 
	static void revisedScanLineMethod(Mat imgOrg, Point2i& center, int& radius, int threshold, int N);
	static bool CircleFitByKasa(vector<Point> validPoints, Point& center, int&	 radius);

	static int radius;
	static Point2i center;
	static Mat image;
	static int _width;
	static int _height;
	static string N_trackbar_name;
	static int N_max_value;
	static int N_value;

	static string Threshold_trackbar_name;
	static int Threshold_max_value;
	static int Threshold_value;

	static string win_name;
};

