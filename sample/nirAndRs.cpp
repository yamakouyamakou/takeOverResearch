#include"Stdafx.h"
#include"iostream"
#include<map>
#include<Eigen/Core>
#include<Eigen/Dense>
#include<fstream>
#include <stdio.h>
#include <string>

#include"MyForm.h"
//#include"realsense.cpp"
#include <librealsense2/rs.hpp>

#include <algorithm> //to replace


void nirExposurekai() {
	// A start of the exposure
	System::Int32 st=400;
	//extern System::Windows::Forms::NumericUpDown^  ShutterTime;
	//HBITMAP hBmp = sample::BitranCCDlibStartExposure(System::Convert::ToInt32(ShutterTime->Text), 1, 0, 0, 0, 0, 0);//撮影開始
	HBITMAP hBmp = sample::BitranCCDlibStartExposure(st, 1, 0, 0, 0, 0, 0);//撮影開始
	//HBITMAP hBmp = sample::BitranCCDlibStartExposure(ShutterTime, 1, 0, 0, 0, 0, 0);//撮影開始
	if (hBmp == 0)
		std::cout<<"aaoegrk\n";

	std::cout << "start\n";
	static bool StartFlag=true, TimerBusy = false;
	int count = 1;
	for (; StartFlag && hBmp; count++)
	{
		std::cout << "in for\n";
		cv::Mat mat = cvwin::BitmapToMat(hBmp);
		cv::imshow("mat",mat);
	
		DeleteObject(hBmp);
		hBmp = sample::BitranCCDlibStartExposure(st, 2, 0, 0, 0, 0, 0);
	}
}


