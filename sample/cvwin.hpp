//nir‚ÉopencvŽg‚¦‚é‚æ‚¤‚Écwin‚ð“ü‚ê‚Ä‚é
#pragma once

#include"stdafx.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vector>
#include <windows.h>
#include <msclr/marshal.h>

namespace cvwin {

	HBITMAP IconToBitmap(HICON hIcon);

	std::vector<uchar> BitmapToPng(HBITMAP hBitmap);
	cv::Mat BitmapToMat(HBITMAP hBitmap);

	std::vector<uchar> IconToPng(HICON hIcon);
	cv::Mat IconToMat(HICON hIcon);

}