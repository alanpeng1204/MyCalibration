#include "stdafx.h"
#include "Viewer.h"


Viewer::Viewer(Mat image)
{
	Size size(VIEWER_WIDTH, VIEWER_HEIGHT);
	win_name = "My Viewer";
	result = Mat(size, image.type());
	angleH = 0;
	angleV = PI / 2;
	distance = 300;
}

void Viewer::SetParam(double aH, double aV)
{
	angleH = aH;
	angleV = aV;
}


Mat Viewer::ConvertImage(Mat image)
{
	return Mat();
}


Point3d Viewer::navigationHV(Point3d orgPt, double aH, double aV)
{
	Mat cvtMatH(3, 3, CV_64F, Scalar(0));
	cvtMatH.at<double>(0, 0) = cos(aH);
	cvtMatH.at<double>(0, 2) = -sin(aH);
	cvtMatH.at<double>(1, 1) = 1;
	cvtMatH.at<double>(2, 0) = sin(aH);
	cvtMatH.at<double>(2, 2) = cos(aH);

	Mat cvtMatV(3, 3, CV_64F, Scalar(0));
	cvtMatV.at <double>(0, 0) = 1;
	cvtMatV.at<double>(1, 1) = cos(aV);
	cvtMatV.at<double>(1, 2) = -sin(aV);
	cvtMatV.at<double>(2, 1) = sin(aV);
	cvtMatV.at<double>(2, 2) = cos(aV);

	Mat cvtMat = cvtMatV*cvtMatH;

	Mat org(orgPt);
	Mat_<double> dOrg;
	org.convertTo(dOrg, CV_64F);

	Mat_<double> ret(3, 1, CV_64F);
	ret = cvtMat*dOrg;

	return Point3d(ret);
}