// akagawa unndai
void coordiTransRs2NirAuto2()
{
	std::cout << "start coordiTransRs2Nir()\n";

	//////////////////////////////////////////   NIR    ////////////////////////////////
	cv::Mat nirCamMat, nirDistCoeffs;
	cv::FileStorage fs0("parameter\\instrinsics_Nir.xml", cv::FileStorage::READ);
	fs0["camera_matrix"] >> nirCamMat;
	fs0["distortion_coefficients"] >> nirDistCoeffs;
	std::cout << "nirCamMat = " << nirCamMat << std::endl;
	std::cout << "nirDistCoeffs = " << nirDistCoeffs << std::endl;

	int bai = 1.0;
	nirCamMat.at<double>(0, 0) = nirCamMat.at<double>(0, 0)*bai;
	nirCamMat.at<double>(1, 1) = nirCamMat.at<double>(1, 1)*bai;
	std::cout << "nirCamMat_kai = " << nirCamMat << std::endl;

	cv::Size nirImgSize{ 128, 128 };
	cv::Mat map1, map2;
	cv::Mat mapR;// = cv::Mat::eye(3, 3, CV_64F);
	cv::initUndistortRectifyMap(
		nirCamMat,//3*3のカメラ行列
		nirDistCoeffs,//4,5,または8個の歪み係数
		mapR,//R,平行化変換（3*3）
		nirCamMat,//新しいカメラ行列
		nirImgSize,//歪み補正された画像のサイズ
		CV_16SC2,//map1の型；cv_16sc2,cv_32FC1,cv_32C2
		map1,//最初の出力マップ
		map2//二番目の出力マップ
	);

	HBITMAP hBmp = sample::BitranCCDlibStartExposure(100, 1, 0, 0, 0, 0, 0);//撮影開始
	if (hBmp == 0)
		std::cout << "aaoegrk\n";


	/////////////////////////////////////////      R S       //////////////////////////////////////////////

	std::cout << "define filtre\n";//-------------------------fileter
	rs2::decimation_filter dec_filter;
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
	rs2::disparity_transform depth_to_disparity(true);
	rs2::disparity_transform disparity_to_depth(false);
	rs2::spatial_filter spat_filter;
	spat_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.5);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	spat_filter.set_option(RS2_OPTION_HOLES_FILL, 0);
	rs2::temporal_filter temp_filter;
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.4);
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	temp_filter.set_option(RS2_OPTION_HOLES_FILL, 3);
	rs2::hole_filling_filter hf_filter;
	hf_filter.set_option(RS2_OPTION_HOLES_FILL, 1);


	cv::FileStorage fs1("parameter\\rsCalibResult0.yml", cv::FileStorage::READ);
	cv::Mat rs_cam_mat, rs_dist_coeffs;
	fs1["cam_matrix"] >> rs_cam_mat;
	fs1["distortion_coeffs"] >> rs_dist_coeffs;
	fs1.release();
	std::cout << "rs_cam_mat = " << rs_cam_mat << std::endl;
	std::cout << "rs_dist_coeffs = " << rs_dist_coeffs << std::endl;

	cv::Size rsImgSize{ 1280,720 };
	cv::Mat map3, map4, mapR2;
	cv::initUndistortRectifyMap(
		rs_cam_mat,//入力されるカメラ行列
		rs_dist_coeffs,//入力される歪み係数ベクトル
		mapR2,           //オプション．物体空間における平行化変換
		rs_cam_mat,//新しいカメラ行列
		rsImgSize,//歪み補整された画像サイズ
		CV_16SC2,// 1番目の出力マップの型． CV_32FC1 または CV_16SC2 のいずれか
		map3,
		map4
	);

	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	int dev_id = 0;
	rs2::device dev = devices[dev_id];


	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
	cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);
	cfg.enable_device(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;
	// Start streaming with default recommended configuration
	auto profile = pipe.start(cfg);
	//rs2::stream_profile profile = pipe.start(cfg);

	auto depth_stream = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
	auto depth_intr = depth_stream.get_intrinsics();




	///////////////////////////////////////////////////////////////////////////////////
	cv::FileStorage fs2("parameter\\stereoCalibResult.yml", cv::FileStorage::READ);
	cv::Mat initR, T;
	fs2["R"] >> initR;
	fs2["T"] >> T;
	fs2.release();



	std::cout << "original_T = " << T << std::endl;
	T.at<double>(0, 0) = 0.0;
	T.at<double>(1, 0) = 0.135;
	//T.at<double>(2, 0) = -0.11;
	//T.at<double>(0, 0) = 0.0;
	//T.at<double>(1, 0) = 0.18;
	//T.at<double>(2, 0) = -1.1 + 1.4;
	//T.at<double>(2, 0) = -0.1;
	T.at<double>(2, 0) = 1.1;
	//T.at<double>(2, 0) = 1.75;
	std::cout << "mod_T = " << T << std::endl;

	cv::Mat oR;
	cv::Rodrigues(initR, oR);
	std::cout << "oR = " << oR << std::endl;
	oR.at<double>(0, 0) = 0.0;
	oR.at<double>(1, 0) = 0.0;
	oR.at<double>(2, 0) = 0.0;
	//cv::Rodrigues(rodR, initR);
	std::cout << "oR = " << oR << std::endl;

	////////////////////////////////
	std::map<std::string, double>nir_intr;
	std::string names[] = { "fx","ppx","fy","ppy" };
	nir_intr[names[0]] = nirCamMat.at<double>(0, 0);
	nir_intr[names[1]] = nirCamMat.at<double>(0, 2);
	nir_intr[names[2]] = nirCamMat.at<double>(1, 1);
	nir_intr[names[3]] = nirCamMat.at<double>(1, 2);

	std::cout << "nir_intr[fx] = " << nir_intr["fx"] << std::endl;
	std::cout << "nir_intr[ppx] = " << nir_intr["ppx"] << std::endl;
	std::cout << "nir_intr[fy] = " << nir_intr["fy"] << std::endl;
	std::cout << "nir_intr[ppy] = " << nir_intr["ppy"] << std::endl << std::endl;

	//cv::Mat transedImg = cv::Mat::zeros(720, 1280, CV_8UC1);
	cv::Mat transedImg(720, 1280, CV_8UC1, cv::Scalar(125));
	cv::Mat transedImgTest = cv::imread("coordiTransRs2Nir\\transedImgTest.png", 0);

	std::ofstream pcNir("pointCloud\\transedNir.csv.");
	//std::ofstream pcColorRs("pointCloud\\pcColorRs.csv");
	//std::ofstream pcGrayRs("pointCloud\\pcGrayRs.csv");

	int count = 1;
	int num = 0;
	bool getRsInf = 1;
	cv::Mat checkList = cv::Mat::zeros(720, 1280, CV_8UC1);
	std::cout << "//////////////////////////////////// START ///////////////////////////////////////////\n";
	for (int n = 0; hBmp; n++)
	{
		//std::cout << "in for\n";
		cv::Mat nir = cvwin::BitmapToMat(hBmp);
		cv::remap(nir, nir, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
		DeleteObject(hBmp);
		hBmp = sample::BitranCCDlibStartExposure(100, 2, 0, 0, 0, 0, 0);
		cv::Mat grayNir;
		cvtColor(nir, grayNir, CV_RGB2GRAY);

		/////////////////////////////////////////////////////////////////////

		rs2::frameset frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

		rs2::align align(RS2_STREAM_COLOR);
		auto aligned_frames = align.process(frames);

		rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();// rs2::depth_frame じゃないとget_distance できない
		rs2::frame depth_image = aligned_frames.get_depth_frame().apply_filter(color_map);// rs2::depth_frame じゃないとget_distance できない
		rs2::frame color_frame = aligned_frames.get_color_frame();

		// Query frame size (width and height)
		const int w = depth_frame.as<rs2::video_frame>().get_width();
		const int h = depth_frame.as<rs2::video_frame>().get_height();
		//std::cout << "wRs = " << w << ", hRs = " << h << std::endl;

		//fileter
		depth_frame = dec_filter.process(depth_frame);
		depth_frame = depth_to_disparity.process(depth_frame);
		depth_frame = spat_filter.process(depth_frame);
		depth_frame = temp_filter.process(depth_frame);
		depth_frame = disparity_to_depth.process(depth_frame);
		depth_frame = hf_filter.process(depth_frame);
		//std::wcout << "filter OK!\n";

		cv::Mat color = cv::Mat(cv::Size(w, h), CV_8UC3, const_cast<void*>(color_frame.get_data()));
		cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_image.get_data());

		cv::remap(color, color, map3, map4, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());

		cv::Mat gray;
		cvtColor(color, gray, CV_RGB2GRAY);


		cv::Mat centerDepth = color.clone();
		circle(centerDepth, cv::Point(w / 2 + 20, h / 2 - 20), 10, cv::Scalar(0, 200, 0), 3, 4);

		float depthInf0 = depth_frame.get_distance(w / 2 / 3 + 20, h / 2 / 3 - 20);//3*3サブピクセルでフィルタしてるから3で割る,cam1
		//std::cout << "depthInf0 = " << depthInf0 << std::endl;

		//cv::Mat vecR;
		//cv::Rodrigues(R, vecR);
		//std::cout << "vecR = " <<vecR << std::endl;

		//////////////////////////////////////////////////////////////////////////

		std::ifstream ifs("shared_folder_ubuntu\\switch.txt");
		std::string str;
		ifs >> str;
		//std::cout << "str = " << str << std::endl;
		ifs.close();


		//cv::waitKey(500);
		if (str == "0") {
			//if (GetAsyncKeyState(VK_UP)){//include<windows.h>



			std::cout << "oR = " << oR << std::endl << std::endl;//ここは常に0
			///// ここでshared_folder_ubuntu を読み込む
			cv::Mat Rd;
			cv::FileStorage fs3("C:\\Users\\yamak\\source\\repos\\jedi2\\shared_folder_ubuntu\\sharedExPara.yml", cv::FileStorage::READ);
			fs3["R"] >> Rd;

			std::ifstream fs4("shared_folder_ubuntu\\panDeg.txt");
			double pan;
			fs4 >> pan;
			if (num > 7)pan += 0;
			std::cout << "pan = " << pan << std::endl;
			Rd.at<double>(0, 1) = -pan;


			std::ifstream fs5("shared_folder_ubuntu\\tiltDeg.txt");
			double b;
			fs5 >> b;
			std::cout << "b = " << b << std::endl;
			Rd.at<double>(0, 0) = b;

			std::cout << "Rd(degree) = " << Rd << std::endl << std::endl;

			Rd.at<double>(0, 0) = Rd.at<double>(0, 0)*(3.14159 / 180);//degree to radian
			Rd.at<double>(1, 0) = Rd.at<double>(1, 0)*(3.14159 / 180);
			Rd.at<double>(2, 0) = Rd.at<double>(2, 0)*(3.14159 / 180);
			std::cout << "Rd(radian) = " << Rd << std::endl << std::endl;

			cv::Mat vecR;
			//vecR += Rd;
			vecR = Rd;
			std::cout << "vecR = " << vecR << std::endl << std::endl;

			cv::Mat R;
			cv::Rodrigues(vecR, R);//回転行列にする
			std::cout << "R = " << R << std::endl << std::endl;

			std::cout << "calc eR\n";//----------------------------------------calc eR
			cv::Mat tR = R.t();
			cv::Mat tT = T.t();
			tR.push_back(tT);
			std::cout << "tR=" << tR << std::endl;
			cv::Mat RT;
			RT = tR.t();
			std::cout << "RT=" << RT << std::endl << std::endl;


			cv::Mat camMatRT = nirCamMat * RT;

			std::cout << "camMatRT=" << camMatRT << std::endl << std::endl;

			std::cout << "trans start\n";
			for (int n = 0; n < gray.total(); n++) {
				if (n == 1)std::cout << "n==1\n";
				int x = n % gray.cols;
				int y = n / gray.cols;
				//std::cout << "x = " << x << ", y= " << y << std::endl;
				//std::cout << " n= " << n << std::endl;

				float rsPixel[2] = { (float)x, (float)y };
				float depthInf = depth_frame.get_distance(rsPixel[0] / 3.0, rsPixel[1] / 3.0);//3*3サブピクセルでフィルタしてるから3で割る,cam1

				float rsPoint[3];
				rs2_deproject_pixel_to_point(rsPoint, &depth_intr, rsPixel, depthInf);//cam1からの三次元情報
				//std::cout << "rsPoint[0]=" << rsPoint[0] <<  std::endl;
				//std::cout << "rsPoint[1]=" << rsPoint[1] <<  std::endl;
				//std::cout << "rsPoint[2]=" << rsPoint[2] << std::endl << std::endl;

				cv::Mat rsPointMat = cv::Mat::zeros(4, 1, CV_64F);//Matで計算できるように配列からcv::Mat
				rsPointMat.at<double>(0, 0) = rsPoint[0];
				rsPointMat.at<double>(1, 0) = rsPoint[1];
				rsPointMat.at<double>(2, 0) = rsPoint[2];
				rsPointMat.at<double>(3, 0) = 1;
				/*
				std::cout << "rsPointMat.at<double>(0,0)=" << rsPointMat.at<double>(0, 0)  << std::endl;
				std::cout << "rsPointMat.at<double>(1,0)=" << rsPointMat.at<double>(1, 0)  << std::endl;
				std::cout << "rsPointMat.at<double>(2,0)=" << rsPointMat.at<double>(2, 0)  << std::endl;
				std::cout << "rsPointMat.at<double>(3,0)=" << rsPointMat.at<double>(3, 0) << std::endl << std::endl;
				std::cout << "rsPointMat.width=" << rsPointMat.size() << std::endl << std::endl;
				*/

				cv::Mat nirPixel = camMatRT * rsPointMat;
								
				nirPixel.at<double>(0, 0) = nirPixel.at<double>(0, 0) / nirPixel.at<double>(2, 0);
				nirPixel.at<double>(1, 0) = nirPixel.at<double>(1, 0) / nirPixel.at<double>(2, 0);
				nirPixel.at<double>(2, 0) = nirPixel.at<double>(2, 0) / nirPixel.at<double>(2, 0);

				/*
				std::cout << "nirPixel.size()=" << nirPixel.size() << std::endl;
				std::cout << "nirPixel.at<double>(0,0)=" << nirPixel.at<double>(0, 0) << std::endl;
				std::cout << "nirPixel.at<double>(1,0)=" << nirPixel.at<double>(1, 0) << std::endl;
				std::cout << "nirPixel.at<double>(2,0)=" << nirPixel.at<double>(2, 0) << std::endl << std::endl;
				*/

				if (0 <= nirPixel.at<double>(0,0) && nirPixel.at<double>(0,0) <= 128 && 0 <= nirPixel.at<double>(1,0) && nirPixel.at<double>(1,0) <= 128 && checkList.at<uchar>(y, x) == 0) {
					transedImg.at<uchar>(y, x) = grayNir.at<uchar>(nirPixel.at<double>(1,0), nirPixel.at<double>(0,0));
					transedImgTest.at<uchar>(y, x) = grayNir.at<uchar>(nirPixel.at<double>(1,0), nirPixel.at<double>(0,0));

					pcNir << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," <<
						int(grayNir.at<uchar>(nirPixel.at<double>(1,0), nirPixel.at<double>(0,0))) << "," <<
						int(grayNir.at<uchar>(nirPixel.at<double>(1,0), nirPixel.at<double>(0,0))) << "," <<
						int(grayNir.at<uchar>(nirPixel.at<double>(1,0), nirPixel.at<double>(0,0))) << std::endl;

					checkList.at<uchar>(y, x) += 50;
				}
				/*
				//alpha brending
				else if (0 <= nirPixel.at<double>(0, 0) && nirPixel.at<double>(0, 0) <= 128 && 0 <= nirPixel.at<double>(1, 0) && nirPixel.at<double>(1, 0) <= 128 && checkList.at<uchar>(y, x) > 0) {
					float a=0.5;
					double data = transedImg.at<uchar>(y, x)*a + grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0)) * (1 - a);
					
					transedImg.at<uchar>(y, x) = data;
					transedImgTest.at<uchar>(y, x) = data;
					pcNir << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," << data << "," << data << "," << data << std::endl;
					checkList.at<uchar>(y, x) += 50;
				}
				*/
			}
			std::cout << "trans end\n";
			std::cout << "num = " << num << std::endl << std::endl;
			std::cout << "----------------------------------------------------------\n";

			cv::imwrite("coordiTransRs2Nir\\transedImg.png", transedImg);
			cv::imwrite("coordiTransRs2Nir\\transedImgTest.png", transedImgTest);
			cv::imwrite("coordiTransRs2Nir\\grayNir" + std::to_string(num) + ".png", grayNir);
			cv::imwrite("coordiTransRs2Nir\\color.png", color);
			cv::imwrite("coordiTransRs2Nir\\depth.png", depth);
			cv::imwrite("coordiTransRs2Nir\\gray.png", gray);
			cv::imwrite("coordiTransRs2Nir\\checkList.png", checkList);

			std::ofstream ifs2("shared_folder_ubuntu\\switch.txt");
			ifs2 << "1";
			ifs2.close();

			num++;

		}
		getRsInf = 0;

		cv::namedWindow("nir", CV_WINDOW_NORMAL);
		cv::imshow("color", color);
		cv::imshow("centerDepth", centerDepth);
		cv::imshow("gray", gray);
		cv::imshow("depth", depth);
		cv::imshow("transedImgTest.png", transedImgTest);
		cv::imshow("transedImg.png", transedImg);
		cv::imshow("nir", grayNir);
		cv::imwrite("checkList.png", checkList);

		cv::waitKey(1);

	}

}



