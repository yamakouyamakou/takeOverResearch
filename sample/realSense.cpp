#include"Stdafx.h"
#include"iostream"

#include<opencv2/core.hpp>
#include<opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include"cv-helpers.hpp"//*****

#include <string>
#include<windows.h>//GetAsyncKeyState(VK_UP)
#include <typeinfo.h>
#include <tuple>
#include<fstream>// image_points の保存


#include"librealsense2/rs.hpp"
#include<librealsense2/rsutil.h>

#include<Eigen/Core>
#include<Eigen/Dense>
#include <opencv2/core/eigen.hpp>

//#include"realSense.h"
//#include"MyForm.h"
//---------------------------------------------



void rsExposure3() {

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

	const auto window_name = "Display Image";
	namedWindow(window_name, cv::WINDOW_AUTOSIZE);

	std::cout << "ready...\n";
	while (cv::waitKey(1) < 0 && getWindowProperty(window_name, cv::WND_PROP_AUTOSIZE) >= 0)
	{
		std::cout << "GO !!!\n";

		rs2::frameset frames = pipe.wait_for_frames(); // Wait for next set of frames from the camera

		rs2::align align(RS2_STREAM_COLOR);
		auto aligned_frames = align.process(frames);

		rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();// rs2::depth_frame じゃないとget_distance できない
		rs2::frame depth_image = aligned_frames.get_depth_frame().apply_filter(color_map);// rs2::depth_frame じゃないとget_distance できない
		rs2::frame color_frame = aligned_frames.get_color_frame();

		// Query frame size (width and height)
		const int w = depth_frame.as<rs2::video_frame>().get_width();
		const int h = depth_frame.as<rs2::video_frame>().get_height();
		std::cout << "w = " << w << ", h = " << h << std::endl;

		//fileter
		depth_frame = dec_filter.process(depth_frame);
		depth_frame = depth_to_disparity.process(depth_frame);
		depth_frame = spat_filter.process(depth_frame);
		depth_frame = temp_filter.process(depth_frame);
		depth_frame = disparity_to_depth.process(depth_frame);
		depth_frame = hf_filter.process(depth_frame);
		
		// Create OpenCV matrix of size (w,h) from the colorized depth data
		//cv::Mat color(cv::Size(w, h), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
		cv::Mat color = cv::Mat(cv::Size(w, h), CV_8UC3, const_cast<void*>(color_frame.get_data()));
		//cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
		//cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_frame.get_data());
		//cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_frame.get_data());
		cv::Mat depth(cv::Size(w, h), CV_8UC3, (void*)depth_image.get_data());

		cv::Mat gray;
		cvtColor(color, gray, CV_RGB2GRAY);
		
		// Update the window with new data
		cv::imshow("color", color);
		cv::imshow("depth", depth);

		
		

	}






}


