#include "eyesdetect.h"
#include<Qdebug.h>

eyesdetect::eyesdetect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}

void eyesdetect::control_slect()
{
	control_number = ui.comboBox_2->currentIndex();
	eyesInit = false;
}

void eyesdetect::slect_clink()
{
	slect_num = ui.comboBox->currentIndex();
	if (slect_num == 0)
	{
		eyes_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	}
	else 
	{
		eyes_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye.xml";
	}
	eyes_cascade.load(eyes_cascade_name);
}

void eyesdetect::set_mousespeed()
{
	x_speed = ui.mouse_x->value();
	y_speed = ui.mouse_y->value();
}

void eyesdetect::open_clink()         //打开摄像机按钮响应
{
	if (Lpause&&eye_detect&&!Lstop)
	{
		eye_detect = True;
	}
	else
	{
		eye_detect = False;
	}
	Lstop = False;
	Lpause = False;
	cap.open(0);
	if (!cap.isOpened())             // 检测是否成功打开摄像机
	{
		Lstop = True;
		QMessageBox msg;             //加载错误后弹出窗口
		msg.setWindowTitle("error");
		msg.setText("error! capture can not open!");
		msg.exec();
		return;
	}
//	int i = 0;
//	QThread t;
//  t.start();
	open_cemera();
//	t.quit();
//	t.wait();
//  subThread.wait();
//	QtConcurrent::run(this, &Worker::fun1);
}

void eyesdetect::open_cemera()
{
	while (!Lstop)
	{
		t = (double)cv::getTickCount();   //计时，用于计算fps
		cap >> frame;                     // 获取一帧图片
		all_picture++;
		if (!frame.empty())
		{
			if (eye_detect)
			{
			   detectAndDisplay(frame);
			}
			qImage = Mat2QImage(frame);
			QCoreApplication::processEvents();   //用于检测是否有按键按下，避免ui假死
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			fps = 1.0 / t;                       //计算fps
			sprintf_s(string_fps, "%.2f", fps);  // 帧率保留两位小数
			std::string fpsString("FPS:");
			fpsString += string_fps;            // 在"FPS:"后加入帧率数值字符串
											    // 将帧率信息写在输出帧上
			putText(frame,                      // 图像矩阵
				fpsString,                      // string型文字内容
				cv::Point(5, 20),               // 文字坐标，以左下角为原点
				cv::FONT_HERSHEY_SIMPLEX,       // 字体类型
				0.5,                            // 字体大小
				cv::Scalar(0, 0, 0));           // 字体颜色
			//left_eye.clear();
			//right_eye.clear();
			eyes_detect();
		}
	}
	cap.release();
}

void eyesdetect::start_clink()                     //start响应
{
	open_eye = True;
	eye_detect = True;
	all_right=0;
	all_face = 0;
	all_picture = 0;
	trackObject = -1;
	start_lefteye.clear();  //清楚find_puild记录的坐标
	start_righteye.clear();
	campshift_flag = false;
	if (!face_cascade.load(face_cascade_name))
	{  
		QMessageBox msg;                           //分类器加载错误后弹出窗口
		msg.setWindowTitle("error");
		msg.setText("error! face_cascade.load error!");
		msg.exec();
		return;
	};
	if (!eyes_cascade.load(eyes_cascade_name)) 
	{ 
		QMessageBox msg;                            //加载错误后弹出窗口
		msg.setWindowTitle("error");
		msg.setText("error! eyes_cascade.load error!");
		msg.exec();
		return;
	};
	SetCursorPos(960, 540);
	
	if(!frame.empty() && Lpause)
	{
		eye_detect = True;
		Lstop = False;
	//	trackObject = 0;
		all_time = (double)cv::getTickCount();      //暂停后再按开始检测重新计时
		open_clink();                               //检测暂停后再按开始检测重新开始检测
	}
	if (!eyesInit&&control_number == 0 )
	{
		int i = 0;
		//if (cap.isOpened())
		//{
		//	vector<Point> message_palce;
		//	message_palce.push_back(Point(910, 520));
		//	message_palce.push_back(Point(0, 40));
		//	message_palce.push_back(Point(1800, 40));
		//	message_palce.push_back(Point(1800, 900));
		//	message_palce.push_back(Point(0, 900));
		//	trackObject = -1;
		//	QMessageBox msg;                            //加载错误后弹出窗口
		//	for (int i = 0; i < 3; i++)
		//	{
		//		msg.setWindowTitle((u8"初始化"));
		//		msg.setText((u8"请注视1秒!"));
		//		msg.setGeometry(message_palce[i].x, message_palce[i].y, 100, 80);
		//		msg.exec();
		//		for (int j = 0; j < 10; j++)
		//		{
		//			cap >> frame;
		//			detectAndDisplay(frame);
		//			qImage = Mat2QImage(frame);
		//			QCoreApplication::processEvents();   //用于检测是否有按键按下，避免ui假死
		//			eyes_detect();
		//		}
		//		cout << "next" <<endl;
		//	}
		//}
	}
	eyesInit = true;
	int trackObject = -1;      //判断第一次检测
	all_time = (double)cv::getTickCount();
}