//rsの一つのフレームで座標変換する
void coordiTransRs2NirPanTilt2()
{
	std::cout << "start coordiTransRs2Nir2()\n";

	//////////////////////////////////////////   NIR    ////////////////////////////////
	cv::Mat nirCamMat, nirDistCoeffs;
	cv::FileStorage fs0("parameter\\instrinsics_Nir.xml", cv::FileStorage::READ);
	fs0["camera_matrix"] >> nirCamMat;
	fs0["distortion_coefficients"] >> nirDistCoeffs;
	std::cout << "nirCamMat = " << nirCamMat << std::endl;
	std::cout << "nirDistCoeffs = " << nirDistCoeffs << std::endl;

	int bai = 1.0;
	nirCamMat.at<double>(0, 0) = nirCamMat.at<double>(0, 0)*bai;
	nirCamMat.at<double>(1, 1) = nirCamMat.at<double>(1, 1)*bai;
	std::cout << "nirCamMat_kai = " << nirCamMat << std::endl;

	cv::Size nirImgSize{ 128, 128 };
	cv::Mat map1, map2;
	cv::Mat mapR;// = cv::Mat::eye(3, 3, CV_64F);
	cv::initUndistortRectifyMap(
		nirCamMat,//3*3のカメラ行列
		nirDistCoeffs,//4,5,または8個の歪み係数
		mapR,//R,平行化変換（3*3）
		nirCamMat,//新しいカメラ行列
		nirImgSize,//歪み補正された画像のサイズ
		CV_16SC2,//map1の型；cv_16sc2,cv_32FC1,cv_32C2
		map1,//最初の出力マップ
		map2//二番目の出力マップ
	);
	int st = 250;//******************************************************************************************
	HBITMAP hBmp = sample::BitranCCDlibStartExposure(st, 1, 0, 0, 0, 0, 0);//撮影開始
	if (hBmp == 0)
		std::cout << "aaoegrk\n";


	/////////////////////////////////////////      R S       //////////////////////////////////////////////

	std::cout << "define filtre\n";//-------------------------fileter
	rs2::decimation_filter dec_filter;
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
	rs2::disparity_transform depth_to_disparity(true);
	rs2::disparity_transform disparity_to_depth(false);
	rs2::spatial_filter spat_filter;
	spat_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.5);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	spat_filter.set_option(RS2_OPTION_HOLES_FILL, 0);
	rs2::temporal_filter temp_filter;
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.4);
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	temp_filter.set_option(RS2_OPTION_HOLES_FILL, 3);
	rs2::hole_filling_filter hf_filter;
	hf_filter.set_option(RS2_OPTION_HOLES_FILL, 1);


	cv::FileStorage fs1("parameter\\rsCalibResult0.yml", cv::FileStorage::READ);
	cv::Mat rs_cam_mat, rs_dist_coeffs;
	fs1["cam_matrix"] >> rs_cam_mat;
	fs1["distortion_coeffs"] >> rs_dist_coeffs;
	fs1.release();
	std::cout << "rs_cam_mat = " << rs_cam_mat << std::endl;
	std::cout << "rs_dist_coeffs = " << rs_dist_coeffs << std::endl;

	cv::Size rsImgSize{ 1280,720 };
	cv::Mat map3, map4, mapR2;
	cv::initUndistortRectifyMap(
		rs_cam_mat,//入力されるカメラ行列
		rs_dist_coeffs,//入力される歪み係数ベクトル
		mapR2,           //オプション．物体空間における平行化変換
		rs_cam_mat,//新しいカメラ行列
		rsImgSize,//歪み補整された画像サイズ
		CV_16SC2,// 1番目の出力マップの型． CV_32FC1 または CV_16SC2 のいずれか
		map3,
		map4
	);

	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	int dev_id = 0;
	rs2::device dev = devices[dev_id];


	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);
	cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 15);
	cfg.enable_device(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;
	// Start streaming with default recommended configuration
	auto profile = pipe.start(cfg);
	//rs2::stream_profile profile = pipe.start(cfg);

	auto depth_stream = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
	auto depth_intr = depth_stream.get_intrinsics();




	///////////////////////////////////////////////////////////////////////////////////
	cv::FileStorage fs2("parameter\\stereoCalibResult.yml", cv::FileStorage::READ);
	cv::Mat initR, T;
	fs2["R"] >> initR;
	fs2["T"] >> T;
	fs2.release();



	std::cout << "original_T = " << T << std::endl;
	T.at<double>(0, 0) = 0.0;
	T.at<double>(1, 0) = 0.195039 - 0.1;//小さくするとNIRは下に行く
	T.at<double>(2, 0) = 1.7;
	std::cout << "mod_T = " << T << std::endl;

	cv::Mat oR;
	cv::Rodrigues(initR, oR);
	std::cout << "oR = " << oR << std::endl;
	oR.at<double>(0, 0) = 0.0;
	oR.at<double>(1, 0) = 0.0;
	oR.at<double>(2, 0) = 0.0;
	std::cout << "oR = " << oR << std::endl;

	////////////////////////////////
	std::map<std::string, double>nir_intr;
	std::string names[] = { "fx","ppx","fy","ppy" };
	nir_intr[names[0]] = nirCamMat.at<double>(0, 0);
	nir_intr[names[1]] = nirCamMat.at<double>(0, 2);
	nir_intr[names[2]] = nirCamMat.at<double>(1, 1);
	nir_intr[names[3]] = nirCamMat.at<double>(1, 2);

	std::cout << "nir_intr[fx] = " << nir_intr["fx"] << std::endl;
	std::cout << "nir_intr[ppx] = " << nir_intr["ppx"] << std::endl;
	std::cout << "nir_intr[fy] = " << nir_intr["fy"] << std::endl;
	std::cout << "nir_intr[ppy] = " << nir_intr["ppy"] << std::endl << std::endl;

	//cv::Mat transedImg = cv::Mat::zeros(720, 1280, CV_8UC1);
	cv::Mat transedImg(720, 1280, CV_8UC1, cv::Scalar(125));
	cv::Mat A_brendingImg(720, 1280, CV_8UC1, cv::Scalar(125));
	cv::Mat transedImgTest = cv::imread("coordiTransRs2Nir\\transedImgTest.png", 0);

	std::ofstream pcNir("pointCloud\\transedNir.csv.");
	//std::ofstream pcColorRs("pointCloud\\pcColorRs.csv");
	//std::ofstream pcGrayRs("pointCloud\\pcGrayRs.csv");

	int count = 1;
	int num = 0;
	bool getRsInf = 1;
	cv::Mat checkList = cv::Mat::zeros(720, 1280, CV_8UC1);

	std::ofstream initSw("C:\\Users\\yamak\\source\\repos\\jedi2\\shared_folder_ubuntu\\switch.txt");
	initSw << "1";
	initSw.close();

	std::cout << "//////////////////////////////////// START ///////////////////////////////////////////\n";
	for (int n = 0; hBmp; n++)
	{
		//std::cout << "in for\n";
		cv::Mat nir = cvwin::BitmapToMat(hBmp);
		cv::remap(nir, nir, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
		DeleteObject(hBmp);
		hBmp = sample::BitranCCDlibStartExposure(st, 2, 0, 0, 0, 0, 0);
		cv::Mat grayNir;
		cvtColor(nir, grayNir, CV_RGB2GRAY);

		/////////////////////////////////////////////////////////////////////

		rs2::frameset frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

		rs2::align align(RS2_STREAM_COLOR);
		auto aligned_frames = align.process(frames);

		rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();// rs2::depth_frame じゃないとget_distance できない
		rs2::frame depth_image = aligned_frames.get_depth_frame().apply_filter(color_map);// rs2::depth_frame じゃないとget_distance できない
		rs2::frame color_frame = aligned_frames.get_color_frame();

		// Query frame size (width and height)
		const int w = depth_frame.as<rs2::video_frame>().get_width();
		const int h = depth_frame.as<rs2::video_frame>().get_height();

		//fileter
		depth_frame = dec_filter.process(depth_frame);
		depth_frame = depth_to_disparity.process(depth_frame);
		depth_frame = spat_filter.process(depth_frame);
		depth_frame = temp_filter.process(depth_frame);
		depth_frame = disparity_to_depth.process(depth_frame);
		depth_frame = hf_filter.process(depth_frame);

		cv::Mat color = cv::Mat(cv::Size(w, h), CV_8UC3, const_cast<void*>(color_frame.get_data()));
		cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_image.get_data());

		cv::remap(color, color, map3, map4, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());

		cv::Mat gray;
		cvtColor(color, gray, CV_RGB2GRAY);


		cv::Mat centerDepth = color.clone();
		circle(centerDepth, cv::Point(w / 2 + 20, h / 2 - 20), 10, cv::Scalar(0, 200, 0), 3, 4);

		float depthInf0 = depth_frame.get_distance(w / 2 / 3 + 20, h / 2 / 3 - 20);//3*3サブピクセルでフィルタしてるから3で割る,cam1
	
		//////////////////////////////////////////////////////////////////////////

		std::ifstream ifs("shared_folder_ubuntu\\switch.txt");
		std::string str;
		ifs >> str;
		ifs.close();


		if (str == "0") {//ここからRSを止めたい、というか止まる．
			while (true) {
				//ここでSTR読み込みと、新しいNIR作る
				std::ifstream ifsStr("shared_folder_ubuntu\\switch.txt");
				std::string str2;
				ifsStr >> str2;
				ifsStr.close();

				cv::Mat nir2 = cvwin::BitmapToMat(hBmp);
				cv::remap(nir2, nir2, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
				DeleteObject(hBmp);
				hBmp = sample::BitranCCDlibStartExposure(st, 2, 0, 0, 0, 0, 0);
				cv::Mat grayNir2;
				cvtColor(nir2, grayNir2, CV_RGB2GRAY);


				if (str2 == "0") {

					std::cout << "oR = " << oR << std::endl << std::endl;//ここは常に0
					///// ここでshared_folder_ubuntu を読み込む
					cv::Mat Rd;
					cv::FileStorage fs3("C:\\Users\\yamak\\source\\repos\\jedi2\\shared_folder_ubuntu\\sharedExPara.yml", cv::FileStorage::READ);
					fs3["R"] >> Rd;

					std::ifstream fs4("shared_folder_ubuntu\\panDeg.txt");
					double pan;
					fs4 >> pan;
					std::cout << "pan = " << pan << std::endl << std::endl;
					Rd.at<double>(0, 1) = pan;


					std::ifstream fs5("shared_folder_ubuntu\\tiltDeg.txt");
					double tilt;
					fs5 >> tilt;
					std::cout << "tilt = " << tilt << std::endl << std::endl;
					Rd.at<double>(0, 0) = -tilt;
					if (tilt <= -28)break;

					std::cout << "Rd(degree) = " << Rd << std::endl << std::endl;

					Rd.at<double>(0, 0) = Rd.at<double>(0, 0)*(3.14159 / 180);//degree to radian
					Rd.at<double>(1, 0) = Rd.at<double>(1, 0)*(3.14159 / 180);
					Rd.at<double>(2, 0) = Rd.at<double>(2, 0)*(3.14159 / 180);
					std::cout << "Rd(radian) = " << Rd << std::endl << std::endl;

					cv::Mat vecR;
					vecR = Rd;
					std::cout << "vecR = " << vecR << std::endl << std::endl;

					cv::Mat R;
					cv::Rodrigues(vecR, R);//回転行列にする
					std::cout << "R = " << R << std::endl << std::endl;

					std::cout << "calc eR\n";//----------------------------------------calc eR
					cv::Mat tR = R.t();
					cv::Mat tT = T.t();
					tR.push_back(tT);
					std::cout << "tR=" << tR << std::endl;
					cv::Mat RT;
					RT = tR.t();
					std::cout << "RT=" << RT << std::endl << std::endl;


					cv::Mat camMatRT = nirCamMat * RT;

					std::cout << "camMatRT=" << camMatRT << std::endl << std::endl;

					std::cout << "trans start\n";
					for (int n = 0; n < gray.total(); n++) {
						if (n == 1)std::cout << "n==1\n";
						int x = n % gray.cols;
						int y = n / gray.cols;

						float rsPixel[2] = { (float)x, (float)y };
						float depthInf = depth_frame.get_distance(rsPixel[0] / 3.0, rsPixel[1] / 3.0);//3*3サブピクセルでフィルタしてるから3で割る,cam1


						float rsPoint[3];
						rs2_deproject_pixel_to_point(rsPoint, &depth_intr, rsPixel, depthInf);//cam1からの三次元情報

						cv::Mat rsPointMat = cv::Mat::zeros(4, 1, CV_64F);//Matで計算できるように配列からcv::Mat
						rsPointMat.at<double>(0, 0) = rsPoint[0];
						rsPointMat.at<double>(1, 0) = rsPoint[1];
						rsPointMat.at<double>(2, 0) = rsPoint[2];
						rsPointMat.at<double>(3, 0) = 1;

						cv::Mat nirPixel = camMatRT * rsPointMat;

						nirPixel.at<double>(0, 0) = nirPixel.at<double>(0, 0) / nirPixel.at<double>(2, 0);
						nirPixel.at<double>(1, 0) = nirPixel.at<double>(1, 0) / nirPixel.at<double>(2, 0);
						nirPixel.at<double>(2, 0) = nirPixel.at<double>(2, 0) / nirPixel.at<double>(2, 0);


						if (0 <= nirPixel.at<double>(0, 0) && nirPixel.at<double>(0, 0) <= 128 && 0 <= nirPixel.at<double>(1, 0) && nirPixel.at<double>(1, 0) <= 128 && checkList.at<uchar>(y, x) == 0) {
							transedImg.at<uchar>(y, x) = grayNir2.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0));
							
							pcNir << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," <<
								int(grayNir2.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0))) << "," <<
								int(grayNir2.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0))) << "," <<
								int(grayNir2.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0))) << std::endl;

							checkList.at<uchar>(y, x) += 50;
						}

						//alpha brending
						else if (0 <= nirPixel.at<double>(0, 0) && nirPixel.at<double>(0, 0) <= 128 && 0 <= nirPixel.at<double>(1, 0) && nirPixel.at<double>(1, 0) <= 128 && checkList.at<uchar>(y, x) > 0) {
							float a = 0.5;
							double data = transedImg.at<uchar>(y, x)*a + grayNir2.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0)) * (1 - a);

							transedImg.at<uchar>(y, x) = data;
							pcNir << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," << data << "," << data << "," << data << std::endl;
							checkList.at<uchar>(y, x) += 50;
						}

					}
					std::cout << "trans end\n";
					std::cout << "num = " << num << std::endl << std::endl;
					std::cout << "----------------------------------------------------------\n";

					cv::imwrite("coordiTransRs2Nir\\transedImg.png", transedImg);
					cv::imwrite("coordiTransRs2Nir\\transedImgTest.png", transedImgTest);
					cv::imwrite("coordiTransRs2Nir\\grayNir2" + std::to_string(num) + ".png", grayNir2);
					cv::imwrite("coordiTransRs2Nir\\color.png", color);
					cv::imwrite("coordiTransRs2Nir\\depth.png", depth);
					cv::imwrite("coordiTransRs2Nir\\gray.png", gray);
					cv::imwrite("coordiTransRs2Nir\\checkList.png", checkList);


					if (str == "0") {
						std::ofstream ifs2("shared_folder_ubuntu\\switch.txt");
						ifs2 << "1";
						ifs2.close();
					}

					num++;


				}//if2

				cv::namedWindow("nir", CV_WINDOW_NORMAL);
				cv::imshow("color", color);
				cv::imshow("centerDepth", centerDepth);
				cv::imshow("gray", gray);
				cv::imshow("depth", depth);
				cv::imshow("transedImg.png", transedImg);
				cv::imshow("nir", grayNir2);
				cv::imshow("checkList.png", checkList);

				cv::waitKey(1);

			}//while
		}
		getRsInf = 0;

		cv::namedWindow("nir", CV_WINDOW_NORMAL);
		cv::imshow("color", color);
		cv::imshow("centerDepth", centerDepth);
		cv::imshow("gray", gray);
		cv::imshow("depth", depth);
		cv::imshow("transedImg.png", transedImg);
		cv::imshow("nir", grayNir);
		cv::imshow("checkList.png", checkList);

		cv::waitKey(1);

	}

}



