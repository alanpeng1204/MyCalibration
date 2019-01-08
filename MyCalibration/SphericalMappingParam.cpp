//#include "pch.h"	//�°汾��pch.h
#include "stdafx.h"	//�ɰ汾��stdafx.h
#include "SphericalMappingParam.h"

int SphericalMappingParam::_width = -1;
int SphericalMappingParam::_height = -1;
int SphericalMappingParam::radius = -1;
int SphericalMappingParam::N_value = 5;
int SphericalMappingParam::Threshold_value = 255;
string SphericalMappingParam::win_name = "SphericalMappingParam";
Point2i SphericalMappingParam::center = Point2i(-1, -1);
int SphericalMappingParam::Threshold_max_value = 255;
string SphericalMappingParam::Threshold_trackbar_name = "Threshold(0-255)";
int SphericalMappingParam::N_max_value = 15;
string SphericalMappingParam::N_trackbar_name = "N(0-15)";
Mat SphericalMappingParam::image = Mat();
//���۾�ͷ���ӳ���
const double SphericalMappingParam::FOV = PI / 180.0*185.0;

vector<vector<Point>> SphericalMappingParam::lines;
string SphericalMappingParam::check_win_name = "Check Verify";
vector<Point> SphericalMappingParam::points;

int SphericalMappingParam::CalculateParam(Mat img)
{
	_width = img.cols;
	_height = img.rows;
	imshow("123", img);
	waitKey();
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
	//�ñ�Ƕ���ɨ�跨ȡ��Բ����Ч�����Բ�ĺͰ뾶�Ĳ���
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

					points.push_back(ptMax1);
					goto top_label;
				}
			}
		top_label:

			for (int i = imgSize.height - 1; i >= 0; i--)
			{
				minMaxLoc(gray.row(i), &minVal, &maxVal, &ptMin2, &ptMax2);
				if ((maxVal - minVal) > threshold)
				{
					flag++;

					ptMax2.y = i;
					
					points.push_back(ptMax2);
					goto bottom_label;
				}
			}
		bottom_label:

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
						points.push_back(ptMax1);
						
						goto outer1;
					}
				}
			}
		outer1:


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
						points.push_back(ptMax2);
						goto outer2;
					}
				}
			}
		outer2:
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
					points.push_back(ptMax1);
					goto left_label;
				}
			}
		left_label:

			for (int i = gray.cols - 1; i >= 0; i--)
			{
				minMaxLoc(gray.col(i), &minVal, &maxVal, &ptMin2, &ptMax2);
				if ((maxVal - minVal) > threshold)
				{
					flag++;
					ptMax2.x = i;
					points.push_back(ptMax2);

					goto right_label;
				}
			}

		right_label:
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

						goto outer3;
					}
				}
			}
		outer3:
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
						points.push_back(ptMax2);
						goto outer4;
					}
				}
			}
		outer4:
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

	cout << "Use the Revised ScanLine Method:" << endl
		<< "\tThe center is (" << center.x << ", "
		<< center.y << ")" << endl
		<< "\tThe radius is " << radius << endl;

	circle(src, center, radius, Scalar(0, 0, 255), src.cols / 300);
	circle(src, center, 5, Scalar(0, 255, 255), -1);

	imshow(win_name, src);

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

//���ص�����
void SphericalMappingParam::onMouse(int event, int x, int y, int, void* params)
{
	Mat src = image.clone();
	Point pt(x, y);
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		circle(src, pt, src.cols*0.005, Scalar(0, 255, 0), -1);
		imshow(check_win_name, src);
		points.push_back(pt);
		if (2 == points.size())
		{
			findPoints(center, radius, points);
			vector<Point>::iterator it = points.begin();
			while (it != points.end() - 1)
			{
				circle(src, *it, src.cols*0.003, Scalar(0, 0, 255), -1);
				circle(src, *(it + 1), src.cols*0.003, Scalar(0, 0, 255), -1);

				line(src, *it, *(it + 1), Scalar(0, 255, 255), src.cols*0.002);
				++it;
			}
			imshow(check_win_name, src);
			lines.push_back(points);
			points.clear();
		}
		break;
	case EVENT_MOUSEMOVE:
		//cout << "(x, y) = (" << x << ", " << y << ")" << endl;
	default:
		;
	}
}

