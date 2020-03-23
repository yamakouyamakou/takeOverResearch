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
	//cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\removedNirImg.png", 0);
	cv::Mat nir = cv::imread("data\\" + date + "\\" + num + "\\floorNirImg.png", 0);


	//ヒストグラム作成
	std::vector<cv::MatND> hist(3);
	cv::Mat hist_img;
	std::tie(hist, hist_img) = createHistgram(nir);//std::tie()要素をまとめて取り出す
	cv::imshow("hist_img",hist_img );
	cv::imwrite("data\\" + date + "\\" + num + "\\hist_img.png",hist_img);
	
	//ヒストグラム考察
	std::cout << "hist[0] = " << hist[0] << std::endl;
	double max;
	cv::Point minP,maxP;
	cv::minMaxLoc(hist[0], NULL, &max, NULL, &maxP);
	std::cout << "max = " << max << std::endl;
	std::cout << "minP = " << minP << std::endl;
	std::cout << "maxP = " << maxP << std::endl;// ここでもとめた（0,44）つまり44以下の画素を色図けする
	///////////////////////////////////////////////////////////////////////////
	
	//正規化
	//cv::Mat normalized = cv::Mat::zeros(cv::Size(1280,702), CV_8UC1);
	//cv::normalize(nir, normalized, 0, 255, CV_MINMAX);
	//cv::Point max_pt;
	//double maxVal;
	//cv::minMaxLoc(grayRs, NULL, &maxVal, NULL, &max_pt);
	//std::cout << "maxVal = " << maxVal << std::endl;
	

	std::cout << "maxP.y = " << maxP.y << std::endl;
	cv::Mat floorNir = cv::imread("data\\" + date + "\\" + num + "\\floorNirImg.png",0 );
	cv::Mat estimateNirImg = cv::Mat::zeros(720, 1280, CV_8UC3);

	estimateNirImg = coloring(floorNir,estimateNirImg,num,date,maxP);
	
	

	/*
	for (int limen = 0; limen < 255; limen += 10) {
		cv::Mat difImg = createDiffImg(grayRs, nir, date, num, limen);
		cv::Mat segmentImg = kMeans(difImg);

		cv::imshow("difImg" + std::to_string(limen) + ".png", difImg);
		cv::imshow("segmentImg" + std::to_string(limen) + ".png", segmentImg);
		cv::imwrite("data\\" + date + "\\" + num + "\\difImg" + "\\difImg" + std::to_string(limen) + ".png", difImg);
		cv::imwrite("data\\" + date + "\\" + num + "\\K-means" + "\\segment" + std::to_string(limen) + ".png", segmentImg);
	}
	*/

	cv::Mat segmentImg = kMeans(rs);
	cv::Mat segmentGrayRsImg = kMeans(grayRs);

	cv::imshow("rs", rs);
	cv::imshow("grayRs", grayRs);
	cv::imshow("nir", nir);
	cv::imshow("segmentImg", segmentImg);
	cv::imshow("segmentGrayRsImg", segmentGrayRsImg);
	cv::imshow("floorNirImg", floorNir);
	cv::imshow("estimateNirImg", estimateNirImg);
	cv::imwrite("data\\" + date + "\\" + num +"\\K-means"+ "\\segmentRsImg.png", segmentImg);
	cv::imwrite("data\\" + date + "\\" + num + "\\estimateNirImg.png", estimateNirImg);

}


cv::Mat coloring(
											cv::Mat floorNirImg,cv::Mat estimateNirImg,
											std::string num,
											std::string date,
											cv::Point max
											) {
	
	std::ifstream pcRs("data\\" + date + "\\" + num + "\\floorTransedNir.csv");
	std::ofstream pcEstimateNir("data\\" + date + "\\" + num + "\\estimateFloorNir.csv");

	int n = 0;
	std::string line;
	while( std::getline(pcRs, line) ) {//点群
		
		double num1, num2, num3, num4, num5, num6;
		replace(line.begin(), line.end(), ',', ' ');
		std::istringstream iss(line);
		iss >> num1 >> num2 >> num3 >> num4 >> num5 >> num6;

		if ( num4< 55 ) {//-------------------------------------閾値:max.y
			pcEstimateNir << num1 << "," << num2 << "," << num3 << "," << 255 << "," << 0 << "," << 0 << std::endl;
		}
		else {
			pcEstimateNir << num1 << "," << num2 << "," << num3 << "," << num4 << "," << num5 << "," << num6 << std::endl;
		}


		n++;
	}

	n = 0;
	std::cout << "floorNirImg.total() = " << floorNirImg.total() << std::endl;
	while (floorNirImg.total()>n ) {//画像
		int x = n % floorNirImg.cols;
		int y = n / floorNirImg.cols;


		if ((int)floorNirImg.at<uint8_t>(y, x) < 55) {//---------------------閾値
			estimateNirImg.at<cv::Vec3b>(y, x)[0] = 0;
			estimateNirImg.at<cv::Vec3b>(y, x)[1] = 0;
			estimateNirImg.at<cv::Vec3b>(y, x)[2] = 255;
		}
		else {
			estimateNirImg.at<cv::Vec3b>(y, x)[0] = (int)floorNirImg.at<uint8_t>(y, x);
			estimateNirImg.at<cv::Vec3b>(y, x)[1] = (int)floorNirImg.at<uint8_t>(y, x);
			estimateNirImg.at<cv::Vec3b>(y, x)[2] = (int)floorNirImg.at<uint8_t>(y, x);
		}

		n++;
	}


	return estimateNirImg;
}