void rsGetPointCloud() {
	std::cout << "start\n";
	rs2::colorizer color_map;//深度情報の着色宣言

	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	int dev_id = 0;
	rs2::device dev = devices[dev_id];

	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
	cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);
	cfg.enable_device(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));


	rs2::pipeline pipe;//The SDK API entry point is the pipeline class
	//pipe.start(cfg);
	
	auto profile = pipe.start(cfg);
	auto depth_stream = profile.get_stream(RS2_STREAM_COLOR).as< rs2::video_stream_profile >();
	auto depth_intr = depth_stream.get_intrinsics();// 画角情報取得
	rs2::align align_to(RS2_STREAM_COLOR);

	std::cout << "config filter\n";
	rs2::decimation_filter dec_filter;// 所在は /usr/local/include/librealsense2/rs_processing.hpp
	dec_filter.set_option(RS2_OPTION_FILTER_MAGNITUDE, 3);//3x3のサブピクセル化を指定
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

	char key;

	std::cout << "please push to get points clouds\n";

	while (cv::waitKey(1) == -1) {
		
		rs2::frameset frames = pipe.wait_for_frames();
		rs2::align align(RS2_STREAM_COLOR);
		auto aligned_frames = align.process(frames);
		rs2::video_frame color_frame = aligned_frames.first(RS2_STREAM_COLOR);
		rs2::depth_frame depth_frame = aligned_frames.get_depth_frame();


		/*
		auto frames = pipe.wait_for_frames();
		frames = align_to.process(frames);
		auto depth_frame = frames.get_depth_frame();
		auto color_frame = frames.get_color_frame();
		*/
		/*
		depth_frame = dec_filter.process(depth_frame);
		depth_frame = depth_to_disparity.process(depth_frame);
		depth_frame = spat_filter.process(depth_frame);
		depth_frame = temp_filter.process(depth_frame);
		depth_frame = disparity_to_depth.process(depth_frame);
		depth_frame = hf_filter.process(depth_frame);
		*/
		cv::Mat color(cv::Size(1280, 720), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
		cv::Mat depth(cv::Size(1280, 720), CV_16SC1, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
		//cv::Mat color = cv::Mat(cv::Size(color_frame.get_width(), color_frame.get_height()), CV_8UC3, const_cast<void*>(color_frame.get_data()));
		//cv::Mat depth(cv::Size(depth_frame.get_width(), depth_frame.get_height()), CV_16SC1, (void *)depth_frame.get_data(), cv::Mat::AUTO_STEP);

		depth.convertTo(depth, CV_8U, -255.0 / 10000.0, 255.0);// 行列をスケーリングして別のデータ型に変換します
		cv::equalizeHist(depth, depth);//グレースケール画像のヒストグラムを均一化します．
		cv::applyColorMap(depth, depth, cv::COLORMAP_JET);

		cv::Mat drawColor= color.clone();
		cv::circle(drawColor, cv::Point(color.cols / 2, color.rows / 2), 25, cv::Scalar(0, 200, 0), 3, 4);
		
		cv::Mat disp(color.rows + depth.rows, (std::max)(color.cols, depth.cols), CV_8UC3);//colorとdepthを並べたやつ
		color.copyTo(cv::Mat(disp, cv::Rect(0, 0, color.cols, color.rows)));
		depth.copyTo(cv::Mat(disp, cv::Rect(0, color.rows, depth.cols, depth.rows)));
		cv::resize(disp, disp, cv::Size(), 0.65, 0.65);

		cv::Mat rsGray;
		cvtColor(color, rsGray, CV_RGB2GRAY);

		cv::imshow("color", color);
		cv::imshow("drawColor", drawColor);
		cv::imshow("rsGray",rsGray);
		cv::imshow("depth", depth);
		cv::imshow("disp", disp);


		key = cv::waitKey(0);
		if (GetAsyncKeyState(VK_UP))//include<windows.h>
		{
			std::cout << "please wait\n";
			std::ofstream ofs("pointCloud\\pcColorRs.csv");
			std::ofstream ofs3("pointCloud\\pcGrayRs.csv");
			for (int y = 0; color.rows > y; y++) {
				for (int x = 0; color.cols > x; x++) {
					float pixel[2] = { float(x),float(y) };
					//float depthInf = depth_frame.get_distance(pixel[0] / 3.0, pixel[1] / 3.0);//3*3サブピクセルでフィルタしてるから3で割る
					float depthInf = depth_frame.get_distance(pixel[0] , pixel[1] );
					float point[3];
					rs2_deproject_pixel_to_point(point, &depth_intr, pixel, depthInf);
					
					ofs << point[0] << " " << point[1] << " " << point[2] << " " <<
						int(color.at<cv::Vec3b>(y, x)[2]) << " " << int(color.at<cv::Vec3b>(y, x)[1]) <<" "<< int(color.at<cv::Vec3b>(y, x)[0]) << std::endl;
				
					ofs3 << point[0] << " " << point[1] << " " << point[2] << " " << int(rsGray.at<uchar>(y, x) ) << std::endl;
				}
			}
			std::cout << "saved 3dpc.csv\n";
			std::cout << "saved 3dpcGyay.csv\n";

			


		}
		





	}


}


std::tuple<rs2::pipeline, struct rs2_intrinsics>supplyRsPipe(int n) {
//void GreatFunction(int n){
	// pipe, align_to, フィルタ各種, depth_intr, 
	rs2::pipeline pipe;
	rs2::pipeline_profile profile;

	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	int devId = n;
	rs2::device dev = devices[devId];
	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30);
	cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30);
	cfg.enable_device( dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER) );
	profile = pipe.start(cfg);

	//rs2::frameset frames;
	auto depth_st = profile.get_stream( RS2_STREAM_COLOR ).as<rs2::video_stream_profile>();
	auto depth_intr = depth_st.get_intrinsics();
	//rs2::align align_to(RS2_STREAM_COLOR);

	cv::Mat A = cv::Mat( cv::Size(3,3), CV_8UC1 );
	float xf = depth_intr.fx;
	std::cout << "coeffs = " << depth_intr.coeffs << std::endl;
	std::cout << "fx = " << depth_intr.fx << std::endl;
	std::cout << "fy = " << depth_intr.fy << std::endl;
	std::cout << "height = " << depth_intr.height << std::endl;
	std::cout << "model = " << depth_intr.model << std::endl;
	std::cout << "ppx = " << depth_intr.ppx << std::endl;
	std::cout << "ppy = " << depth_intr.ppy << std::endl;
	std::cout << "width = " << depth_intr.width << std::endl;
	A.at<double>(0,0) = depth_intr.fy;

	//std::cout << "A = " << A << std::endl;
	//std::cout << "typeid(profile).name() = " << typeid(profile).name() << std::endl;
	//std::cout << "typeid(depth_intr).name() = " << typeid(depth_intr).name() << std::endl;
	return std::forward_as_tuple( pipe, depth_intr );
}