void SphericalMappingParam::findPoints(Point2i center, int radius, vector<Point> &points, camMode projMode)
{
	vector<Point3f> spherePoints(points.size());
	vector<Point3f>::iterator itSphere = spherePoints.begin();

	cout << points << endl;

	vector<Point>::iterator it = points.begin();

	while (it != points.end())
	{
		int u = it->x;
		int v = it->y;

		//Convert to cartiesian cooradinate in unity circle
		int x_cart = (u - center.x);
		int y_cart = -(v - center.y);

		//convert to polar axes
		double theta = cvFastArctan(y_cart, x_cart)*PI / 180;
		double p = sqrt(pow(x_cart, 2) + pow(y_cart, 2));

		double foval = 0.0;
		double Theta_sphere;
		switch (projMode)
		{

		case STEREOGRAPHIC:
			foval = radius / (2 * tan(FOV / 4));
			Theta_sphere = 2 * atan(p / (2 * foval));
			break;
		case EQUIDISTANCE:
			foval = radius / (FOV / 2);
			Theta_sphere = p / foval;
			break;
		case EQUISOLID:
			foval = radius / (2 * sin(FOV / 4));
			Theta_sphere = 2 * asin(p / (2 * foval));
			break;
		case ORTHOGONAL:
			foval = radius / sin(FOV / 2);
			Theta_sphere = asin(p / foval);
			break;
		default:
			cout << "The camera mode hasn't been choose!" << endl;
		}

		//convert to sphere surface parameter cooradinate
		double Phi_sphere = theta;

		//convert to sphere surface 3D cooradinate
		itSphere->x = sin(Theta_sphere)*cos(Phi_sphere);
		itSphere->y = sin(Theta_sphere)*sin(Phi_sphere);
		itSphere->z = cos(Theta_sphere);

		double temp = itSphere->x*itSphere->x +
			itSphere->y*itSphere->y +
			itSphere->z*itSphere->z;
		cout << "[x, y, z] = " << *itSphere << endl
			<< "norm = " << sqrt(temp) << endl;

		++it;
		++itSphere;
	}
	///////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	double angle = acos(spherePoints[0].dot(spherePoints[1]));

	//double angle = PI;
	cout << "spherePoints[0]=" << spherePoints[0] << endl;
	cout << "spherePoints[1]=" << spherePoints[1] << endl;

	Point3f e3 = spherePoints[0].cross(spherePoints[1]);
	double norm_e3 = norm(e3);
	e3.x /= norm_e3;
	e3.y /= norm_e3;
	e3.z /= norm_e3;

	if (e3.dot(Point3f(0, 0, 1)) < 0)
	{
		e3 = spherePoints[1].cross(spherePoints[0]);
		double norm_e3 = norm(e3);
		e3.x /= norm_e3;
		e3.y /= norm_e3;
		e3.z /= norm_e3;

		//swap shpereSpoint[0] and spherePoints[1]
		spherePoints[0] = spherePoints[0] + spherePoints[1];
		spherePoints[1] = spherePoints[0] - spherePoints[1];
		spherePoints[0] = spherePoints[0] - spherePoints[1];

	}
	Point3f e1 = spherePoints[0];
	Point3f e2 = e3.cross(e1);

	cout << "e1.e2=" << e1.dot(e2) << endl
		<< e2.dot(e3) << endl
		<< e3.dot(e1) << endl;
	cout << "norm(e1)=" << norm(e1) << endl << norm(e2) << endl
		<< norm(e3) << endl;
	vector<Point3f> tmpK;
	tmpK.push_back(e1);
	tmpK.push_back(e2);
	tmpK.push_back(e3);
	cout << e1 << endl << e2 << endl << e3 << endl;
	cout << "tmpK=" << tmpK << endl;
	Mat K = Mat(tmpK).reshape(1).t(); //�ӱ�׼�ռ����굽����ͷ�ռ�����ı任����
	cout << "K=" << K << endl;
	Mat T = K.inv(CV_SVD);//�ӱ���ͷ�ռ����굽��׼�ռ�����ƽ��任����

	points.clear();
	const int count = 20;
	double step = angle / count;
	double start = 0.0;
	int l = 0;
	while (l++ <= count)
	{
		Point3f stdPt(cos(start), sin(start), 0);
		Mat  matPt(stdPt);

		cout << matPt << endl << K << endl;
		Mat ptSphere(K*matPt);
		cout << ptSphere << endl;
		Mat_<double> ptSphere_double;
		ptSphere.convertTo(ptSphere_double, CV_64F);
		double x = ptSphere_double.at<double>(0, 0);
		double y = ptSphere_double.at<double>(1, 0);
		double z = ptSphere_double.at<double>(2, 0);

		cout << x << ", " << y << ", " << z << endl;

		//Convert from sphere cooradinate to the parameter sphere cooradinate
		double Theta_sphere = acos(z);
		double Phi_sphere = cvFastArctan(y, x)*PI / 180;//return value in Angle
														////////////////////////////////////////////////////////////////////////////////

		double foval = 0.0;
		double p;
		switch (projMode)
		{

		case STEREOGRAPHIC:
			foval = radius / (2 * tan(FOV / 4));
			p = 2 * foval*tan(Theta_sphere / 2);
			break;
		case EQUIDISTANCE:
			foval = radius / (FOV / 2);
			p = foval*Theta_sphere;
			break;
		case EQUISOLID:
			foval = radius / (2 * sin(FOV / 4));
			p = 2 * foval*sin(Theta_sphere / 2);
			break;
		case ORTHOGONAL:
			foval = radius / sin(FOV / 2);
			p = foval*sin(Theta_sphere);
			break;
		default:
			cout << "The camera mode hasn't been choose!" << endl;
		}
		//Convert from parameter sphere cooradinate to fish-eye polar cooradinate
		//p = sin(Theta_sphere);
		double theta = Phi_sphere;

		//Convert from fish-eye polar cooradinate to cartesian cooradinate
		double x_cart = p*cos(theta);
		double y_cart = p*sin(theta);

		//double R = radius / sin(camerFieldAngle / 2);

		//Convert from cartesian cooradinate to image cooradinate
		double u = x_cart + center.x;
		double v = -y_cart + center.y;

		Point pt = Point(u, v);
		points.push_back(pt);
		//////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////
		start += step;
	}
	/////////////////////////////////////////////////////////////////////
	cout << points << endl;
}

void SphericalMappingParam::checkVarify()
{
	namedWindow(check_win_name, CV_WINDOW_NORMAL);
	resizeWindow(check_win_name, _width, _height);
	imshow(check_win_name, image);
	setMouseCallback(check_win_name, onMouse);
	waitKey();
	destroyWindow(check_win_name);
}