//首を振ってNIR撮るだけ(座標変換しない)
void getNirPanTilt()
{
	std::cout << "start coordiTransRs2Nir()\n";

	//////////////////////////////////////////   NIR    ////////////////////////////////
	cv::Mat nirCamMat, nirDistCoeffs;
	cv::FileStorage fs0("parameter\\instrinsics_Nir.xml", cv::FileStorage::READ);
	fs0["camera_matrix"] >> nirCamMat;
	fs0["distortion_coefficients"] >> nirDistCoeffs;
	std::cout << "nirCamMat = " << nirCamMat << std::endl;
	std::cout << "nirDistCoeffs = " << nirDistCoeffs << std::endl;

	int bai = 1.0;
	nirCamMat.at<double>(0, 0) = nirCamMat.at<double>(0, 0)*bai;
	nirCamMat.at<double>(1, 1) = nirCamMat.at<double>(1, 1)*bai;
	std::cout << "nirCamMat_kai = " << nirCamMat << std::endl;

	cv::Size nirImgSize{ 128, 128 };
	cv::Mat map1, map2;
	cv::Mat mapR;// = cv::Mat::eye(3, 3, CV_64F);
	cv::initUndistortRectifyMap(
		nirCamMat,//3*3のカメラ行列
		nirDistCoeffs,//4,5,または8個の歪み係数
		mapR,//R,平行化変換（3*3）
		nirCamMat,//新しいカメラ行列
		nirImgSize,//歪み補正された画像のサイズ
		CV_16SC2,//map1の型；cv_16sc2,cv_32FC1,cv_32C2
		map1,//最初の出力マップ
		map2//二番目の出力マップ
	);
	int st = 250;//******************************************************************************************
	HBITMAP hBmp = sample::BitranCCDlibStartExposure(st, 1, 0, 0, 0, 0, 0);//撮影開始
	if (hBmp == 0)
		std::cout << "aaoegrk\n";


	/////////////////////////////////////////      R S       //////////////////////////////////////////////

	std::cout << "define filtre\n";//-------------------------fileter
	rs2::decimation_filter dec_filter;
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
	rs2::disparity_transform depth_to_disparity(true);
	rs2::disparity_transform disparity_to_depth(false);
	rs2::spatial_filter spat_filter;
	spat_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.5);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	spat_filter.set_option(RS2_OPTION_HOLES_FILL, 0);
	rs2::temporal_filter temp_filter;
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.4);
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	temp_filter.set_option(RS2_OPTION_HOLES_FILL, 3);
	rs2::hole_filling_filter hf_filter;
	hf_filter.set_option(RS2_OPTION_HOLES_FILL, 1);


	cv::FileStorage fs1("parameter\\rsCalibResult0.yml", cv::FileStorage::READ);
	cv::Mat rs_cam_mat, rs_dist_coeffs;
	fs1["cam_matrix"] >> rs_cam_mat;
	fs1["distortion_coeffs"] >> rs_dist_coeffs;
	fs1.release();
	std::cout << "rs_cam_mat = " << rs_cam_mat << std::endl;
	std::cout << "rs_dist_coeffs = " << rs_dist_coeffs << std::endl;

	cv::Size rsImgSize{ 1280,720 };
	cv::Mat map3, map4, mapR2;
	cv::initUndistortRectifyMap(
		rs_cam_mat,//入力されるカメラ行列
		rs_dist_coeffs,//入力される歪み係数ベクトル
		mapR2,           //オプション．物体空間における平行化変換
		rs_cam_mat,//新しいカメラ行列
		rsImgSize,//歪み補整された画像サイズ
		CV_16SC2,// 1番目の出力マップの型． CV_32FC1 または CV_16SC2 のいずれか
		map3,
		map4
	);

	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	int dev_id = 0;
	rs2::device dev = devices[dev_id];


	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);
	cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 15);
	cfg.enable_device(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));

	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::pipeline pipe;
	// Start streaming with default recommended configuration
	auto profile = pipe.start(cfg);
	//rs2::stream_profile profile = pipe.start(cfg);

	auto depth_stream = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
	auto depth_intr = depth_stream.get_intrinsics();




	///////////////////////////////////////////////////////////////////////////////////
	cv::FileStorage fs2("parameter\\stereoCalibResult.yml", cv::FileStorage::READ);
	cv::Mat initR, T;
	fs2["R"] >> initR;
	fs2["T"] >> T;
	fs2.release();



	std::cout << "original_T = " << T << std::endl;
	T.at<double>(0, 0) = 0.0;
	T.at<double>(1, 0) = 0.195039 - 0.1;//小さくするとNIRは下に行く
	T.at<double>(2, 0) = 1.7;
	std::cout << "mod_T = " << T << std::endl;

	cv::Mat oR;
	cv::Rodrigues(initR, oR);
	std::cout << "oR = " << oR << std::endl;
	oR.at<double>(0, 0) = 0.0;
	oR.at<double>(1, 0) = 0.0;
	oR.at<double>(2, 0) = 0.0;
	std::cout << "oR = " << oR << std::endl;

	////////////////////////////////
	std::map<std::string, double>nir_intr;
	std::string names[] = { "fx","ppx","fy","ppy" };
	nir_intr[names[0]] = nirCamMat.at<double>(0, 0);
	nir_intr[names[1]] = nirCamMat.at<double>(0, 2);
	nir_intr[names[2]] = nirCamMat.at<double>(1, 1);
	nir_intr[names[3]] = nirCamMat.at<double>(1, 2);

	std::cout << "nir_intr[fx] = " << nir_intr["fx"] << std::endl;
	std::cout << "nir_intr[ppx] = " << nir_intr["ppx"] << std::endl;
	std::cout << "nir_intr[fy] = " << nir_intr["fy"] << std::endl;
	std::cout << "nir_intr[ppy] = " << nir_intr["ppy"] << std::endl << std::endl;

	cv::Mat transedImg(720, 1280, CV_8UC1, cv::Scalar(125));
	cv::Mat A_brendingImg(720, 1280, CV_8UC1, cv::Scalar(125));
	cv::Mat transedImgTest = cv::imread("coordiTransRs2Nir\\transedImgTest.png", 0);

	std::ofstream pcNir("pointCloud\\transedNir.csv.");
	//std::ofstream pcColorRs("pointCloud\\pcColorRs.csv");
	//std::ofstream pcGrayRs("pointCloud\\pcGrayRs.csv");

	int count = 1;
	int num = 0;
	bool getRsInf = 1;
	cv::Mat checkList = cv::Mat::zeros(720, 1280, CV_8UC1);

	std::ofstream initSw("shared_folder_ubuntu\\switch.txt");
	initSw << "1";
	initSw.close();

	std::cout << "//////////////////////////////////// START ///////////////////////////////////////////\n";
	for (int n = 0; hBmp; n++)
	{
		cv::Mat nir = cvwin::BitmapToMat(hBmp);
		cv::remap(nir, nir, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
		DeleteObject(hBmp);
		hBmp = sample::BitranCCDlibStartExposure(st, 2, 0, 0, 0, 0, 0);
		cv::Mat grayNir;
		cvtColor(nir, grayNir, CV_RGB2GRAY);

		/////////////////////////////////////////////////////////////////////

		rs2::frameset frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

		rs2::align align(RS2_STREAM_COLOR);
		auto aligned_frames = align.process(frames);

		rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();// rs2::depth_frame じゃないとget_distance できない
		rs2::frame depth_image = aligned_frames.get_depth_frame().apply_filter(color_map);// rs2::depth_frame じゃないとget_distance できない
		rs2::frame color_frame = aligned_frames.get_color_frame();

		// Query frame size (width and height)
		const int w = depth_frame.as<rs2::video_frame>().get_width();
		const int h = depth_frame.as<rs2::video_frame>().get_height();
	
		//fileter
		depth_frame = dec_filter.process(depth_frame);
		depth_frame = depth_to_disparity.process(depth_frame);
		depth_frame = spat_filter.process(depth_frame);
		depth_frame = temp_filter.process(depth_frame);
		depth_frame = disparity_to_depth.process(depth_frame);
		depth_frame = hf_filter.process(depth_frame);
		
		cv::Mat color = cv::Mat(cv::Size(w, h), CV_8UC3, const_cast<void*>(color_frame.get_data()));
		cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_image.get_data());

		cv::remap(color, color, map3, map4, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());

		cv::Mat gray;
		cvtColor(color, gray, CV_RGB2GRAY);


		cv::Mat centerDepth = color.clone();
		circle(centerDepth, cv::Point(w / 2 + 20, h / 2 - 20), 10, cv::Scalar(0, 200, 0), 3, 4);

		float depthInf0 = depth_frame.get_distance(w / 2 / 3 + 20, h / 2 / 3 - 20);//3*3サブピクセルでフィルタしてるから3で割る,cam1
		
		//////////////////////////////////////////////////////////////////////////

		std::ifstream ifs("shared_folder_ubuntu\\switch.txt");
		std::string str;
		ifs >> str;
		ifs.close();




		//cv::waitKey(500);
		if (str == "0") {

			std::cout << "oR = " << oR << std::endl << std::endl;//ここは常に0
			
			///// ここでshared_folder_ubuntu を読み込む
			cv::Mat Rd;
			cv::FileStorage fs3("shared_folder_ubuntu\\sharedExPara.yml", cv::FileStorage::READ);
			fs3["R"] >> Rd;

			std::ifstream fs4("shared_folder_ubuntu\\panDeg.txt");
			double pan;
			fs4 >> pan;
			std::cout << "pan = " << pan << std::endl;
			Rd.at<double>(0, 1) = pan;


			std::ifstream fs5("shared_folder_ubuntu\\tiltDeg.txt");
			double tilt;
			fs5 >> tilt;
			std::cout << "tilt = " << tilt << std::endl;
			Rd.at<double>(0, 0) = -tilt;
			if (tilt <= -28)break;

			std::cout << "Rd(degree) = " << Rd << std::endl << std::endl;

			Rd.at<double>(0, 0) = Rd.at<double>(0, 0)*(3.14159 / 180);//degree to radian
			Rd.at<double>(1, 0) = Rd.at<double>(1, 0)*(3.14159 / 180);
			Rd.at<double>(2, 0) = Rd.at<double>(2, 0)*(3.14159 / 180);
			std::cout << "Rd(radian) = " << Rd << std::endl << std::endl;

			cv::Mat vecR;
			vecR = Rd;
			std::cout << "vecR = " << vecR << std::endl << std::endl;

			cv::Mat R;
			cv::Rodrigues(vecR, R);//回転行列にする
			std::cout << "R = " << R << std::endl << std::endl;

			std::cout << "calc eR\n";//----------------------------------------calc eR
			cv::Mat tR = R.t();
			cv::Mat tT = T.t();
			tR.push_back(tT);
			std::cout << "tR=" << tR << std::endl;
			cv::Mat RT;
			RT = tR.t();
			std::cout << "RT=" << RT << std::endl << std::endl;


			cv::Mat camMatRT = nirCamMat * RT;

			std::cout << "camMatRT=" << camMatRT << std::endl << std::endl;

			std::cout << "num = " << num << std::endl << std::endl;
			std::cout << "----------------------------------------------------------\n";

			cv::imwrite("coordiTransRs2Nir\\transedImg.png", transedImg);
			cv::imwrite("coordiTransRs2Nir\\transedImgTest.png", transedImgTest);
			cv::imwrite("coordiTransRs2Nir\\grayNir" + std::to_string(num) + ".png", grayNir);
			cv::imwrite("coordiTransRs2Nir\\color.png", color);
			cv::imwrite("coordiTransRs2Nir\\depth.png", depth);
			cv::imwrite("coordiTransRs2Nir\\gray.png", gray);
			cv::imwrite("coordiTransRs2Nir\\checkList.png", checkList);

			if (str == "0") {
				std::ofstream ifs2("shared_folder_ubuntu\\switch.txt");
				ifs2 << "1";
				ifs2.close();
			}


			num++;

		}
		getRsInf = 0;

		cv::namedWindow("nir", CV_WINDOW_NORMAL);
		cv::imshow("color", color);
		cv::imshow("centerDepth", centerDepth);
		cv::imshow("gray", gray);
		cv::imshow("depth", depth);
		cv::imshow("transedImg.png", transedImg);
		cv::imshow("nir", grayNir);
		cv::imshow("checkList.png", checkList);

		cv::waitKey(1);

	}

}