rs2_stream find_stream_to_align(const std::vector<rs2::stream_profile>& streams)
{
	//Given a vector of streams, we try to find a depth stream and another stream to align depth with.
	//We prioritize color streams to make the view look better.
	//If color is not available, we take another stream that (other than depth)
	rs2_stream align_to = RS2_STREAM_ANY;
	bool depth_stream_found = false;
	bool color_stream_found = false;
	for (rs2::stream_profile sp : streams)
	{
		rs2_stream profile_stream = sp.stream_type();
		if (profile_stream != RS2_STREAM_DEPTH)
		{
			if (!color_stream_found)         //Prefer color
				align_to = profile_stream;

			if (profile_stream == RS2_STREAM_COLOR)
			{
				color_stream_found = true;
			}
		}
		else
		{
			depth_stream_found = true;
		}
	}

	if (!depth_stream_found)
		throw std::runtime_error("No Depth stream available");

	if (align_to == RS2_STREAM_ANY)
		throw std::runtime_error("No stream found to align with Depth");

	return align_to;
}


void rsCalib(int n) {
	std::cout << "start function\n";
	cv::Size rs_image_size{ 1280, 720 };
	std::cout << "image_size_width=" << rs_image_size.width << std::endl;
	std::cout << "image_size_height=" << rs_image_size.height << std::endl;

	//格納場所を確保
	std::vector < std::vector<cv::Point2f> > rs_image_points;
	std::vector< std::vector<cv::Point3f> >rs_object_points;


	double last_captured_timestamp;/////////////// calib board config ////////////
	int n_boards = 20;
	int CLOCKS_PER_SEC;
	float image_sf = 0.5f;// コーナーの座標をスケーリングするためのやつ
	//int board_w = 6;
	//int board_h = 9;
	int board_w = 9;
	int board_h = 6;
	int board_n = board_w * board_h;

	//////////////////////////////////////////////////////////

	// Realsense用のパイプライン作成
	rs2::pipeline pipe;
	rs2::pipeline_profile profile;


	rs2::context ctx;
	rs2::device_list devices = ctx.query_devices();
	int dev_id = n;
	rs2::device dev = devices[dev_id];
	try {
		// 設定ファイル作成&適応
		rs2::config cfg;
		cfg.enable_stream(RS2_STREAM_COLOR, 1280, 720, RS2_FORMAT_BGR8, 30); // RGB
		cfg.enable_stream(RS2_STREAM_DEPTH, 1280, 720, RS2_FORMAT_Z16, 30); // Depth
		cfg.enable_device(dev.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
		profile = pipe.start(cfg);
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		system("PAUSE");
		exit(-1);
	}

	// 適応した設定を再読込、フレーム間の座標関係等を取得
	auto sensor = profile.get_device().first<rs2::depth_sensor>();
	auto depth_scale = sensor.get_depth_scale();
	rs2_stream align_to = find_stream_to_align(profile.get_streams());
	rs2::align align(align_to);

	// RSウォームアップ - 自動露光が安定するまで最初の数フレームは無視
	rs2::frameset frames;
	for (int i = 0; i < 30; i++)
	{
		//Wait for all configured streams to produce a frame
		frames = pipe.wait_for_frames();
	}
	// Declare depth colorizer for pretty visualization of depth data
	rs2::colorizer color_map;

	//------------------------------------------------------------------------------------

	int number = 0;
	int count = 1;
	float move = 0.0f;
	//for (;;) {
	for (; ; count++) {
		cv::Size board_sz = cv::Size(board_w, board_h);

		//-------------------------------------------------------------------
		// フレーム取得
		rs2::frameset frameset = pipe.wait_for_frames(); // Wait for next set of frames from the camera
		//rs2::frame depth_frame = color_map(frameset.get_depth_frame()); // Find and colorize the depth data
		//rs2::frame color_frame = frameset.get_color_frame();            // Find the color data

		// カラー画像と深度画像を対応付ける
		auto processed = align.process(frameset);
		rs2::video_frame color_frame = processed.first(align_to);
		rs2::depth_frame depth_frame = processed.get_depth_frame();

		// フレームからMatを生成
		cv::Mat color(cv::Size(1280, 720), CV_8UC3, (void*)color_frame.get_data(), cv::Mat::AUTO_STEP);
		cv::imshow("rsColor", color);
		
		auto depth_mat = depth_frame_to_meters(pipe, depth_frame);
		cv::Mat depth(cv::Size(1280, 720), CV_8UC3, (void*)depth_frame.get_data(), cv::Mat::AUTO_STEP);
		cv::imshow("depth_mat", depth_mat);
		
		cv::Mat rsGray;
		cvtColor(color, rsGray, CV_RGB2GRAY);
		std::cout << "rsGray.size()=" << rsGray.size() << std::endl;

		//ボードを探す
		std::vector<cv::Point2f>rs_corners;
		bool found = cv::findChessboardCorners(rsGray, board_sz, rs_corners);
		std::cout << "rs_corners=" << rs_corners << std::endl;

		double timestamp = (double)clock() / CLOCKS_PER_SEC;
		
		//--------------------------------------------------------------

		cv::Mat nir_image;
		//if (nir_found && found && timestamp - last_captured_timestamp >= 1) {
		if (found >= 1) {
			cornerSubPix(rsGray, rs_corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));

			cv::Mat mcorners(rs_corners);
			//mcorners *= (1. / image_sf);//コーナーの座標をスケーリングする
			rs_image_points.push_back(rs_corners);
			rs_object_points.push_back(std::vector<cv::Point3f>());
			std::vector<cv::Point3f>& opts = rs_object_points.back();
			opts.resize(board_n);

			int optsN = 0;
			for (int y = 0; y < 6; y++) {
				for (int x = 0; x < 9; x++) {
					//std::cout << "(x, y) = " << x << ", " << y << std::endl;
					opts[optsN] = cv::Point3f((float)x, (float)y, 0.f);
					optsN++;
				}
			}

			std::cout << "opts=" << opts << std::endl;

			cv::imwrite("rsCalibImg\\" + std::to_string(number) + "rsGray.jpg", rsGray);

			std::cout << "collected our" << (int)rs_image_points.size() << "of" << n_boards << "needed chessboard images\n" << std::endl;
			cv::waitKey(300);
			number++;
		}
		drawChessboardCorners(rsGray, board_sz, rs_corners, found);
		cv::imshow("drawRsgray", rsGray);

		cv::waitKey(1);
		if (number >= 20)break;
	}

	//--------------------------------------------------------画像の取り組みループおわり
	cv::destroyWindow("calibration");
	std::cout << "\n\n***calibrationg the camera...\n" << std::endl;


	//格子点の位置をメートルに変換
	float Size = 0.0271;
	for (int i = 0; i < rs_object_points.size(); i++) {
		for (int i2 = 0; i2 < board_n; i2++) {
			rs_object_points[i][i2].x = rs_object_points[i][i2].x*Size;
			rs_object_points[i][i2].y = rs_object_points[i][i2].y*Size;
		}
	}
	std::cout << "rs_object_points =" << rs_object_points[0] << std::endl;


	//----------------------------------------------------------
	//カメラキャリブレーション
	cv::Mat cam_matrix, distortion_coeffs;
	std::vector<cv::Mat>rsRvecs, rsTvecs;
	double err = cv::calibrateCamera(
		rs_object_points,
		rs_image_points,
		rs_image_size,
		cam_matrix,//カメラ行列
		distortion_coeffs,//歪み係数
		rsRvecs,
		rsTvecs,
		cv::CALIB_ZERO_TANGENT_DIST | cv::CALIB_FIX_PRINCIPAL_POINT
	);
	std::cout << "cam_matrix=" << cam_matrix << std::endl;

	//内部パラメータと歪み係数を保存
	std::string fileName = "parameter\\rsCalibResult" + std::to_string(n) + ".yml";
	cv::FileStorage fs(fileName, cv::FileStorage::WRITE);
	cv::write(fs, "rs_image_width", rs_image_size.width);
	cv::write(fs, "rs_image_height", rs_image_size.height);
	cv::write(fs, "cam_matrix", cam_matrix);
	cv::write(fs, "distortion_coeffs", distortion_coeffs);
	cv::write(fs, "rsRvecs", rsRvecs);
	cv::write(fs, "rsTvecs", rsTvecs);
	fs << "rs_image_points" << rs_image_points;
	fs << "rs_object_points" << rs_object_points;
	fs.release();
	std::cout << "save rsCalibResultN.yml!" << std::endl;

	std::cout << "compleate!" << std::endl;

	std::cout << " rsベクトルの大きさ " << std::endl;
	for (int i = 0; rsTvecs.size() > i; i++) {
		Eigen::Vector3d Ers;
		cv::cv2eigen(rsTvecs[i], Ers);
		Eigen::Vector3d absrs = Ers.cwiseAbs2();//係数毎の計算：絶対値の二乗
		float addrs = absrs[0] + absrs[1] + absrs[2];
		float Sqrtrs = sqrt(addrs);
		std::cout << "Sqrtrs=" << Sqrtrs << std::endl;
	}



}




