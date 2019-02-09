#include "stdafx.h"
#include "MyMapping.h"


int MyMapping::StartMapping(int num)
{
	Cam_Num = num;
	cout << "Press a to choose the image for calulating." << endl;
	for (int i = 0; i < num; i++)
	{
		VideoCapture capture(i);
		Mat frame;
		capture >> frame;
		imshow(win_name, frame);
		int key = waitKey(30);
		if (key == 'a')
		{
			destroyWindow(win_name);
			break;
		}
		if (SphericalMappingParam::CalculateParam(frame) < 0)
		{
			cout << "Error: SphericalMapping::StartMapping" << endl;
			return -1;
		}

		Circle c;
		SphericalMappingParam::getCircleParatemer(c.center, c.radius);
		circles.push_back(c);
	}
	CalculateMap(Point3d(0, 0, 1), 90);
	while (1)
	{
		VideoCapture cap(0);
		Mat frame;
		cap >> frame;
		remap(frame, frame, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT);
		imshow("1", frame);
		waitKey(40);
	}
	return 0;
}

int MyMapping::CalculateMap(Point3d direction, int FOV)
{
	//x指向左方，y指向上方，z指向画面
	if (direction.x == direction.y == direction.z == 0)
		direction.z = 1;
	if (FOV == 0)
		FOV = 90;
	int cam = 0;

	Mat result;
	Point center = circles[cam].center;
	double radius = circles[cam].radius;
	int edge = CalculateEdge(radius, FOV);
	int d = circles[cam].radius * 2;

	//方向与XY轴的夹角弧度
	double radianX = asin(direction.x / direction.z);
	double radianY = asin(direction.y / direction.z);
	double h = edge / 2 / tan(FOV / 2.0 / 180.0 * CV_PI);	//球心到成像面的距离


	result.create(Size(edge, edge), CV_32FC1);

	//create mapping map
	map_x.create(result.size(), CV_32FC1);
	map_y.create(result.size(), CV_32FC1);
	for (int j = 0; j< edge - 1; j++)
	{
		for (int i = 0; i< edge - 1; i++)
		{
			double tempX = atan((i - edge / 2.0) / h) + radianX;
			double tempY = atan((j - edge / 2.0) / h) + radianY;
			map_x.at<float>(i, j) = static_cast<float>(center.x - radius * sin(tempX));
			map_y.at<float>(i, j) = static_cast<float>(center.y - radius * sin(tempY));
		}
	}

	return 0;
}

int MyMapping::CalculateEdge(int R, int FOV)
{
	//horizontal and vertical FOV are same here
	//return int(R * sqrt(0.5 + 1 / (4 * pow(tan(FOV / 2 / 180 * CV_PI), 2))));
	int result = -1;
	result = R * sqrt(0.5 + 1 / (4 * pow(tan(FOV / 2.0 / 180.0 * CV_PI), 2)));
	return result;
}