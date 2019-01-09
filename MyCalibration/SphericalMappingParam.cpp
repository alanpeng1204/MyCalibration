//#include "pch.h"	//新版本用pch.h
#include "stdafx.h"	//旧版本用stdafx.h
#include "SphericalMappingParam.h"

int SphericalMappingParam::_width = -1;
int SphericalMappingParam::_height = -1;
int SphericalMappingParam::radius = -1;
int SphericalMappingParam::N_value = 5;
int SphericalMappingParam::Threshold_value = 40;
string SphericalMappingParam::win_name = "SphericalMappingParam";
Point2i SphericalMappingParam::center = Point2i(-1, -1);
int SphericalMappingParam::Threshold_max_value = 255;
string SphericalMappingParam::Threshold_trackbar_name = "Threshold(0-255)";
int SphericalMappingParam::N_max_value = 15;
string SphericalMappingParam::N_trackbar_name = "N(0-15)";
Mat SphericalMappingParam::image = Mat();
//鱼眼镜头的视场角
const double SphericalMappingParam::FOV = PI;

int SphericalMappingParam::CalculateParam(Mat img)
{
	_width = img.cols;
	_height = img.rows;
	image = img;
	cout << "Find the circular region in image:" << endl;
	namedWindow(win_name, CV_WINDOW_NORMAL);
	resizeWindow(win_name, _width, _height + 100);
	createTrackbar(N_trackbar_name, win_name, &N_value, N_max_value, On_N_Change);
	createTrackbar(Threshold_trackbar_name, win_name, &Threshold_value, Threshold_max_value, On_Threshold_Change);
	On_N_Change(N_value, 0);
	On_Threshold_Change(Threshold_value, 0);
	waitKey();
	destroyWindow(win_name);
	
	return 0;
}

//Callback Function
void SphericalMappingParam::On_N_Change(int N_value, void* param) 
{
	//用变角度线扫描法取求圆形有效区域的圆心和半径的参数
	if (image.data)
		revisedScanLineMethod(image, center, radius, Threshold_value, N_value);

}