//bagFile使って1フレームから座標変換
double cordiTransBagFile() {
	
	//////////////////////////////////////////   NIR    ////////////////////////////////
	cv::Mat nirCamMat, nirDistCoeffs;
	cv::FileStorage fs0("parameter\\instrinsics_Nir.xml", cv::FileStorage::READ);
	fs0["camera_matrix"] >> nirCamMat;
	fs0["distortion_coefficients"] >> nirDistCoeffs;
	std::cout << "nirCamMat = " << nirCamMat << std::endl;
	std::cout << "nirDistCoeffs = " << nirDistCoeffs << std::endl;
	

	std::cout << "************************** RS *************************" << std::endl;
	std::cout << "define filtre\n";//-------------------------fileter
	rs2::decimation_filter dec_filter;
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);
	rs2::disparity_transform depth_to_disparity(true);
	rs2::disparity_transform disparity_to_depth(false);
	rs2::spatial_filter spat_filter;
	spat_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.5);
	spat_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	spat_filter.set_option(RS2_OPTION_HOLES_FILL, 0);
	rs2::temporal_filter temp_filter;
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_ALPHA, 0.4);
	temp_filter.set_option(RS2_OPTION_FILTER_SMOOTH_DELTA, 20);
	temp_filter.set_option(RS2_OPTION_HOLES_FILL, 3);
	rs2::hole_filling_filter hf_filter;
	hf_filter.set_option(RS2_OPTION_HOLES_FILL, 1);

	cv::FileStorage fs1("parameter\\rsCalibResult0.yml", cv::FileStorage::READ);
	cv::Mat rs_cam_mat, rs_dist_coeffs;
	fs1["cam_matrix"] >> rs_cam_mat;
	fs1["distortion_coeffs"] >> rs_dist_coeffs;
	fs1.release();
	std::cout << "rs_cam_mat = " << rs_cam_mat << std::endl;
	std::cout << "rs_dist_coeffs = " << rs_dist_coeffs << std::endl;

	std::cout << "create remap" << std::endl;
	cv::Size rsImgSize{ 1280,720 };
	cv::Mat map3, map4, mapR2;
	cv::initUndistortRectifyMap(
		rs_cam_mat,//入力されるカメラ行列
		rs_dist_coeffs,//入力される歪み係数ベクトル
		mapR2,           //オプション．物体空間における平行化変換
		rs_cam_mat,//新しいカメラ行列
		rsImgSize,//歪み補整された画像サイズ
		CV_16SC2,// 1番目の出力マップの型． CV_32FC1 または CV_16SC2 のいずれか
		map3,
		map4
	);

	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	int i = 0;
	int number = 0;
	char key;	

	std::string targetFolder = "5";
	std::string date = "20200119";
	//std::string date = "20200120";
	std::string file_name = "data\\"+date+"\\"+targetFolder+"\\"+targetFolder+".bag";
	std::string output_names = "Outputs";
	rs2::pipeline pipe;
	rs2::config cfg;
	//cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 15);
	//cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 15);
	cfg.enable_device_from_file(file_name.c_str());
	auto profile = pipe.start(cfg);
	auto depth_stream = profile.get_stream(RS2_STREAM_COLOR).as<rs2::video_stream_profile>();
	auto depth_intr = depth_stream.get_intrinsics();

	std::cout << "************************** ex parameter *************************" << std::endl;
	cv::FileStorage fs2("parameter\\stereoCalibResult.yml", cv::FileStorage::READ);
	cv::Mat initR, T;
	fs2["R"] >> initR;
	fs2["T"] >> T;
	fs2.release();

	std::cout << "original_T = " << T << std::endl;
	T.at<double>(0, 0) = 0.0;
	//T.at<double>(0, 0) = -0.050;
	//T.at<double>(1, 0) = 0.195039;
	//T.at<double>(1, 0) = 0.195039 - 0.1;//小さくするとNIRは下に行く
	T.at<double>(1, 0) = 0.195039 - 0.05;//小さくするとNIRは下に行く
	//T.at<double>(2, 0) = -1.1 + 1.4;
	//T.at<double>(2, 0) = 1.5;
	T.at<double>(2, 0) = 1.7;
	//T.at<double>(2, 0) = 1.75;
	std::cout << "mod_T = " << T << std::endl;

	cv::Mat oR;
	cv::Rodrigues(initR, oR);
	std::cout << "oR = " << oR << std::endl;
	oR.at<double>(0, 0) = 0.0;
	oR.at<double>(1, 0) = 0.0;
	oR.at<double>(2, 0) = 0.0;
	//cv::Rodrigues(rodR, initR);
	std::cout << "oR = " << oR << std::endl;

	////////////////////////////////
	std::map<std::string, double>nir_intr;
	std::string names[] = { "fx","ppx","fy","ppy" };
	nir_intr[names[0]] = nirCamMat.at<double>(0, 0);
	nir_intr[names[1]] = nirCamMat.at<double>(0, 2);
	nir_intr[names[2]] = nirCamMat.at<double>(1, 1);
	nir_intr[names[3]] = nirCamMat.at<double>(1, 2);

	std::cout << "nir_intr[fx] = " << nir_intr["fx"] << std::endl;
	std::cout << "nir_intr[ppx] = " << nir_intr["ppx"] << std::endl;
	std::cout << "nir_intr[fy] = " << nir_intr["fy"] << std::endl;
	std::cout << "nir_intr[ppy] = " << nir_intr["ppy"] << std::endl << std::endl;

	//cv::Mat transedImg = cv::Mat::zeros(720, 1280, CV_8UC1);
	cv::Mat transedImg(720, 1280, CV_8UC1, cv::Scalar(125));
	cv::Mat A_brendingImg(720, 1280, CV_8UC1, cv::Scalar(125));

	
	int count = 1;
	int num = 0;
	bool getRsInf = 1;
	cv::Mat checkList = cv::Mat::zeros(720, 1280, CV_8UC1);


	rs2::frameset frames;
	rs2::points points;
	rs2::pointcloud pc;
	
	std::cout << "ready" << std::endl;
	while (true)
	{
		if (pipe.poll_for_frames(&frames))
		{
			int n = 0;
			auto frame = pipe.wait_for_frames();
			rs2::align align(RS2_STREAM_COLOR);
			auto aligned_frames = align.process(frames);

			rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();// rs2::depth_frame じゃないとget_distance できない
			rs2::frame depth_image = aligned_frames.get_depth_frame().apply_filter(color_map);// rs2::depth_frame じゃないとget_distance できない
			rs2::frame color_frame = aligned_frames.get_color_frame();

			// Query frame size (width and height)
			const int w = depth_frame.as<rs2::video_frame>().get_width();
			const int h = depth_frame.as<rs2::video_frame>().get_height();
			
			//fileter
			depth_frame = dec_filter.process(depth_frame);
			depth_frame = depth_to_disparity.process(depth_frame);
			depth_frame = spat_filter.process(depth_frame);
			depth_frame = temp_filter.process(depth_frame);
			depth_frame = disparity_to_depth.process(depth_frame);
			depth_frame = hf_filter.process(depth_frame);
			
			//cv::Mat color_mat = cv::Mat(cv::Size(color_image.get_width(), color_image.get_height()), CV_8UC3, const_cast<void *>(color_image.get_data()));
			//cv::Mat ir(cv::Size(depth_image.get_width(), depth_image.get_height()), CV_16SC1, (void *)depth_image.get_data(), cv::Mat::AUTO_STEP);
			cv::Mat color = cv::Mat(cv::Size(w, h), CV_8UC3, const_cast<void*>(color_frame.get_data()));
			cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_image.get_data());
			cv::remap(color, color, map3, map4, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());

			cv::Mat gray;//変換の時使う
			cvtColor(color, gray, CV_RGB2GRAY);
			
			if (GetAsyncKeyState(VK_UP))//include<windows.h>
			{
				double pan = 35;
				double tilt = 20;
				double addPan = -7;
				double addTilt = -7;
				
				std::ofstream pcNir("data\\"+date+"\\"+targetFolder+"\\transedNir"+targetFolder+".csv.");
				std::ofstream pcRGB("data\\"+date+"\\"+targetFolder+"\\rsRGB.csv.");

				int sw = 0;

				while (true) {
					std::cout << "n=" << n << std::endl;
					std::cout << "(pan, tilt)=(" << pan << ", " << tilt << ")" << std::endl;
					std::cout << "(addPan, addTilt)=(" << addPan << ", " << addTilt << ")" << std::endl;
					cv::Mat nir = cv::imread("data\\"+date+"\\"+targetFolder+"\\grayNir" + std::to_string(n) + ".png");
					if (nir.empty()) {
						std::cout << "fuck" << std::endl;
						break;
					}
					cv::Mat grayNir;
					cvtColor(nir, grayNir, CV_RGB2GRAY);

					// calc R
					cv::Mat Rd;
					cv::FileStorage fs3("C:\\Users\\yamak\\source\\repos\\jedi2\\shared_folder_ubuntu\\sharedExPara.yml", cv::FileStorage::READ);
					fs3["R"] >> Rd;
					Rd.at<double>(0, 1) = pan;
					Rd.at<double>(0, 0) = -tilt;

					Rd.at<double>(0, 0) = Rd.at<double>(0, 0)*(3.14159 / 180);//degree to radian
					Rd.at<double>(1, 0) = Rd.at<double>(1, 0)*(3.14159 / 180);
					Rd.at<double>(2, 0) = Rd.at<double>(2, 0)*(3.14159 / 180);

					cv::Mat vecR;
					vecR = Rd;
					cv::Mat R;
					cv::Rodrigues(vecR, R);//回転行列にする
					cv::Mat tR = R.t();
					cv::Mat tT = T.t();
					tR.push_back(tT);
					std::cout << "tR=" << tR << std::endl;
					cv::Mat RT;
					RT = tR.t();
					std::cout << "RT=" << RT << std::endl << std::endl;

					cv::Mat camMatRT = nirCamMat * RT;
					std::cout << "camMatRT=" << camMatRT << std::endl << std::endl;


					std::cout << "trans start\n";
					for (int N = 0; N < gray.total(); N++) {
						if (N == 1)std::cout << "N==1\n";
						int x = N % gray.cols;
						int y = N / gray.cols;

						float rsPixel[2] = { (float)x, (float)y };
						float depthInf = depth_frame.get_distance(rsPixel[0] / 3.0, rsPixel[1] / 3.0);//3*3サブピクセルでフィルタしてるから3で割る,cam1

						float rsPoint[3];
						rs2_deproject_pixel_to_point(rsPoint, &depth_intr, rsPixel, depthInf);//cam1からの三次元情報

						if (n==0) {
							pcRGB << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," <<
								int(color.at<cv::Vec3b>(y, x)[0]) << "," << int(color.at<cv::Vec3b>(y, x)[1]) << "," << int(color.at<cv::Vec3b>(y, x)[2]) << std::endl;
						}

						
						cv::Mat rsPointMat = cv::Mat::zeros(4, 1, CV_64F);//Matで計算できるように配列からcv::Mat
						rsPointMat.at<double>(0, 0) = rsPoint[0];
						rsPointMat.at<double>(1, 0) = rsPoint[1];
						rsPointMat.at<double>(2, 0) = rsPoint[2];
						rsPointMat.at<double>(3, 0) = 1;

						cv::Mat nirPixel = camMatRT * rsPointMat;

						nirPixel.at<double>(0, 0) = nirPixel.at<double>(0, 0) / nirPixel.at<double>(2, 0);
						nirPixel.at<double>(1, 0) = nirPixel.at<double>(1, 0) / nirPixel.at<double>(2, 0);
						nirPixel.at<double>(2, 0) = nirPixel.at<double>(2, 0) / nirPixel.at<double>(2, 0);


						if (0 <= nirPixel.at<double>(0, 0) && nirPixel.at<double>(0, 0) <= 128 && 0 <= nirPixel.at<double>(1, 0) && nirPixel.at<double>(1, 0) <= 128 && checkList.at<uchar>(y, x) == 0) {
							transedImg.at<uchar>(y, x) = grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0));
							//transedImgTest.at<uchar>(y, x) = grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0));

							pcNir << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," <<
								int(grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0))) << "," <<
								int(grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0))) << "," <<
								int(grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0))) << std::endl;

							checkList.at<uchar>(y, x) += 50;
						}

						//alpha brending
						else if (0 <= nirPixel.at<double>(0, 0) && nirPixel.at<double>(0, 0) <= 128 && 0 <= nirPixel.at<double>(1, 0) && nirPixel.at<double>(1, 0) <= 128 && checkList.at<uchar>(y, x) > 0) {
							float a = 0.5;
							double data = transedImg.at<uchar>(y, x)*a + grayNir.at<uchar>(nirPixel.at<double>(1, 0), nirPixel.at<double>(0, 0)) * (1 - a);

							transedImg.at<uchar>(y, x) = data;
							//transedImgTest.at<uchar>(y, x) = data;
							pcNir << rsPoint[0] << "," << rsPoint[1] << "," << rsPoint[2] << "," << data << "," << data << "," << data << std::endl;
							checkList.at<uchar>(y, x) += 50;
						}
						


					}
					std::cout << "trans end\n";
                    //次の画像の角度
					if (sw == 0) {
						pan += addPan;
						if (pan <= -35 || pan >= 35) {
							sw = 1;
						}
					}
					else if (sw==1 ) {
						tilt += addTilt; 
						addPan = addPan * (-1);
						sw = 0;
					}
					

					cv::imshow("nir"+std::to_string(n), nir);
					cv::imshow("transedImg.png", transedImg);
					cv::imshow("checkList.png", checkList);
					cv::imwrite("data\\"+date+targetFolder+"checkList.png", checkList);
					cv::imwrite("data\\" + date + targetFolder + "transedImg.png", transedImg);
					cv::waitKey(500);
					n++;
				}
			}
			
			
			cv::imshow("Depth", depth);
			cv::imshow("Color", color);
			key = cv::waitKey(1);



			if (key == 's')
			{
				output_names = std::to_string(number) + ".ply";
				//points.export_to_ply(output_names, color_image);
				number++;
				std::cout << "save ply_file" << std::endl;
			}

			if (key == 'q') break;
			//////////////////////////////////////// kokomade hatake /////////////////////



		}
	}

	pipe.stop();


	return 0;



}



