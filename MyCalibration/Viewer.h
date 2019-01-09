#pragma once
#include "GlobalDefine.h"

#define VIEWER_WIDTH 1024
#define VIEWER_HEIGHT VIEWER_WIDTH/2

class Viewer
{
public:
	Viewer(Mat image);
	Mat ConvertImage(Mat image);
	void SetParam(double aH, double aV);

private:
	string win_name;
	Mat result;
	double angleH;
	double angleV;
	double distance;

	Point3d navigationHV(Point3d orgPt, double aH, double aV);
};