void eyesdetect::pause_clink()                        //暂停按钮
{
	//for (int i = 0; i<Leye_line.size() - 1; i++)             //绘制轨迹
	//{
	//	line(frame, 10*(Leye_line[i]- Leye_line[0])+ Leye_line[0], 10*(Leye_line[i+1] - Leye_line[0])+ Leye_line[0], Scalar(0, 255, 0), 2.5);
	//	line(frame, 10*(Reye_line[i]- Reye_line[0])+ Reye_line[0], 10*(Reye_line[i+1] - Reye_line[0])+ Reye_line[0], Scalar(0, 0, 255), 2.5);
	//}
	//namedWindow("轨迹图");
	//imshow("轨迹图", frame);
	Leye_line.clear();
	Reye_line.clear();
	last_eyes.clear();
	find_lefteye.clear();
	find_righteye.clear();
	start_lefteye.clear();
	start_righteye.clear();
	Harris_lefteye.clear();
	Harris_righteye.clear();
	all_time= ((double)cv::getTickCount() - all_time) / cv::getTickFrequency();
	Lstop = True;
	Lpause = True;
	eyesInit = false;  //眼睛注视点初始化
	trackObject = -1;
	campshift_flag = false;
	last_right = (int)(all_right / all_picture*100); //粗略计算识别正确率，检测到两个眼睛就认为检测正确
	if (cap.isOpened()&& eye_detect)
	{
		cap.release();
		QMessageBox msg;                                     //暂停按钮按下后，通过弹窗显示统计结果
		msg.setWindowTitle((u8"瞳孔检测结果统计"));
		detect_picture = QString((u8"总共检测视频："));      //str(u8"显示中文")
		detect_picture.append(QString::number(all_picture));
		detect_picture.append((u8"  帧"));
		detect_picture.append("\n");
		detect_picture.append(QString((u8"总共检测双眼：")));
		detect_picture.append(QString::number(all_right));
		detect_picture.append((u8"  双"));
		detect_picture.append("\n");
		detect_picture.append(QString((u8"检测准确率为：")));
		detect_picture.append(QString::number(last_right));
		detect_picture.append("%");
		detect_picture.append("\n");
		detect_picture.append(QString((u8"检测总共耗时：")));
		detect_picture.append(QString::number(all_time));
		detect_picture.append((u8"  秒"));
		msg.setText(detect_picture);
		msg.exec();
		return;
	}
	eye_detect = False;
}

void eyesdetect::quit_clink()
{ 
	if(cap.isOpened())                 //退出前关闭摄像机
	{
		cap.release();
	}
	exit(0);                           //退出程序
}
 
void eyesdetect::open_videofile()
{
	all_picture = 0;
	while (!Lstop)
	{
		t = (double)cv::getTickCount();   //计时，用于计算fps
		cap >> frame;                     // 获取一帧图片
		all_picture++;
		if (!frame.empty())
		{
			if (eye_detect)
			{
				detectAndDisplay(frame);
			}
			else 
			{
				Sleep(20);    //程序延时
			}
			qImage = Mat2QImage(frame);
			QCoreApplication::processEvents();   //用于检测是否有按键按下，避免ui假死
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			fps = 1.0 / t;                       //计算fps
			sprintf_s(string_fps, "%.2f", fps);  // 帧率保留两位小数
			std::string fpsString("FPS:");
			fpsString += string_fps;            // 在"FPS:"后加入帧率数值字符串
												// 将帧率信息写在输出帧上
			putText(frame,                      // 图像矩阵
				fpsString,                      // string型文字内容
				cv::Point(5, 20),               // 文字坐标，以左下角为原点
				cv::FONT_HERSHEY_SIMPLEX,       // 字体类型
				0.5,                            // 字体大小
				cv::Scalar(0, 0, 0));           // 字体颜色  
		}
		else {
			all_time = ((double)cv::getTickCount() - all_time) / cv::getTickFrequency();
			Lstop = True;
			Lpause = True;
			last_right = (int)(all_right / all_picture * 100); //粗略计算识别正确率，检测到两个眼睛就认为检测正确
			QMessageBox msg;                                     //暂停按钮按下后，通过弹窗显示统计结果
			msg.setWindowTitle((u8"视频检测结果统计"));
			detect_picture = QString((u8"总共检测视频："));      //str(u8"显示中文")
			detect_picture.append(QString::number(all_picture));
			detect_picture.append((u8"  帧"));
			detect_picture.append("\n");
			detect_picture.append(QString((u8"总共检测双眼：")));
			detect_picture.append(QString::number(all_right));
			detect_picture.append((u8"  双"));
			detect_picture.append("\n");
			detect_picture.append(QString((u8"检测准确率为：")));
			detect_picture.append(QString::number(last_right));
			detect_picture.append("%");
			detect_picture.append("\n");
			detect_picture.append(QString((u8"检测总共耗时：")));
			detect_picture.append(QString::number(all_time));
			detect_picture.append((u8"  秒"));
			msg.setText(detect_picture);
			msg.exec();
			return;
		}
	}
	cap.release();
}

