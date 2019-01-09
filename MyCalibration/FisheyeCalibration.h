#pragma once
#include "GlobalDefine.h"

#define ChessBoardWidth 6
#define ChessBoardHeight 9

#define Square_Size 9	//mm


class FisheyeCalibration
{
public:
	FisheyeCalibration() :_width(640), _height(480) {
		_rangew.start = 0;
		_rangew.end = _width;
		_rangeh.start = 0;
		_rangeh.end = _height;
	}
	FisheyeCalibration(int width, int height) :_width(width), _height(height) {
		_rangew.start = 0;
		_rangew.end = width;
		_rangeh.start = 0;
		_rangeh.end = height;
	}
	FisheyeCalibration(int width, int height, Range rangew, Range rangeh) :_rangew(rangew), _rangeh(rangeh), _width(width), _height(height) {};

	int StartCalibration(int cam, int times);
private:
	Range _rangew;
	Range _rangeh;
	int _width;
	int _height;
};

