// MyCalibration.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//#include "pch.h"	//新版本用pch.h
#include "stdafx.h"	//旧版本用stdafx.h
#include "GlobalDefine.h"
#include "FisheyeCalibration.h"
#include "SphericalMapping.h"
#include "MyMapping.h"


#define inputWidth 640
#define inputHeight 480


#define camera 0

void InitMat(Mat& m, float* num)
{
	for (int i = 0; i < m.rows; i++)
		for (int j = 0; j < m.cols; j++)
			m.at<float>(i, j) = *(num + i * m.rows + j);
}

int main()
{
	char c;
	cin >> c;
	if (c == 'a')
	{
		//Take Photos
		VideoCapture capture(0);

		capture.set(CV_CAP_PROP_FRAME_WIDTH, inputWidth);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, inputHeight);

		Mat frame;
		int i = 0;
		while (1)
		{
			capture >> frame;
			imshow("show", frame);
			int key = waitKey(50);
			if ('a' == (char)key)
			{
				cout << "GET" << endl;
				imwrite("a" + to_string(i) + ".jpg", frame);
				i++;
			}
			else if (key != -1)
			{
				cout << key << endl;
			}
		}
	}
	else if (c == 'b')
	{
		//Spherical Mapping
		SphericalMapping sm;
		sm.StartMapping(0);
	}
	else if (c == 'c')
	{
		//My Own Mapping
		MyMapping mm;
		mm.StartMapping(1);
	}
	else if (c == 'd')
	{
		//Chessboard Calibration
		/*
		640 x 480 -> 340 x 320 (-150x2 ,-80x2)
		1280 x 720 -> 550 x 520 (-365x2 ,-100*2)
		*/

		Range Rw;
		Rw.start = 150;
		Rw.end = inputWidth - Rw.start;
		Range Rh;
		Rh.start = 80;
		Rh.end = inputHeight - Rh.start;
		int times = 5;

		FisheyeCalibration fc(inputWidth, inputHeight, Rw, Rh);
		fc.StartCalibration(camera, times);
	}
	return 0;
}