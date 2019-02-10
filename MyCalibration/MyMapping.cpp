#include "stdafx.h"
#include "MyMapping.h"


int MyMapping::StartMapping(int num)
{
	Cam_Num = num;
	cout << "Press a to choose the image for calulating." << endl;
	vector<VideoCapture> captures;

	for (int i = 0; i < num; i++)
	{
		VideoCapture cap(i);
		captures.push_back(cap);
		cout << "Capture camera no." << i << endl;
		waitKey(50);
	}
	for (int i = 0; i < num; i++)
	{
		Mat frame;

		while (1)
		{
			captures[i] >> frame;
			imshow(win_name, frame);
			int key = waitKey(30);
			if (key == 'a')
			{
				destroyWindow(win_name);
				break;
			}
		}

		if (SphericalMappingParam::CalculateParam(frame) < 0)
		{
			cout << "Error: MyMapping::StartMapping" << endl;
			return -1;
		}

		Circle c;
		SphericalMappingParam::getCircleParatemer(c.center, c.radius);
		circles.push_back(c);
	}
	Point3d direction = Point3d(0, 0, 1);
	int FOV = 45;
	CalculateMap(direction, FOV);
	int angleX = 90;
	int angleY = 90;
	int step = 5;
	while (1)
	{
		Mat frame;
		captures[0] >> frame;
		remap(frame, frame, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT);
		resize(frame, frame, Size(), 3.0, 3.0);
		imshow(win_name, frame);
		int key = waitKey(40);
		switch (key)
		{
		case 'a':
			angleX += step;
			if (angleX % 90 == 0)
				direction.x = 0;
			else
				direction.x = 1 / tan(CV_PI*angleX / 180.0);
			CalculateMap(direction, FOV);
			break;
		case 'd':
			angleX -= step;
			if (angleX % 90 == 0)
				direction.x = 0;
			else
				direction.x = 1 / tan(CV_PI*angleX / 180.0);
			CalculateMap(direction, FOV);
			break;
		case 'w':
			angleY += step;
			if (angleY % 90 == 0)
				direction.y = 0;
			else
				direction.y = 1 / tan(CV_PI*angleY / 180.0);
			CalculateMap(direction, FOV);
			break;
		case 's':
			angleY -= step;
			if (angleY % 90 == 0)
				direction.y = 0;
			else
				direction.y = 1 / tan(CV_PI*angleY / 180.0);
			CalculateMap(direction, FOV);
			break;
		case 'z':
			angleX = 90;
			angleY = 90;
			direction = Point3d(0, 0, 1);
			CalculateMap(direction, FOV);
			break;
		}
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
	double edge = CalculateEdge(radius, FOV);
	int d = circles[cam].radius * 2;

	//方向与XY轴的夹角弧度
	double radianX = asin(direction.x / direction.z);
	double radianY = asin(direction.y / direction.z);
	double h = edge / 2 / tan(FOV / 2.0 / 180.0 * CV_PI);	//球心到成像面的距离

	double check = sqrt(pow(radius, 2) - pow(edge, 2) / 2);
	result.create(Size(int(edge), int(edge)), CV_32FC1);

	//create mapping map
	map_x.create(result.size(), CV_32FC1);
	map_y.create(result.size(), CV_32FC1);
	for (int j = 0; j < int(edge); j++)
	{
		for (int i = 0; i < int(edge); i++)
		{
			double tempX = atan((edge / 2.0 - i) / h) + radianX;	//弧度
			double tempY = atan((edge / 2.0 - j) / h) + radianY;	//弧度
			map_x.at<float>(i, j) = static_cast<float>(center.x + radius * sin(tempX));
			map_y.at<float>(i, j) = static_cast<float>(center.y - radius * sin(tempY));
		}
	}

	return 0;
}

double MyMapping::CalculateEdge(int R, int FOV)
{
	//horizontal and vertical FOV are same here
	//return int(R * sqrt(0.5 + 1 / (4 * pow(tan(FOV / 2 / 180 * CV_PI), 2))));
	double result = -1;
	result = R / sqrt(0.5 + 1 / (4 * pow(tan(FOV / 2.0 / 180.0 * CV_PI), 2)));
	return result;
}