/////////////////////////////////////////////////////////////////

#define MAP_HEIGHT 10
#define MAP_WIDTH 10

double m_Map[921600][6]; // csvデータ格納

double stoi(std::string str) { // stoi関数の定義
	double ret;
	std::stringstream ss;
	ss << str;
	ss >> ret;
	return ret;
}


//3次元情報のぶっ飛んだ値を消す
void iCsv() { // csvファイルのインポート
	//cv::Mat img = cv::imread("C:\\Users\\yamak\\source\\repos\\jedi2\\coordiTransRs2Nir\\transedImg.png");
	
	//cvtColor(img, img, CV_RGB2GRAY);

	std::string n = "5";
	std::ifstream ifs("pointCloud\\transedNir.csv");
	//std::ifstream ifs("C:\\Users\\yamak\\Documents\\laboratory\\experiment\\nir統合\\aligned\\removeNoise" + n + ".csv");
	std::string line;
	int i;
	i = 0;

	std::ofstream pc("pointCloud\\removedTransedNir.csv");
	//std::ofstream pc("C:\\Users\\yamak\\Documents\\laboratory\\experiment\\nir統合\\aligned\\removeNoise" + n + "-2.csv");
	std::cout << "start brending\n";
	while (getline(ifs, line)) {  //第１引数に入力ストリームを指定し、第２引数に受取り用の std::basic_string の変数を指定します。
		
		double num1, num2, num3,num4;
		replace(line.begin(), line.end(), ',', ' ');
		std::istringstream iss(line);
		iss >> num1 >> num2 >> num3>>num4;
		//std::cout << "num1=" << num1 << ", num2=" << num2 << ", num3=" << num3 << ", num4=" << num4 << std::endl;

		if (num1<-1.5) {
			pc << num1 << "," << num2 << "," << num3 <<","<< num4<< "," << num4 << "," << num4 << std::endl;
		}

		//pc << num1 << "," << num2 << "," << num3 <<","<< int(img.at<uchar>(y, x))<<"," << int(img.at<uchar>(y, x)) << "," << int(img.at<uchar>(y, x)) << std::endl;
		i++;		
	}
	std::cout << "finish import\n ";

}


void removeToFar() {
	iCsv();
	
	/*
	for (int i = 0; i < 100;i++) {
		std::cout << "m_Map[i]=" << m_Map[i][0]<<",  "<< m_Map[i][1] << ",  " << m_Map[i][2] << ",  " << m_Map[i][3] << std::endl;
	}

	cv::Mat Abrend = cv::imread("coordiTransRs2Nir\\transedImg.png");

	//std::cout<< "m_Map.total()=" << m_Map.size() <<std::endl;
	std::cout<< "Abrend.total()=" << Abrend.total() <<std::endl;

	std::ofstream AbrendPC("pointCloud\\Abrend.csv");
	for (int i = 0; i < Abrend.total();i++) {
		//AbrendPC << m_Map[i][0] << "," << m_Map[i][1] << "," << m_Map[i][2] << "," << Abrend.data[i];
		AbrendPC << m_Map[i][0] << "," << m_Map[i][1] << "," << m_Map[i][2] << std::endl;

	}
	*/
	std::cout << "finish\n";
}