QImage  eyesdetect::Mat2QImage(cv::Mat cvImg)                   //Mat转换为QImage
{
	QImage qImg;
	if (cvImg.channels() == 3)                              
	{
		cv::cvtColor(cvImg, cvImg, CV_BGR2RGB);
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_RGB888);
	}
	else if (cvImg.channels() == 1)                    
	{
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_Indexed8);
	}
	else
	{
		qImg = QImage((const unsigned char*)(cvImg.data),
			cvImg.cols, cvImg.rows,
			cvImg.cols*cvImg.channels(),
			QImage::Format_RGB888);
	}
	return qImg;
}

void  eyesdetect::detectAndDisplay(Mat frame)
{
	//cout << "detect" << endl;
	eyeclose_flag++;
	if (flag_n ==0)
	{  //每次读取新的帧才进入此部分
		cvtColor(frame, frame_gray, CV_BGR2GRAY);//转化为灰度图
		if (!campshift_flag)
		{                   //追踪失败时进入此函数，重新进行人脸检测
			faces.clear();
			face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(180, 180));
			if (faces.size() != 1)
			{
				face_slect();  //多个人脸时选出面积最大的一个
			}
			if (trackObject < 0)                                   
			{                    //只有第一次才进入，只生成一次模板
				face_model();   //生成追踪模型
				campshift_flag = true;
				camshift_face(frame);
			}
		}
	}
	else if(flag_n==1)
	{
		//cout << "flag==1" <<endl;  //递归调用进入此部分
		faces.clear();
		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(180, 180));
		if (faces.size() ==0)
		{
			return;
		}
		else if (faces.size() >1)
		{
			face_slect();
		}
		faces_refer.clear();                                
		faces_refer.push_back(faces[0]);                  
		eyeROI_width = faces_refer[0].width*width_weight;
		eyeROI_height = faces_refer[0].height*height_weight;
		x_bias = faces[0].x + faces[0].width*startx_weight - start_x;
		y_bias = faces[0].y + faces[0].height*starty_weight - strat_y;
	}
	if (trackObject&&flag_n==0)
	{
		camshift_face(frame);
	}
	if (pt.size()>1)
	{
		int i = pt.size() - 1;
		start_x = start_referx + pt[i].x - pt[1].x + x_bias;
		strat_y = strat_refery + pt[i].y - pt[1].y + y_bias;
		//trans_point(pt[i].x - pt[i - 1].x, pt[i].y - pt[i - 1].y);
	}
	else { 
		start_x = start_referx + x_bias;
		strat_y = strat_refery + y_bias;
	}
	Rect rect_det(start_x, strat_y, eyeROI_width, eyeROI_height);
	eyeROI = frame_gray(rect_det);
	/*namedWindow("eyes");
	imshow("eyes", eyeROI);*/
	eyes.clear();
	center_comp.clear();
	eyes_cascade.detectMultiScale(eyeROI, eyes, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(20, 20)); //人眼检测
	if (eyes.size() == 2)
	{
		for (size_t j = 0; j < eyes.size(); j++)
		{                                                                                        //绘制人眼区域
			Point center(start_x + eyes[j].x + eyes[j].width*0.5, strat_y + eyes[j].y + eyes[j].height*0.5);
			int radius = cvRound((eyes[j].width + eyes[j].height)*0.1) + 12;
			circle(frame, center, radius / 2, Scalar(255, 0, 0), 4, 8, 0);
			center_comp.push_back(center);
		}
		flag_n = 0;             //控制递归调用次数，避免一直检测不到双眼是程序陷入死循环
		all_right++;           //正确检测到人眼的次数，粗略统计，检测到两个就认为正确
		int i = pt.size() - 1;
		if (control_number == 1)
		{
			trans_point(pt[i].x - pt[i - 1].x, pt[i].y - pt[i - 1].y);
		}
	}
	else if (flag_n <= 0) {
		//trackObject = 0;
		//pt.erase(pt.end());
		campshift_flag = false;
		//cout << "digui" << endl;
		flag_n++;                    //一次递归后依然检测不到，此帧检测失败，跳出函数，进行下一帧
		detectAndDisplay(frame);     //人眼检测少于两个时，递归调用，重新采用AdaBoost检测，算出追踪和实际差距，进行矫正追踪窗口
		flag_n = 0;
	}
	else 
	{
		if (eyeclose_flag >= 2)
		{
			mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			Sleep(5);        //要留给某些应用的反应时间   
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			//Sleep(20);
			//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			//Sleep(5);        //要留给某些应用的反应时间   
			//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		}
		//cout << "wrong 2" << endl;
		eyeclose_flag = 0;
		//pt.erase(pt.end());
		//trackObject = 0;
		campshift_flag = false;
		flag_n = 0;
	}
}

