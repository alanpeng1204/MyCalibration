#include "stdafx.h"
#include "FisheyeCalibration.h"

int FisheyeCalibration::StartCalibration(int cam, int times)
{
	if (times <= 0)
	{
		cout << "times should more than 0" << endl;
		return -1;
	}

	Mat K, K2;
	Vec4d D;
	vector<vector<Point2f>> imgPoints;
	VideoCapture capture(cam);


	capture.set(CV_CAP_PROP_FRAME_WIDTH, _width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, _height);

	Mat frame;
	vector<Point2f> corners;
	cout << "Press a to get the chess board" << endl;
	while (1)
	{
		capture >> frame;
		frame = Mat(frame, _rangeh, _rangew);
		bool found = findChessboardCorners(frame, Size(ChessBoardWidth, ChessBoardHeight), corners);
		drawChessboardCorners(frame, Size(ChessBoardWidth, ChessBoardHeight), corners, found);
		imshow("test camera", frame);
		int key = waitKey(15);		//延迟毫秒
		if (found && 'a' == (char)key)
		{
			imgPoints.push_back(corners);
			if (imgPoints.size() >= times)
			{
				cout << "start calculate" << endl;
				break;
			}
			cout << imgPoints.size() << endl;
		}
	}

	vector<vector<Point3f>> objPoints;
	vector<Point3f> objTemp;
	for (int i = 0; i < ChessBoardHeight; i++)
	{
		for (int j = 0; j < ChessBoardWidth; j++)
		{
			objTemp.push_back(Point3d(double(j * Square_Size), double(i * Square_Size), 0));
		}
	}
	for (int i = 0; i < imgPoints.size(); i++)
	{
		objPoints.push_back(objTemp);
	}

	vector<Vec3d> R, T;

	int flag = 0;
	flag |= fisheye::CALIB_RECOMPUTE_EXTRINSIC;		//计算外参
	flag |= fisheye::CALIB_CHECK_COND;	//检查条件数conditon number
	flag |= fisheye::CALIB_FIX_SKEW;	//求解时假设内参 fx = fy，偏斜系数skew coefficient始终为0

	double calibrate_error = fisheye::calibrate(
		objPoints,
		imgPoints,
		frame.size(),
		K,
		D,
		R,
		T,
		flag,
		TermCriteria(3, 20, 1e-6));

	cout << Size(frame.cols, frame.rows) << endl;
	cout << K << endl;
	cout << D << endl;

	K2 = getOptimalNewCameraMatrix(K, D, frame.size(), 0, Size(), 0, 1);

	while (1)
	{
		capture >> frame;
		frame = Mat(frame, _rangeh, _rangew);
		Mat output;

		fisheye::undistortImage(frame, output, K, D, K2);
		//remap(frame, output, map1, map2, INTER_LINEAR);
		//namedWindow("test camera", 0);
		//resizeWindow("test camera", Size(newW, newH));
		imshow("test camera", output);
		waitKey(20);
	}

	return 0;
}
