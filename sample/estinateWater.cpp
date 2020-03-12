#pragma once
#include "Stdafx.h"
#include <iostream>

#include"MyForm.h"





void estimateWater(std::string num){

	std::string date = "20200119";

	//cv::Mat rs = cv::imread("data\\" + date + "\\" + num + "\\color.png");
	cv::Mat rs = cv::imread("data\\" + date + "\\" + num + "\\removedRsImg.png");
	
	cv::Mat grayRs = cv::imread("data\\" + date + "\\" + num + "\\removedRsgrayImg.png", 0);
	//cv::Mat grayRs = cv::imread("data\\" + date + "\\" + num + "\\gray.png", 0);

	//cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\transedImg.png",0);
	cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\removedNirImg.png",0);

	///////////////////////////////////////////////////////////////////////////
	//cv::Mat normalized = cv::Mat::zeros(cv::Size(1280,702), CV_8UC1);
	//cv::normalize(nir, normalized, 0, 255, CV_MINMAX);
	
	//cv::Point max_pt;
	//double maxVal;
	//cv::minMaxLoc(grayRs, NULL, &maxVal, NULL, &max_pt);
	//std::cout << "maxVal = " << maxVal << std::endl;
	
	for (int limen = 0; limen < 255; limen += 10) {
		cv::Mat difImg = createDiffImg(grayRs, nir, date, num, limen);
		cv::Mat segmentImg = kMeans(difImg);

		cv::imshow("difImg" + std::to_string(limen) + ".png", difImg);
		cv::imshow("segmentImg" + std::to_string(limen) + ".png", segmentImg);
		cv::imwrite("data\\" + date + "\\" + num + "\\difImg" + "\\difImg" + std::to_string(limen) + ".png", difImg);
		cv::imwrite("data\\" + date + "\\" + num + "\\K-means" + "\\segment" + std::to_string(limen) + ".png", segmentImg);
	}
	
	cv::Mat segmentImg = kMeans(rs);
	cv::Mat segmentGrayRsImg = kMeans(grayRs);

	cv::imshow("rs", rs);
	cv::imshow("grayRs", grayRs);
	cv::imshow("nir", nir);
	cv::imshow("segmentImg", segmentImg);
	cv::imshow("segmentGrayRsImg", segmentGrayRsImg);
	cv::imwrite("data\\" + date + "\\" + num +"\\K-means"+ "\\segmentRsImg.png", segmentImg);

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


void createHistgram() {
	//http://opencv.jp/opencv2-x-samples/color_histogram
	// (1)load a source image as is
	std::string num = "1";
	std::string date = "20200119";
	cv::Mat src_img = cv::imread("data\\" + date + "\\" + num + "\\transedImg.png",0);
	

	// (2)allocate Mat to draw a histogram image
	const int ch_width = 260;
	const int sch = src_img.channels();
	cv::Mat hist_img(cv::Size(ch_width * sch, 200), CV_8UC3, cv::Scalar::all(255));

	std::vector<cv::MatND> hist(3);//N-dimentional Matrix
	const int hist_size = 256;
	const int hdims[] = { hist_size };
	const float hranges[] = { 0,256 };
	const float* ranges[] = { hranges };
	double max_val = .0;


	if (sch == 1) {
		// (3a)if the source image has single-channel, calculate its histogram
		calcHist(&src_img, 1, 0, cv::Mat(), hist[0], 1, hdims, ranges, true, false);
		minMaxLoc(hist[0], 0, &max_val);
	}
	else {
		// (3b)if the souce image has multi-channel, calculate histogram of each plane
		for (int i = 0; i < sch; ++i) {
			calcHist(&src_img, 1, &i, cv::Mat(), hist[i], 1, hdims, ranges, true, false);
			double tmp_val;
			minMaxLoc(hist[i], 0, &tmp_val);
			max_val = max_val < tmp_val ? tmp_val : max_val;
		}
	}

	
	std::cout << "hist[0] = " << hist[0] << std::endl;

	int sum = 0;
	for (int i = 0; i < hist[0].rows;i++) {
		std::cout << i << std::endl;
		std::cout << "hist[i] = " << hist[0].at<float>(i,0) << std::endl;
		sum+=hist[0].at<float>(i, 0);
	}
	std::cout << "sum = " << sum << std::endl;


	// (4)scale and draw the histogram(s)
	cv::Scalar color = cv::Scalar::all(100);
	for (int i = 0; i < sch; i++) {
		if (sch == 3)
			color = cv::Scalar((0xaa << i * 8) & 0x0000ff, (0xaa << i * 8) & 0x00ff00, (0xaa << i * 8) & 0xff0000, 0);
		hist[i].convertTo(hist[i], hist[i].type(), max_val ? 200. / max_val : 0., 0);
		for (int j = 0; j < hist_size; ++j) {
			int bin_w = cv::saturate_cast<int>((double)ch_width / hist_size);//ある基本型から別の基本型への正確な変換のためのテンプレート関数．
			rectangle(hist_img,
				cv::Point(j*bin_w + (i*ch_width), hist_img.rows),
				cv::Point((j + 1)*bin_w + (i*ch_width), hist_img.rows - cv::saturate_cast<int>(hist[i].at<float>(j))),
				color, -1);
		}
	}


	// (5)show the histogram iamge, and quit when any key pressed
	cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Histogram", CV_WINDOW_AUTOSIZE);
	cv::imshow("Image", src_img);
	cv::imshow("Histogram", hist_img);
	cv::waitKey(0);

}


cv::Mat kMeans(cv::Mat src_img)
{
	std::cout << "0\n";
	const int cluster_count = 3; /* number of cluster */
	std::cout << "1\n";

	// (1)load a specified file as a 3-channel color image
	//cv::Mat src_img = cv::imread("data\\20200119\\5\\color.png");
	if (!src_img.data)
		return src_img;
	std::cout << "2\n";

	// (2)reshape the image to be a 1 column cv::Matrix 
	cv::Mat points;
	src_img.convertTo(points, CV_32FC3);
	//src_img.convertTo(points, CV_32FC1);
	//points = points.reshape(3, src_img.rows*src_img.cols);
	points = points.reshape(1, src_img.rows*src_img.cols);
	std::cout << "3\n";

	// (3)run k-means clustering algorithm to segment pixels in RGB color space
	cv::Mat_<int> clusters(points.size(), CV_32SC1);
	cv::Mat centers;
	kmeans(points, cluster_count, clusters,
		cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, cv::KMEANS_PP_CENTERS, centers);//停止条件
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
	
	//std::cout << "clusters = " << clusters << std::endl;

	return dst_img;
}


void nitika() {
	std::string num = "1";
	std::string date = "20200119";
	cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\removedNirImg.png", 0);
	cv::imshow("nir", nir);

	cv::Mat threshold;
	cv::Mat thresholdAuto;
	for (int limen = 0; limen < 255; limen+=10) {
		cv::threshold(nir, threshold, limen, 255, CV_THRESH_BINARY);
		cv::imshow("threshold" + std::to_string(limen), threshold );
	}
	cv::threshold(nir, thresholdAuto, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cv::imshow("threshold2", thresholdAuto);
}



void blockNoise() {
	std::string num = "1";
	std::string date = "20200119";
	cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\removedNirImg.png", 0);
	cv::imshow("nir", nir);

	cv::Mat blur;
	cv::blur(nir, blur, cv::Size(3,3), cv::Size(-1, -1));

	cv::imshow("blur", blur);
}





