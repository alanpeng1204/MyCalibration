#pragma once
#include "SphericalMappingParam.h"
#include "GlobalDefine.h"

class MyMapping
{
public:
	int StartMapping(int num = 2);

private:
	int CalculateMap(Point3d direction, int FOV = 90);
	int CalculateEdge(int R, int FOV);
	vector<Circle> circles;
	string win_name = "My Mapping";
	int Cam_Num = 0;
	Mat map_x, map_y;
};