void decisionLimen() {
	std::string path = "data\\nirHist\\water\\";
	//std::string path = "data\\nirHist\\paper\\";
	std::ofstream ofs(path + "theModes.csv");
	//std::ofstream ofs("data\\nirHist\\water\\theModes.csv");

	for (int i = 1; i < 11;i++) {
		std::cout << "i= " << i << std::endl;
		std::ofstream ofs2(path+ "histData\\" + std::to_string(i)+"histData.csv");
		//cv::Mat src_img = cv::imread("data\\nirHist\\water\\" + std::to_string(i) + "0mm.png", 0);
		cv::Mat src_img = cv::imread(path + std::to_string(i) + "0mm.png", 0);

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
			std::cout << "(3) is ok\n";
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

		cv::imshow("src_img" + std::to_string(i), src_img);
		cv::imshow("hist_img" + std::to_string(i), hist_img);
		cv::imwrite(path+"hist_img\\"+std::to_string(i)+"0mm.png",hist_img);

		double max;
		cv::Point minP, maxP;
		cv::minMaxLoc(hist[0], NULL, &max, NULL, &maxP);
		std::cout << "maxP.y= " << maxP.y << std::endl << std::endl;
		
		ofs << std::to_string(i) << "0mm" << " is " << maxP.y << std::endl;
		ofs2 << std::to_string(i) << "0mm" << std::endl << hist[0] << std::endl;

	}
	
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



//ヒストグラム作る
std::tuple<std::vector<cv::MatND>, cv::Mat > createHistgram( cv::Mat src_img) {
	//http://opencv.jp/opencv2-x-samples/color_histogram
	// (1)load a source image as is
	std::string date = "20200119";
	//cv::Mat src_img = cv::imread("data\\" + date + "\\" + num + "\\floorNirImg.png",0);
	

	// (2)allocate Mat to draw a histogram image
	const int ch_width = 260;
	const int sch = src_img.channels();
	cv::Mat hist_img(cv::Size(ch_width * sch, 100), CV_8UC3, cv::Scalar::all(255));

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


	//std::cout << "hist[0] = " <<hist[0] << std::endl;
	//std::cout << "typeid( hist[0]).name() = " << typeid(hist[0]).name() << std::endl;
	hist[0].at<float>(0, 255) = 0;
	std::cout << " hist[0].at<float>(0,255) = " << hist[0].at<float>(0, 255)<< std::endl;
	//std::cout << "hist. = " << hist.at<double>(0,255) << std::endl;
	//hist.at<double>=
	/*
	std::cout << "hist[0] = " << hist[0] << std::endl;
	int sum = 0;
	for (int i = 0; i < hist[0].rows;i++) {
		std::cout << i << std::endl;
		std::cout << "hist[i] = " << hist[0].at<float>(i,0) << std::endl;
		sum+=hist[0].at<float>(i, 0);
	}
	std::cout << "sum = " << sum << std::endl;
	*/

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
	//cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("Histogram", CV_WINDOW_AUTOSIZE);
	//cv::imshow("Image", src_img);
	//cv::imshow("Histogram", hist_img);
	//cv::waitKey(0);

	//return hist;
	return std::forward_as_tuple(hist, hist_img);
}




cv::Mat kMeans(cv::Mat src_img)
{
	const int cluster_count = 3; /* number of cluster */
	
	// (1)load a specified file as a 3-channel color image
	//cv::Mat src_img = cv::imread("data\\20200119\\5\\color.png");
	if (!src_img.data)
		return src_img;
	
	// (2)reshape the image to be a 1 column cv::Matrix 
	cv::Mat points;
	src_img.convertTo(points, CV_32FC3);
	//src_img.convertTo(points, CV_32FC1);
	//points = points.reshape(3, src_img.rows*src_img.cols);
	points = points.reshape(1, src_img.rows*src_img.cols);
	
	// (3)run k-means clustering algorithm to segment pixels in RGB color space
	cv::Mat_<int> clusters(points.size(), CV_32SC1);
	cv::Mat centers;
	kmeans(points, cluster_count, clusters,
		cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 1, cv::KMEANS_PP_CENTERS, centers);//停止条件
	
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