//-------------------------------------------------------------
void nir_project_point_to_pixel(float pixel[2], cv::Mat nir_cam_mat, cv::Mat nir_dist_coeffs, const float point[3])
{
	//assert(intrin->model != RS2_DISTORTION_INVERSE_BROWN_CONRADY); // Cannot project to an inverse-distorted image

	float x = point[0] / point[2], y = point[1] / point[2];
	/*
	if (intrin->model == RS2_DISTORTION_MODIFIED_BROWN_CONRADY)
	{

		float r2 = x * x + y * y;
		float f = 1 + intrin->coeffs[0] * r2 + intrin->coeffs[1] * r2*r2 + intrin->coeffs[4] * r2*r2*r2;
		x *= f;
		y *= f;
		float dx = x + 2 * intrin->coeffs[2] * x*y + intrin->coeffs[3] * (r2 + 2 * x*x);
		float dy = y + 2 * intrin->coeffs[3] * x*y + intrin->coeffs[2] * (r2 + 2 * y*y);
		x = dx;
		y = dy;
	}
	if (intrin->model == RS2_DISTORTION_FTHETA)
	{
		float r = sqrtf(x*x + y * y);
		float rd = (float)(1.0f / intrin->coeffs[0] * atan(2 * r* tan(intrin->coeffs[0] / 2.0f)));
		x *= rd / r;
		y *= rd / r;
	}
	*/
	pixel[0] = x * nir_cam_mat.at<double>(0,0) + nir_cam_mat.at<double>(0,2);
	pixel[1] = y * nir_cam_mat.at<double>(1, 1) + nir_cam_mat.at<double>(2, 1);
}

