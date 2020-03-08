#pragma once
#include "Stdafx.h"
#include <iostream>

#include"MyForm.h"





void estimateWater(std::string num){
	std::cout << "test\n";

	std::string date = "20200119";
	//std::string num = "1";

	cv::Mat rs = cv::imread("data\\"+date+"\\"+num+"\\color.png");
	
	cv::Mat grayRs;
	cvtColor(rs,grayRs,CV_RGB2GRAY);

	cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\transedImg.png");

	///////////////////////////////////////////////////////////////////////////
	cv::Mat normalized = cv::Mat::zeros(cv::Size(1280,702), CV_8UC1);
	cv::normalize(nir, normalized, 0, 255, CV_MINMAX);
	
	cv::Point max_pt;
	double maxVal;
	cv::minMaxLoc(grayRs, NULL, &maxVal, NULL, &max_pt);
	std::cout << "maxVal = " << maxVal << std::endl;
	
	for (int limen = 0; limen < 255; limen += 10) {
		cv::Mat difImg = createDiffImg(grayRs, nir, date, num, limen);
		cv::imshow("difImg" + std::to_string(limen) + ".png", difImg);
	}
	
	cv::Mat segmentImg = kMeans(rs);

	cv::imshow("rs", rs);
	cv::imshow("grayRs", grayRs);
	cv::imshow("nir", nir);
	cv::imshow("segmentImg", segmentImg);

}



cv::Mat createDiffImg(cv::Mat grayRs, cv::Mat nir, std::string date, std::string num, int limen) {

	//for (int limen = 0; limen < 255; limen += 10) {

	cv::Mat difImg = cv::Mat::zeros(720, 1280, CV_8UC1);
	for (int n = 0; grayRs.total() > n; n++) {
		int x = n % grayRs.cols;
		int y = n / grayRs.cols;

		int dif = int(grayRs.at<uchar>(y, x)) - int(nir.at<uchar>(y, x));

		if (dif > limen) {
			difImg.at<uchar>(y, x) = dif;
		}
	}
	//cv::imwrite("data\\" + date + "\\" + num + "\\diff" + std::to_string(limen) + ".png", difImg);
	//cv::imshow("difImg" + std::to_string(limen) + ".png", difImg);
	//cv::waitKey(1);

	return difImg;
	//}
}


void createHistgram(std::string num){
	std::cout << "test\n";
	std::string date = "20200119";
	cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\transedImg.png");

	cv::Mat hist;
	int hist_size = 256;
	float range[] = { 0, 256 };
	const float* hist_range = range;

	cv::Mat channels[1];
	cv::split(nir, channels);

	/* ‰æ‘f”‚ð”‚¦‚é */
	cv::calcHist(&channels[0], 1, 0, cv::Mat(), hist, 1, &hist_size, &hist_range);
	/* ³‹K‰» */

	cv::normalize(hist, hist, 0.0, 1.0, cv::NORM_MINMAX, -1, cv::Mat());
	/* ƒqƒXƒgƒOƒ‰ƒ€¶¬—p‚Ì‰æ‘œ‚ðì¬ */
	cv::Mat image_hist = cv::Mat(cv::Size(276, 320), CV_8UC3, cv::Scalar(255, 255, 255));



	/* ”wŒi‚ð•`‰æiŒ©‚â‚·‚­‚·‚é‚½‚ß‚ÉƒqƒXƒgƒOƒ‰ƒ€•”•ª‚Ì”wŒi‚ðƒOƒŒ[‚É‚·‚éj */
	for (int i = 0; i < 3; i++) {
		rectangle(image_hist, cv::Point(10, 10 + 100 * i),
			cv::Point(265, 100 + 100 * i), cv::Scalar(230, 230, 230), -1);

		/* ƒqƒXƒgƒOƒ‰ƒ€‚ð•`‰æ */
		for (int i = 0; i < 256; i++) {
			cv::line(image_hist, cv::Point(10 + i, 100),
				cv::Point(10 + i, 100 - (int)(hist.at<float>(i) * 80)),
				cv::Scalar(0, 0, 255), 1, 8, 0);
			// ‰¡Ž²10‚¸‚Âƒ‰ƒCƒ“‚ðˆø‚­
			if (i % 10 == 0) {
				cv::line(image_hist, cv::Point(10 + i, 100), cv::Point(10 + i, 10),
					cv::Scalar(170, 170, 170), 1, 8, 0);
				if (i % 50 == 0) {
					cv::line(image_hist, cv::Point(10 + i, 100), cv::Point(10 + i, 10),
						cv::Scalar(50, 50, 50), 1, 8, 0);
				}
			}
		}
	
	
	}
	cv::imshow("histgram", hist);  // ƒqƒXƒgƒOƒ‰ƒ€•\Ž¦
	cv::waitKey(0);

}


cv::Mat kMeans(cv::Mat src_img)
{
	std::cout << "0\n";
	const int cluster_count = 4; /* number of cluster */
	std::cout << "1\n";

	// (1)load a specified file as a 3-channel color image
	//cv::Mat src_img = cv::imread("data\\20200119\\5\\color.png");
	if (!src_img.data)
		return src_img;
	std::cout << "2\n";

	// (2)reshape the image to be a 1 column cv::Matrix 
	cv::Mat points;
	src_img.convertTo(points, CV_32FC3);
	points = points.reshape(3, src_img.rows*src_img.cols);
	std::cout << "3\n";

	// (3)run k-means clustering algorithm to segment pixels in RGB color space
	cv::Mat_<int> clusters(points.size(), CV_32SC1);
	cv::Mat centers;
	kmeans(points, cluster_count, clusters,
		cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, cv::KMEANS_PP_CENTERS, centers);
	std::cout << "4\n";

	// (4)make a each centroid represent all pixels in the cluster
	cv::Mat dst_img(src_img.size(), src_img.type());
	cv::MatIterator_<cv::Vec3f> itf = centers.begin<cv::Vec3f>();
	cv::MatIterator_<cv::Vec3b> itd = dst_img.begin<cv::Vec3b>(), itd_end = dst_img.end<cv::Vec3b>();
	for (int i = 0; itd != itd_end; ++itd, ++i) {
		cv::Vec3f color = itf[clusters(1, i)];
		(*itd)[0] = cv::saturate_cast<uchar>(color[0]);
		(*itd)[1] = cv::saturate_cast<uchar>(color[1]);
		(*itd)[2] = cv::saturate_cast<uchar>(color[2]);
	}
	std::cout << "5\n";
	/*
	// (5)show source and destination image, and quit when any key pressed
	cv::namedWindow("src_img", CV_WINDOW_AUTOSIZE);
	imshow("src_img", src_img);
	cv::namedWindow("dst_img", CV_WINDOW_AUTOSIZE);
	imshow("dst_img", dst_img);
	cv::waitKey(0);
	*/
	return dst_img;
}