void eyesdetect::find_pupil(Mat frame, int j)//寻找瞳孔函数
{
	Mat img2, img3, img4;
	//cvtColor(img, cimg, COLOR_GRAY2BGR);
	//cvtColor(frame, img2, COLOR_BGR2GRAY);   //把彩色图转换为黑白图像
	GaussianBlur(frame, img2, Size(5, 5), 2, 2);
	/*bilateralFilter(img2, img2, 25, 25 * 2, 25 / 2);
	blur(img2, img2, cv::Size(6,6), cv::Point(-1, -1));*/
	//equalizeHist(img2, img2);

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(9, 11));

	//Mat element = getStructuringElement(MORPH_ELLIPSE, Size(13, 19));
	morphologyEx(img2, img2, MORPH_CLOSE, element);
	//equalizeHist(img2, img2);
	/*int blockSize = 11;
	int maxVal = 255;
	Mat thresholded;
	threshold(eyeROI, thresholded, 150, 255, THRESH_BINARY);
	blur(eyeROI, eyeROI, cv::Size(5, 5), cv::Point(-1, -1));
	adaptiveThreshold(img2, img3, maxVal, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, 3);*/
	//Canny(thresholded, thresholded, 60, 100);
	/*namedWindow("二值图像");
	imshow("二值图像", thresholded);*/

	threshold(img2, img3, 35, 255, THRESH_BINARY);
	//threshold(img2, img3, 70, 255, THRESH_BINARY);  //图像二值化，，注意阈值变化

	//namedWindow("img3", CV_NORMAL);
	//imshow("img3", img3);

	//Canny(img3, img3, 60, 100);//边缘检测

	//namedWindow("detect img3", CV_NORMAL);

	/*namedWindow("detect img3", CV_NORMAL);
	imshow("detect img3", img3);*/

	//vector<vector<Point>>contours;
	//vector<Vec4i>hierarchy;
	//findContours(img3, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//查找出所有的圆边界
	//int index = 0;
	//for (; index >= 0; index = hierarchy[index][0])
	//{
	//	Scalar color(rand() & 255, rand() & 255, rand() & 255);
	//	drawContours(frame, contours, index, color, CV_FILLED, 8, hierarchy);
	//}
	//namedWindow("circles", CV_NORMAL);
	//imshow("circles", frame);

	//标准圆在图片上一般是椭圆，所以采用OpenCV中拟合椭圆的方法求解中心
	/*Mat pointsf;
	Mat(contours[0]).convertTo(pointsf, CV_32F);
	RotatedRect box = fitEllipse(pointsf);
	cout << box.center;*/

	vector<Vec3f> circles;
	//HoughCircles(img3, circles, HOUGH_GRADIENT, 2, 20, 100, 20, 5, 15);
	//HoughCircles(img3, circles, HOUGH_GRADIENT, 1, img3.rows / 10, 200, 100, 5, 100);
//	HoughCircles(img3, circles, HOUGH_GRADIENT, 2, 40, 50, 20, 40, 50);
	//cout << "img3.rows:" << img3.rows << endl;
	HoughCircles(img3, circles, HOUGH_GRADIENT, 2, 30, 70, 20, img3.rows/5, img3.rows / 3);
	//将得到的结果绘图  
	int flag_i = 0;
	while (!circles.size())
	{
		HoughCircles(img3, circles, HOUGH_GRADIENT, 2, 40+4* flag_i, 70, 20, img3.rows / 5, img3.rows / 3);
		flag_i++;
		//cout << "circle while" << endl;
		if (flag_i > 0)
		{
			break;
		}
	}
	if (circles.size() > 1)
	{
		/*int blockSize = 11;
		int maxVal = 255;
		Mat thresholded;
		threshold(eyeROI, thresholded, 150, 255, THRESH_BINARY);
		blur(eyeROI, eyeROI, cv::Size(5, 5), cv::Point(-1, -1));
		adaptiveThreshold(img2, img3, maxVal, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, 3);
		Canny(thresholded, thresholded, 60, 100);*/

	}
	//cout << "img size:" <<img3.size()<< endl;
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);//检测圆中心  
		circle(frame, center, 3, Scalar(255, 255, 0), -1, 8, 0);//检测圆轮廓  
		circle(frame, center, radius, Scalar(255, 0, 255), 1, 8, 0);
		//cout << "radius:" << radius << endl;
		if (j == 0 && circles.size() == 1)   //左眼
		{
			int x = last_eyes[0].x + center.x;
			//cout << "x:" << x << endl;
			start_lefteye.push_back(Point(last_eyes[0].x + center.x, last_eyes[0].y + center.y));
			//cout << "left eye:" << Point(last_eyes[0].x + center.x, last_eyes[0].y + center.y) << endl;
		}
		else if (j == 1 && circles.size() == 1)
		{
			start_lefteye.push_back(Point(last_eyes[1].x + center.x, last_eyes[1].y + center.y));
			//cout << "right eye:"<< Point(last_eyes[1].x + center.x, last_eyes[1].y + center.y) <<endl;
		}
	}
	//namedWindow("HoughResult", CV_NORMAL);
	//imshow("HoughResult", frame);
	//waitKey();
}