//Callback Function
void SphericalMappingParam::On_Threshold_Change(int Threshold_value, void* parmas)
{
	if (image.data)
		revisedScanLineMethod(image, center, radius, Threshold_value, N_value);
}
void SphericalMappingParam::revisedScanLineMethod(Mat imgOrg, Point2i& center, int& radius, int threshold, int N)
{
	Mat src, gray;
	src = imgOrg.clone();
	cvtColor(src, gray, CV_BGR2GRAY);

	vector<Point> points;
	vector<double> distance;

	Size imgSize = src.size();

	int x, y;
	double theta = 0;

	for (int n = 0; n < 2 * N; n++, theta = PI*n / (2 * N))
	{
		//if (n == N||n==0) continue;
		int min1, min2;
		min1 = min2 = 255;
		int max1, max2;
		max1 = max2 = 0;
		int radius = 0;
		Point ptMax1(0, 0), ptMax2(0, 0);
		Point ptMin1(0, 0), ptMin2(0, 0);
		int flag = 0;

		double minVal, maxVal;

		if (0 == n)
		{
			for (int i = 0; i < imgSize.height; i++)
			{
				minMaxLoc(gray.row(i), &minVal, &maxVal, &ptMin1, &ptMax1);
				if ((maxVal - minVal)>threshold)
				{
					flag++;

					ptMax1.y = i;
					//cout << "horizontal top:" << endl;
					//cout << "ptMax1=(" << ptMax1.x << ", " << ptMax1.y << ")" << endl;
					points.push_back(ptMax1);
					goto top_label;
				}
			}
		top_label:

#ifdef _SHOW_POINTS_
			circle(src, ptMax1, 5, Scalar(0, 255, 255), -1);
			imshow("src", src);
			cv::waitKey();
#endif

			for (int i = imgSize.height - 1; i >= 0; i--)
			{
				minMaxLoc(gray.row(i), &minVal, &maxVal, &ptMin2, &ptMax2);
				if ((maxVal - minVal) > threshold)
				{
					flag++;

					ptMax2.y = i;
					//cout << "horizontal bottom:" << endl;
					//cout << "ptMax2=(" << ptMax2.x << ", " << ptMax2.y << ")" << endl;

					//src.row(i) = Scalar(0, 0, 255);
					//src.row(i + 1) = Scalar(0, 0, 255);

					points.push_back(ptMax2);
					goto bottom_label;
				}
			}
		bottom_label:

#ifdef _SHOW_POINTS_
			circle(src, ptMax2, 5, Scalar(0, 255, 255), -1);
			line(src, ptMax1, ptMax2, Scalar(192, 192, 0), 2);
			imshow("src", src);
			cv::waitKey();
#endif
			if (flag == 2)
			{
				distance.push_back(sqrt(pow(ptMax1.x - ptMax2.x, 2) + pow(ptMax1.y - ptMax2.y, 2)));
			}
			else if (flag == 1)
			{
				points.pop_back();
			}
		}
		else if (0 < n&&n < N)
		{
			for (int i = 0; i < imgSize.width; i++)
			{
				for (int j = 0; j <= i; j++)
				{
					x = j;
					y = -tan(theta)*(x - i);

					Point ptCur(x, y);
					if (!ptCur.inside(Rect(0, 0, imgSize.width, imgSize.height)))
					{
						continue;
					}

					uchar I = gray.at<uchar>(ptCur);

					if (I > max1)
					{
						max1 = I;
						ptMax1 = ptCur;
					}
					if (I < min1)
					{
						min1 = I;
					}

					if (abs(max1 - min1) > threshold)
					{
						flag++;
						//cout << "jump outer1" << endl;
						//cout << "ptMax1=(" << ptMax1.x << ", " << ptMax1.y << ")" << endl;
						points.push_back(ptMax1);

						/*						Point start, end;
						for (int k = 0; k <= i; k++)
						{
						x = k;
						y = -tan(theta)*(x - i);
						if (k == 0)
						{
						start = Point(x, y);
						}
						else if (k == i)
						{
						end = Point(x, y);
						}


						}
						line(src, start, end, Scalar(0, 0, 255), 2);*/

						goto outer1;
					}
				}
			}
		outer1:

#ifdef _SHOW_POINTS_
			circle(src, ptMax1, 5, Scalar(0, 255, 255), -1);
			imshow("src", src);
			cv::waitKey();
#endif

			for (int i = imgSize.width - 1; i >= 0; i--)
			{
				for (int j = i; j < imgSize.width; j++)
				{
					x = j;
					y = imgSize.height - 1 - tan(theta)*(x - i);

					Point ptCur(x, y);

					if (!ptCur.inside(Rect(0, 0, imgSize.width, imgSize.height)))
					{
						continue;
					}
					uchar I = gray.at<uchar>(ptCur);

					if (I > max2)
					{
						max2 = I;
						ptMax2 = ptCur;
					}
					if (I < min2)
					{
						min2 = I;
					}

					if (abs(max2 - min2) > threshold)
					{
						flag++;
						//cout << "jump outer2" << endl;
						//cout << "ptMax2=(" << ptMax2.x << ", " << ptMax2.y << ")" << endl;
						points.push_back(ptMax2);

						//Point start, end;
						//for (int k = i; k < imgSize.width; k++)
						//{
						//	x = k;
						//	y = imgSize.height - 1 - tan(theta)*(x - i);
						//	if (k == i)
						//	{
						//		start = Point(x, y);
						//	}
						//	else if (k == imgSize.width-1)
						//	{
						//		end = Point(x, y);
						//	}


						//}
						//line(src, start, end, Scalar(0, 0, 255), 2);

						goto outer2;
					}
				}
			}
		outer2:
			;

#ifdef _SHOW_POINTS_
			circle(src, ptMax2, 5, Scalar(0, 255, 255), -1);
			line(src, ptMax1, ptMax2, Scalar(192, 192, 0), 2);
			imshow("src", src);
			cv::waitKey();
#endif
			if (flag == 2)
			{
				distance.push_back(sqrt(pow(ptMax1.x - ptMax2.x, 2) + pow(ptMax1.y - ptMax2.y, 2)));
			}
			else if (flag == 1)
			{
				points.pop_back();
			}

		}
		else if (N == n)
		{
			for (int i = 0; i < imgSize.width; i++)
			{
				minMaxLoc(gray.col(i), &minVal, &maxVal, &ptMin1, &ptMax1);
				if ((maxVal - minVal)>threshold)
				{
					flag++;
					ptMax1.x = i;
					//cout << "vertical left:" << endl;
					//cout << "ptMax1=(" << ptMax1.x << ", " << ptMax1.y << ")" << endl;

					//src.col(i) = Scalar(0, 0, 255);
					//src.col(i - 1) = Scalar(0, 0, 255);
					points.push_back(ptMax1);
					goto left_label;
				}
			}
		left_label:

#ifdef _SHOW_POINTS_
			circle(src, ptMax1, 5, Scalar(0, 255, 255), -1);
			imshow("src", src);
			cv::waitKey();
#endif

			for (int i = gray.cols - 1; i >= 0; i--)
			{
				minMaxLoc(gray.col(i), &minVal, &maxVal, &ptMin2, &ptMax2);
				if ((maxVal - minVal) > threshold)
				{
					flag++;
					ptMax2.x = i;
					//cout << "vertical right:" << endl;
					//cout << "ptMax1=(" << ptMax2.x << ", " << ptMax2.y << ")" << endl;
					points.push_back(ptMax2);

					//src.col(i) = Scalar(0, 0, 255);
					//src.col(i + 1) = Scalar(0, 0, 255);

					goto right_label;
				}
			}

		right_label:

#ifdef _SHOW_POINTS_
			circle(src, ptMax2, 5, Scalar(0, 255, 255), -1);
			line(src, ptMax1, ptMax2, Scalar(192, 192, 0), 2);
			imshow("src", src);
			cv::waitKey();
#endif
			if (flag == 2)
			{
				distance.push_back(sqrt(pow(ptMax1.x - ptMax2.x, 2) + pow(ptMax1.y - ptMax2.y, 2)));
			}
			else if (flag == 1)
			{
				points.pop_back();
			}

		}
		else if (N < n&&n < 2 * N)
		{
			for (int i = 0; i < imgSize.width; i++)
			{
				for (int j = 0; j <= i; j++)
				{
					x = j;
					y = imgSize.height - 1 - tan(theta)*(x - i);

					Point ptCur(x, y);
					if (!ptCur.inside(Rect(0, 0, imgSize.width, imgSize.height)))
					{
						continue;
					}
					uchar I = gray.at<uchar>(ptCur);

					if (I > max1)
					{
						max1 = I;
						ptMax1 = ptCur;
					}
					if (I < min1)
					{
						min1 = I;
					}

					if (abs(max1 - min1) > threshold)
					{
						flag++;
						//cout << "jump outer3" << endl;
						//cout << "ptMax1=(" << ptMax1.x << ", " << ptMax1.y << ")" << endl;
						points.push_back(ptMax1);

						//Point start, end;
						//for (int k = 0; k <= i; k++)
						//{
						//	x = k;
						//	y = imgSize.height - 1 - tan(theta)*(x - i);

						//	if (k == 0)
						//	{
						//		start = Point(x, y);
						//	}
						//	else if (k == i)
						//	{
						//		end = Point(x, y);
						//	}
						//}
						//line(src, start, end, Scalar(0, 0, 255), 2);

						goto outer3;
					}
				}
			}
		outer3:

#ifdef _SHOW_POINTS_
			circle(src, ptMax1, 5, Scalar(0, 255, 255), -1);
			imshow("src", src);
			cv::waitKey();
#endif

			for (int i = imgSize.width - 1 / 2; i >= 0; i--)
			{
				for (int j = i; j < imgSize.width; j++)
				{
					x = j;
					y = -tan(theta)*(x - i);

					Point ptCur(x, y);
					if (!ptCur.inside(Rect(0, 0, imgSize.width, imgSize.height)))
					{
						continue;
					}
					uchar I = gray.at<uchar>(ptCur);

					if (I > max2)
					{
						max2 = I;
						ptMax2 = ptCur;
					}
					if (I < min2)
					{
						min2 = I;
					}

					if (abs(max2 - min2) > threshold)
					{
						flag++;
						//cout << "jump outer4" << endl;
						//cout << "ptMax2=(" << ptMax2.x << ", " << ptMax2.y << ")" << endl;
						points.push_back(ptMax2);

						/*			Point start, end;
						for (int k = i; k < imgSize.width; k++)
						{
						x = k;
						y = -tan(theta)*(x - i);
						if (k == i)
						{
						start = Point(x, y);
						}
						else if (k == imgSize.width - 1)
						{
						end = Point(x, y);
						}


						}
						line(src, start, end, Scalar(0, 0, 255), 2);*/

						goto outer4;
					}
				}
			}
		outer4:

			;
#ifdef _SHOW_POINTS_
			circle(src, ptMax2, 5, Scalar(0, 255, 255), -1);
			line(src, ptMax1, ptMax2, Scalar(192, 192, 0), 2);
			imshow("src", src);
			cv::waitKey();
#endif
			if (flag == 2)
			{
				distance.push_back(sqrt(pow(ptMax1.x - ptMax2.x, 2) + pow(ptMax1.y - ptMax2.y, 2)));
			}
			else if (flag == 1)
			{
				points.pop_back();
			}

		}
		else
		{
			cout << "The value of n is error!" << endl;
			break;
		}
	}


	//vector<Point>::iterator itero = points.begin();
	//ofstream of("points.txt", ios::trunc | ios::out);
	//while (itero != points.end())
	//{
	//	of << (*itero).x << ", " << (*itero).y << endl;
	//	itero++;
	//}
	//of.close(); 


	//find out validate points
	double mean = 0;
	vector<double>::iterator iter = distance.begin();
	while (iter != distance.end())
	{
		mean += *iter;
		iter++;
	}
	mean /= distance.size();

	vector<Point> validPoints;
	for (int i = 0; i < distance.size(); i++)
	{
		if (distance.at(i) < mean)
		{
			validPoints.push_back(points.at(2 * i));
			validPoints.push_back(points.at(2 * i + 1));
		}
	}

	//figure out the center and radius of the circle with Kasa method

	if (!CircleFitByKasa(validPoints, center, radius))
	{
		cout << "Revisied LineScan Method Failed, Because the Circle Fit Method failed!" << endl;
		return;
	}

	//#ifdef _DEBUG_
	cout << "Use the Revised ScanLine Method:" << endl
		<< "\tThe center is (" << center.x << ", "
		<< center.y << ")" << endl
		<< "\tThe radius is " << radius << endl;

	circle(src, center, radius, Scalar(0, 0, 255), src.cols / 300);
	circle(src, center, 5, Scalar(0, 255, 255), -1);

	//cv::namedWindow("Revised ScanLine Method Result", CV_WINDOW_AUTOSIZE);
	//imshow("Revised ScanLine Method Result", src);
	imshow(win_name, src);
	//imwrite("Revised_Scan_ret.tiff", src);
	//cv::waitKey();
	//#endif

}