static void nir_deproject_pixel_to_point(float point[3], const struct rs2_intrinsics * intrin, const float pixel[2], float depth)
{
	assert(intrin->model != RS2_DISTORTION_MODIFIED_BROWN_CONRADY); // Cannot deproject from a forward-distorted image
	assert(intrin->model != RS2_DISTORTION_FTHETA); // Cannot deproject to an ftheta image
	//assert(intrin->model != RS2_DISTORTION_BROWN_CONRADY); // Cannot deproject to an brown conrady model
	
	float x = (pixel[0] - intrin->ppx) / intrin->fx;
	float y = (pixel[1] - intrin->ppy) / intrin->fy;
	if (intrin->model == RS2_DISTORTION_INVERSE_BROWN_CONRADY)
	{
		std::cout << "intrin.model = " << intrin->model << std::endl;
		float r2 = x * x + y * y;
		float f = 1 + intrin->coeffs[0] * r2 + intrin->coeffs[1] * r2*r2 + intrin->coeffs[4] * r2*r2*r2;
		float ux = x * f + 2 * intrin->coeffs[2] * x*y + intrin->coeffs[3] * (r2 + 2 * x*x);
		float uy = y * f + 2 * intrin->coeffs[3] * x*y + intrin->coeffs[2] * (r2 + 2 * y*y);
		x = ux;
		y = uy;
	}
	point[0] = depth * x;
	point[1] = depth * y;
	point[2] = depth;
}