void eyesdetect::eyes_detect()
{
	if (center_comp.size() == 2)    //通过比较用于确定哪个坐标属于左眼和右眼
	{
		last_eyes.clear();
		if (center_comp[0].x>center_comp[1].x)
		{
			Leye_line.push_back(Point(center_comp[0].x, center_comp[0].y));
			Reye_line.push_back(Point(center_comp[1].x, center_comp[1].y));
			Rect rect_Leye(start_x + eyes[0].x + eyes[0].width*pupil_startx, strat_y + eyes[0].y + eyes[0].height*pupil_starty, pupil_width*eyes[0].width, eyes[0].height*pupil_height);
			Rect rect_Reye(start_x + eyes[1].x + eyes[1].width*0.25, strat_y + eyes[1].y + eyes[1].height*pupil_starty, pupil_width*eyes[1].width, eyes[1].height*pupil_height);
			last_eyes.push_back(rect_Leye);  //左眼右眼
			last_eyes.push_back(rect_Reye);
			left_eyeROI = frame_gray(rect_Leye);
			right_eyeROI = frame_gray(rect_Reye);
		}
		else
		{
			Leye_line.push_back(Point(center_comp[1].x, center_comp[1].y));  //记录眼睛轨迹
			Reye_line.push_back(Point(center_comp[0].x, center_comp[0].y));
			Rect rect_Leye(start_x + eyes[1].x + eyes[1].width*pupil_startx, strat_y + eyes[1].y + eyes[1].height*pupil_starty, pupil_width*eyes[1].width, eyes[1].height*pupil_height);
			Rect rect_Reye(start_x + eyes[0].x + eyes[0].width*0.25, strat_y + eyes[0].y + eyes[0].height*pupil_starty, pupil_width*eyes[0].width, eyes[0].height*pupil_height);
			last_eyes.push_back(rect_Leye);  //左眼右眼
			last_eyes.push_back(rect_Reye);
			left_eyeROI = frame_gray(rect_Leye);
			right_eyeROI = frame_gray(rect_Reye);
		}
		/*namedWindow("左眼图像", CV_NORMAL);
		imshow("左眼图像", left_eyeROI);
		namedWindow("右眼图像", CV_NORMAL);
		imshow("右眼图像", right_eyeROI);*/
		// find_pupil(left_eyeROI,0);
		//find_pupil(right_eyeROI,1);
	}
	else if (eye_detect && !last_eyes.empty())
	{
		left_eyeROI = frame_gray(last_eyes[0]);
		right_eyeROI = frame_gray(last_eyes[1]);
		/*left_eye = QString::QString("***");   //人眼检测出错只显示***
		left_eye.append(" , ");
		left_eye.append(QString::QString("***"));
		right_eye = QString::QString("***");
		right_eye.append(" , ");
		right_eye.append(QString::QString("***"));*/
		//ui.lineEdit_left->setText(left_eye);
		//ui.lineEdit_right->setText(right_eye);
	}
	ui.picture_show->setPixmap(QPixmap::fromImage(qImage));//显示  
	if (eye_detect && !left_eyeROI.empty() && !right_eyeROI.empty())
	{
		//namedWindow("左眼图像", CV_NORMAL);
		//imshow("左眼图像", left_eyeROI);
		//Size Lsize = Size(ui.left_eye->size().width()-2, ui.left_eye->size().height()-2);
		//Size Rsize = Size(ui.right_eye->size().width() - 2, ui.right_eye->size().height() - 2);		
		Size Lsize = Size(left_eyeROI.cols * 3, left_eyeROI.rows * 3);
		Size Rsize = Size(right_eyeROI.cols * 3, right_eyeROI.rows * 3);
		cv::resize(left_eyeROI, left_eyeROI, Lsize);
		cv::resize(right_eyeROI, right_eyeROI, Rsize);

		if (center_comp.size() == 2 && control_number == 0)
		{
			Rect Harris_L(0 * left_eyeROI.cols, 0.5*left_eyeROI.rows, 0.3*left_eyeROI.cols, 0.4*left_eyeROI.rows);
			Rect Harris_R(0.65*right_eyeROI.cols, 0.5* right_eyeROI.rows, 0.3*right_eyeROI.cols, 0.4*right_eyeROI.rows);
			Harris_Leye = left_eyeROI(Harris_L);
			Harris_Reye = right_eyeROI(Harris_R);
			on_GoodFeaturesToTrack(0, 0, 0);
			on_GoodFeaturesToTrack(0, 0, 1);
			//namedWindow("Leye", CV_NORMAL);
			//imshow("Leye", Harris_Reye);
			find_pupil(left_eyeROI, 0);
			find_pupil(right_eyeROI,1);
		}
		//namedWindow("左眼图像", CV_NORMAL);
		//namedWindow("左眼图像");
		//imshow("左眼图像", left_eyeROI);
		//namedWindow("右眼图像", CV_NORMAL);
		//namedWindow("右眼图像");
		//imshow("右眼图像", right_eyeROI);
		//imwrite("D:\\test\\right_eyeROIr.jpg", right_eyeROI);
		//imwrite("D:\\test\\right_eyeROIr.jpg", right_eyeROI);
		Size Lqsize = Size(ui.left_eye->size().width() - 2, ui.left_eye->size().height() - 2);
		Size Rqsize = Size(ui.right_eye->size().width() - 2, ui.right_eye->size().height() - 2);
		cv::resize(left_eyeROI, left_eyeROI, Lqsize);
		cv::resize(right_eyeROI, right_eyeROI, Rqsize);
		qlefteye = Mat2QImage(left_eyeROI);
		qrighteye = Mat2QImage(right_eyeROI);
		ui.left_eye->setPixmap(QPixmap::fromImage(qlefteye));
		ui.right_eye->setPixmap(QPixmap::fromImage(qrighteye));
		/*if (qlefteye.width() != ui.left_eye->size().width() || qlefteye.height() != ui.left_eye->size().height())
		{
		QImage newleftImg = qlefteye.scaled(ui.left_eye->size().width() - 2, ui.left_eye->size().height() - 2);
		ui.left_eye->setPixmap(QPixmap::fromImage(newleftImg));
		}
		else
		{
		ui.left_eye->setPixmap(QPixmap::fromImage(qlefteye));
		}
		if (qrighteye.width() != ui.right_eye->size().width() || qrighteye.height() != ui.right_eye->size().height())
		{
		QImage newrightImg = qrighteye.scaled(ui.right_eye->size().width() - 2, ui.right_eye->size().height() - 2);
		ui.right_eye->setPixmap(QPixmap::fromImage(newrightImg));
		}
		else
		{
		ui.right_eye->setPixmap(QPixmap::fromImage(qrighteye));
		}*/
	}
}