bool SphericalMappingParam::CircleFitByKasa(vector<Point> validPoints, Point& center, int&	 radius)
{
	if (validPoints.size() <= 2)
	{
		cout << "The Circle fit failed, Because there is not enought validate points to use!" << endl;
		return false;
	}

	vector<Point3i> extendA;
	vector<int> extendB;
	vector<Point>::iterator iter = validPoints.begin();
	while (iter != validPoints.end())
	{
		extendA.push_back(Point3i((*iter).x, (*iter).y, 1));
		extendB.push_back((pow((*iter).x, 2) + pow((*iter).y, 2)));
		iter++;
	}
	Mat A = Mat(extendA).reshape(1);
	Mat	B = Mat(extendB).reshape(1);

	Mat_<double> dA, dB;
	Mat_<double> P(3, 1, CV_64F);
	A.convertTo(dA, CV_64F);
	B.convertTo(dB, CV_64F);
	P = dA.inv(CV_SVD)*dB;

	//cout << P << endl;

	double p1, p2, p3;
	p1 = P.at<double>(0, 0);
	p2 = P.at<double>(1, 0);
	p3 = P.at<double>(2, 0);

	center.x = p1 / 2;
	center.y = p2 / 2;
	radius = sqrt((pow(p1, 2) + pow(p2, 2)) / 4 + p3);

	//cout << center.x << endl << center.y << endl << radius << endl;
	return true;
}

bool SphericalMappingParam::getCircleParatemer(Point2i& c, int& r)
{
	if (center != Point2i(-1, -1) && radius != -1)
	{
		c = center;
		r = radius;
		return true;
	}
	return false;
}