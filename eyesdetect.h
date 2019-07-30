#pragma once
//#include <stdio.h>
//#include <iostream>
//#include<string.h>
//#include<time.h> 
//#include <sstream>
//#include <stdlib.h>
//#include<QFile.h>
#include "ui_eyesdetect.h"
#include <Windows.h>
#include<math.h>
#include <QtWidgets/QMainWindow>
#include<QFileDialog>
#include <QMessageBox>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace std;
using namespace cv;
#define startx_weight 0
#define starty_weight 0.10
#define width_weight 1.0
#define height_weight 0.5
#define False 0
#define True 1
#define pupil_startx 0.05
#define pupil_starty 0.2
#define pupil_width 0.8
#define pupil_height 0.6


class eyesdetect : public QMainWindow
{
	Q_OBJECT

public:
	eyesdetect(QWidget *parent = Q_NULLPTR);
	string face_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	//string eyes_cascade_glass_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";  //戴眼镜
	//string eyes_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye.xml";
	string eyes_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;
private slots:
    void start_clink(); //开始按钮
	void pause_clink(); //暂停按钮
	void open_clink();
	void quit_clink();
	void slect_clink();
	void control_slect();
	void open_videofile();
	void set_mousespeed();
private:
	Ui::eyesdetectClass ui;
	void open_cemera();
	QImage  Mat2QImage(Mat cvImg);   //Mat转换成QImage函数
	void detectAndDisplay(Mat frame);//人脸人眼检测函数
	void find_pupil(Mat frame,int j);
	void camshift_face(Mat frame);
	void trans_point(int x, int y);
	void face_slect();
	void face_model();
	void on_GoodFeaturesToTrack(int, void*,int eyeflag);
	void eyes_detect();

	VideoCapture cap;
	QImage qImage;
	QImage qlefteye;
	QImage qrighteye;
	QString left_eye, right_eye;         //眼睛坐标
	QString right_result, detect_picture;//统计结果
	Mat frame;
	bool eye_detect=False;//状态信息，防止随意按按钮出现的bug
	bool open_eye = True;//用于判断是否眨眼，用于判断是否需要更新模板
	bool Lstop=False;
	bool Lpause=False;
	bool campshift_flag = false;
	double fps;
	char string_fps[10];   //用于存放帧率的字符串
	double t = 0;          //计时每帧检测时间
	double all_time;       //统计检测总时间
	int slect_num = 0;     //用于确定设置信息，确定是否戴眼镜
	int control_number = 0;//用于判断控制方式,0眼控
	float all_right = 0;
	float all_face = 0;
	int all_picture = 0;   //检测帧数
	float last_right;      //最终检测准确率
	int detect_error=0;
	vector<Rect> last_eyes;
	vector<Point> find_lefteye; //记录find_pupil函数定位的坐标
	vector<Point> find_righteye;
	vector<Point> start_lefteye; //记录find_pupil眼睛的起始坐标，用于坐标转换
	vector<Point> start_righteye;
	vector<Point> Harris_lefteye;//记录角点坐标
	vector<Point> Harris_righteye;

	Mat left_eyeROI;    //截取的眼部区域
	Mat right_eyeROI;
	Mat Harris_Leye;    //选区角点检测区域
	Mat Harris_Reye;
	Mat eyeROI;        //三庭五眼法追踪的眼框
	Mat frame_gray;
	int histSize = 200;
	float histR[2] = { 0,255 };
	const float *histRange = histR;
	int channels[2] = { 0,1 };
	POINT mouse_position;//鼠标坐标
	Rect rect;
	vector<Rect> eyes;
	vector<Point> pt;           //保存目标追踪轨迹
	vector<Rect> faces;
	vector<Point> Leye_line;     //眼部运动轨迹

	vector<Point> Reye_line;
	vector<Rect> faces_refer;   //记录追踪开始时脸部坐标信息
	vector<Point2f> center_comp;//用于记录左右眼坐标
	int start_x, strat_y;       //记录追踪起始坐标
	int start_referx, strat_refery;
	int eyeROI_width, eyeROI_height;
	int flag_n = 0;             //控制递归调用次数，避免陷入死循环
	int x_bias, y_bias;
	int x_speed = 5;           //鼠标速度设置
    int y_speed = 5;
	int eyeclose_flag = 0;

	bool selectObject = false; 
	bool eyesInit = false;   //眼睛初始化，建立屏幕映射
	int trackObject = -1;
	Point origin; 
	Rect selection; //选区跟踪人脸坐标
	int vmin = 20, vmax = 256, smin = 100;
	Rect trackWindow; //跟踪窗口
	int hsize = 16; //划分直方图bins个数
	float hranges[2] = { 0,180 };
	const float* phranges = hranges;
	Mat  hsv, hue, mask, hist, backproj;
	//Mat g_srcImage, g_grayImage; //寻找角点
	int g_maxCornerNumber = 1;
	//int g_maxTrackbarNumber = 50;
	double qualityLevel = 0.01;//角点检测可接受的最小特征值  0.01
	double minDistance = 5;//角点之间的最小距离  
	int blockSize = 7;//计算导数自相关矩阵时指定的邻域范围  
	double k = 0.1;//权重系数  0.04
};