void eyesdetect::camshift_face(Mat frame)
{
	Mat image;
	frame.copyTo(image);
	cvtColor(image, hsv, COLOR_BGR2HSV);
	if (trackObject)	 /*click left key:-1, ctrl-c:0, original value:0*/
	{
		int _vmin = vmin, _vmax = vmax;
		inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)), Scalar(180, 256, MAX(_vmin, _vmax)), mask);
		int ch[] = { 0, 0 };
		hue.create(hsv.size(), hsv.depth());/*init hue by hsv*/
		mixChannels(&hsv, 1, &hue, 1, ch, 1);
		if (trackObject < 0) /*once object is selected by rect box*/
		{
			Mat roi(hue, selection), maskroi(mask, selection);
			calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
			normalize(hist, hist, 0, 255, CV_MINMAX);
			trackWindow = selection;
			trackObject = 1;
		}
		calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
		backproj &= mask;/*得到掩模内反向投影*/
		RotatedRect trackBox = CamShift(backproj, trackWindow, TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));
		if (trackWindow.area() <= 1)
		{
			int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
			trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
				trackWindow.x + r, trackWindow.y + r) &Rect(0, 0, cols, rows);
		}
		ellipse(image, trackBox, Scalar(0, 0, 255), 3, CV_AA);
		pt.push_back(Point(trackBox.center));
		for (int i = 0; i<pt.size() - 1; i++)
		{
			line(image, pt[i], pt[i + 1], Scalar(0, 255, 0), 2.5);
		}
	}
	//cout << "cam" << endl;
	if (eyesInit)
	{
		//imshow("CamShift Demo", image);
		//Sleep(15);
	}
	//cvWaitKey();
}

void eyesdetect::trans_point(int x, int y)  
{
	int end_x, end_y;      //控制鼠标移动的函数，自己瞎试的
	GetCursorPos(&mouse_position);//获取鼠标坐标  
	//cout << x <<"  "<<y<< endl;
	if (abs(x) > 2 )
	{
		if (abs(y) < 2|| abs(y)>6)
		{
			y = 0;
		}
	}
	else if(abs(x)>1)
	{
		if (abs(y)>1)
		{
			x = 0;
		}
	}
	else 
	{
		if(abs(y)>0)
		{ 
			x = 0;
		}
	}
	if (abs(x) > 14)
	{
		end_x = mouse_position.x + 0 * x;
	}
	else if (abs(x) > 12)
	{
		end_x = mouse_position.x + 0.5 * x;
	}
	else if (abs(x) > 10)
	{
		end_x = mouse_position.x + 0.8 * x;
	}
	else if (abs(x) > 8)
	{
		end_x = mouse_position.x + (x_speed * 4 - 4) * x;
	}
	else if (abs(x) > 6)
	{
		end_x = mouse_position.x + (x_speed * 4) * x;
	}
	else if (abs(x) > 5)
	{
		end_x = mouse_position.x + (x_speed * 2 + 10) * x;
	}
	else if (abs(x) > 4)
	{
		end_x = mouse_position.x + (x_speed * 3 + 8) * x;
	}
	else if (abs(x) > 3)
	{
		end_x = mouse_position.x + (x_speed * 4 + 6)* x;
	}
	else if (abs(x) > 2)
	{
		end_x = mouse_position.x + (x_speed *5) * x;
	}
	else if (abs(x) > 1)
	{
		end_x = mouse_position.x + (x_speed*1) * x;
	}
	else
	{
		end_x = mouse_position.x;
	}
	
	if (abs(y) > 12)
	{
		end_y = mouse_position.y + 0* y;
	}
	else if (abs(y) > 10)
	{
		end_y = mouse_position.y + 0* y;
	}
	else if (abs(y) > 8)
	{
		end_y = mouse_position.y + 0.3 * y;
	}
	else if (abs(y) > 7)
	{
		end_y = mouse_position.x + 0.5 * y;
	}
	else if (abs(y) > 6)
	{
		end_y = mouse_position.x + (abs(y_speed)*0.5) * y;
	}
	else if (abs(y) > 5)
	{
		end_y = mouse_position.x + y_speed * y;
	}
	else if (abs(y) > 4)
	{
		end_y = mouse_position.x + (y_speed * 1) * y;
	}
	else if (abs(y) > 3)
	{
		end_y = mouse_position.y + (y_speed * 5)* y;
	}
	else if (abs(y) > 2)
	{
		end_y = mouse_position.y + y_speed * 6 * y;
	}
	else if (abs(y) > 1)
	{
		end_y = mouse_position.y + (y_speed*5)*y;
	}
	else if (abs(y) > 0)
	{
		end_y = mouse_position.y + (y_speed *1)*y;
	}
	else
	{
		end_y = mouse_position.y;
	}
	SetCursorPos(end_x, end_y);
}

void eyesdetect::face_slect()
{
	if (faces.size() == 0)
	{
		campshift_flag = false;
		trackObject = -1;
		center_comp.clear();
		flag_n = 0;
		return;
	}
	else if (faces.size() > 1)                           //判断出多个人脸则选择面积最大的人脸进行检测
	{
		vector<Rect> faces_swap;
		for (size_t i = 1; i < faces.size(); i++)
		{
			if (faces[i].area() > faces[i - 1].area())
			{
				faces_swap.clear();
				faces_swap.push_back(faces[i]);
			}
		}
		faces.clear();
		faces.push_back(faces_swap[0]);
	}
}

void eyesdetect::face_model()
{
	if (faces.size() == 1)
	{
		selection.x = faces[0].x; /*the released point of mouse*/
		selection.y = faces[0].y;
		selection.width = faces[0].width; /*the width of selection box*/
		selection.height = frame.rows - faces[0].y - 10;
		selection &= Rect(0, 0, frame.cols, frame.rows);
		faces_refer.clear();        //清理上次存放的信息，更新信息
		faces_refer.push_back(faces[0]); //初次用AdaBoost算法检测的人脸信息压入栈
		eyeROI_width = faces_refer[0].width*width_weight;
		eyeROI_height = faces_refer[0].height*height_weight;
		start_x = faces[0].x + faces[0].width*startx_weight;
		strat_y = faces[0].y + faces[0].height*starty_weight;
		start_referx = faces[0].x + faces[0].width*startx_weight;
		strat_refery = faces[0].y + faces[0].height *starty_weight;
		x_bias = 0;  //用于矫正水平方向的偏差
		y_bias = 0;
		pt.clear(); //清理运动轨迹记录
		//cout << "new" << endl;
	}
	else if (faces.size() == 0)
	{
		return;
	}
}

void eyesdetect::on_GoodFeaturesToTrack(int, void*,int eyeflag)
{
	//cout << "eyeflag:"<< eyeflag << endl;
	//【1】Shi-Tomasi算法（goodFeaturesToTrack函数）的参数准备  
	vector<Point2f> corners;
	int r = 4;
	//Mat copy = g_srcImage.clone();  //复制源图像到一个临时变量中，作为感兴趣区域  
	//Mat copy = left_eyeROI.clone();
	if (eyeflag == 0)
	{
		Mat copy = Harris_Leye.clone(); 						//【3】进行Shi-Tomasi角点检测  
		goodFeaturesToTrack(//g_grayImage,//输入图像  
			Harris_Leye,
			corners,//检测到的角点的输出向量  
			g_maxCornerNumber,//角点的最大数量  
			qualityLevel,//角点检测可接受的最小特征值  
			minDistance,//角点之间的最小距离  
			Mat(),//感兴趣区域  
			blockSize,//计算导数自相关矩阵时指定的邻域范围  
			false,//不使用Harris角点检测  
			k);//权重系数  
		    int slect_x = 0;
		    int temp = corners[0].x;
		    for (unsigned int i = 0; i < corners.size(); i++)
		    {
			  circle(copy, corners[i], r, Scalar(255, 255, 0), -1, 8, 0);
			  if (corners[i].x < temp)
			  {
				  temp = corners[i].x;
				  slect_x = i;
			  }
			}
		    //namedWindow("左眼角点", CV_NORMAL);
		    //namedWindow("角点");
		    //imshow("左眼角点", copy);
			Harris_lefteye.push_back(Point(corners[slect_x].x, 0.5*left_eyeROI.rows+ corners[slect_x].y));
			circle(left_eyeROI, Point(corners[slect_x].x, 0.5*left_eyeROI.rows + corners[slect_x].y), r, Scalar(255, 255, 0), -1, 8, 0);
			Harris_lefteye.push_back(Point(corners[slect_x].x, 0.5*left_eyeROI.rows+ corners[slect_x].y));
			circle(left_eyeROI, Point(corners[slect_x].x, 0.5*left_eyeROI.rows + corners[slect_x].y), r, Scalar(255, 255, 0), -1, 8, 0);
			//cout << "left:" << Point(corners[slect_x].x, 0.5*left_eyeROI.rows + corners[slect_x].y) << endl;
	}
	else
	{
		Mat copy = Harris_Reye.clone(); 					
        goodFeaturesToTrack(Harris_Reye,corners,g_maxCornerNumber,qualityLevel,
			minDistance,Mat(),blockSize,false,k); 
		int slect_x = 0;
		int temp = corners[0].x;
		for (unsigned int i = 0; i < corners.size(); i++)
		{
			circle(copy, corners[i], r, Scalar(255, 255, 0), -1, 8, 0);
			if (corners[i].x > temp)
			{
				temp = corners[i].x;
				slect_x = i;
			}
		}
		//namedWindow("右眼角点", CV_NORMAL);
		//namedWindow("角点");
		//imshow("右眼角点", copy);
		Harris_righteye.push_back(Point(corners[slect_x].x+ 0.65*right_eyeROI.cols,corners[slect_x].y+ 0.5* right_eyeROI.rows));
		circle(right_eyeROI, Point(corners[slect_x].x + 0.65*right_eyeROI.cols, corners[slect_x].y + 0.5* right_eyeROI.rows), r, Scalar(255, 255, 0), -1, 8, 0);
		//namedWindow("换算后", CV_NORMAL);
		//namedWindow("角点");
		//imshow("换算后", right_eyeROI);
	   // cout << "right:"<< Point(corners[slect_x].x + 0.65*right_eyeROI.cols, corners[slect_x].y + 0.5* right_eyeROI.rows) <<endl;
	}
	//Size winSize = Size(5, 5);
	//Size zeroZone = Size(-1, -1);
	//TermCriteria criteria = TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);
	//cornerSubPix(g_grayImage, corners, winSize, zeroZone, criteria);	//【8】计算出亚像素角点位置  
	//cout << "min:" << slect_x << endl;
}

//void eyesdetect::find_pupil(Mat frame,int j)//寻找瞳孔函数
//{
//	Mat img2, img3, img4;
//	//cvtColor(img, cimg, COLOR_GRAY2BGR);
//	//cvtColor(frame, img2, COLOR_BGR2GRAY);   //把彩色图转换为黑白图像
//	GaussianBlur(frame, img2, Size(5, 5), 2, 2);
//	/*bilateralFilter(img2, img2, 25, 25 * 2, 25 / 2);
//	blur(img2, img2, cv::Size(6,6), cv::Point(-1, -1));*/
//	//equalizeHist(img2, img2);
//
//	//Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 11));
//
//	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 11));
//	morphologyEx(img2, img2, MORPH_CLOSE, element);
//	//equalizeHist(img2, img2);
//	/*int blockSize = 11;
//	int maxVal = 255;
//	Mat thresholded;
//	threshold(eyeROI, thresholded, 150, 255, THRESH_BINARY);
//	blur(eyeROI, eyeROI, cv::Size(5, 5), cv::Point(-1, -1));
//	adaptiveThreshold(img2, img3, maxVal, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, blockSize, 3);*/
//	//Canny(thresholded, thresholded, 60, 100);
//	/*namedWindow("二值图像");
//	imshow("二值图像", thresholded);*/
//
//	threshold(img2, img3, 40, 255, THRESH_BINARY);
//	//threshold(img2, img3, 70, 255, THRESH_BINARY);  //图像二值化，，注意阈值变化
//	
//	/*namedWindow("img2", CV_NORMAL);
//	imshow("img2", img2);*/
//
//	//Canny(img3, img3, 60, 100);//边缘检测
//
//	//namedWindow("detect img3", CV_NORMAL);
//
//	/*namedWindow("detect img3", CV_NORMAL);
//	imshow("detect img3", img3);*/
//
//	//vector<vector<Point>>contours;
//	//vector<Vec4i>hierarchy;
//	//findContours(img3, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//查找出所有的圆边界
//	//int index = 0;
//	//for (; index >= 0; index = hierarchy[index][0])
//	//{
//	//	Scalar color(rand() & 255, rand() & 255, rand() & 255);
//	//	drawContours(frame, contours, index, color, CV_FILLED, 8, hierarchy);
//	//}
//	//namedWindow("circles", CV_NORMAL);
//	//imshow("circles", frame);
//
//	//标准圆在图片上一般是椭圆，所以采用OpenCV中拟合椭圆的方法求解中心
//	/*Mat pointsf;
//	Mat(contours[0]).convertTo(pointsf, CV_32F);
//	RotatedRect box = fitEllipse(pointsf);
//	cout << box.center;*/
//
//	vector<Vec3f> circles;
//	//HoughCircles(img3, circles, HOUGH_GRADIENT, 1, img3.rows / 10, 200, 100, 5, 100);
//	HoughCircles(img3, circles, HOUGH_GRADIENT, 2, 20, 100, 20, 5, 15);
//	//将得到的结果绘图  
//	//cout << circles.size() << endl;
//	for (size_t i = 0; i < circles.size(); i++)
//	{
//		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//		int radius = cvRound(circles[i][2]);//检测圆中心  
//		circle(frame, center, 3, Scalar(255, 255, 0), -1, 8, 0);//检测圆轮廓  
//		circle(frame, center, radius, Scalar(255, 0, 255), 1, 8, 0);
//		cout << "center:" << center << endl;
//		if (j == 0 && circles.size() == 1)
//		{
//			int x = last_eyes[0].x + center.x;
//			cout << "x:" << x << endl;
//			start_lefteye.push_back(Point(last_eyes[0].x + center.x, last_eyes[0].y + center.y));
//		}
//		else if (j == 1 && circles.size() == 1)
//		{
//			start_lefteye.push_back(Point(last_eyes[1].x + center.x, last_eyes[1].y + center.y));
//		}
//	}
//	//namedWindow("HoughResult", CV_NORMAL);
//	//imshow("HoughResult", frame);
//	//waitKey();
//}
