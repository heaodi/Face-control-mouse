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
	//string eyes_cascade_glass_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";  //���۾�
	//string eyes_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye.xml";
	string eyes_cascade_name = "C:\\opencv\\sources\\data\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml";
	CascadeClassifier face_cascade;
	CascadeClassifier eyes_cascade;
private slots:
    void start_clink(); //��ʼ��ť
	void pause_clink(); //��ͣ��ť
	void open_clink();
	void quit_clink();
	void slect_clink();
	void control_slect();
	void open_videofile();
	void set_mousespeed();
private:
	Ui::eyesdetectClass ui;
	void open_cemera();
	QImage  Mat2QImage(Mat cvImg);   //Matת����QImage����
	void detectAndDisplay(Mat frame);//�������ۼ�⺯��
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
	QString left_eye, right_eye;         //�۾�����
	QString right_result, detect_picture;//ͳ�ƽ��
	Mat frame;
	bool eye_detect=False;//״̬��Ϣ����ֹ���ⰴ��ť���ֵ�bug
	bool open_eye = True;//�����ж��Ƿ�գ�ۣ������ж��Ƿ���Ҫ����ģ��
	bool Lstop=False;
	bool Lpause=False;
	bool campshift_flag = false;
	double fps;
	char string_fps[10];   //���ڴ��֡�ʵ��ַ���
	double t = 0;          //��ʱÿ֡���ʱ��
	double all_time;       //ͳ�Ƽ����ʱ��
	int slect_num = 0;     //����ȷ��������Ϣ��ȷ���Ƿ���۾�
	int control_number = 0;//�����жϿ��Ʒ�ʽ,0�ۿ�
	float all_right = 0;
	float all_face = 0;
	int all_picture = 0;   //���֡��
	float last_right;      //���ռ��׼ȷ��
	int detect_error=0;
	vector<Rect> last_eyes;
	vector<Point> find_lefteye; //��¼find_pupil������λ������
	vector<Point> find_righteye;
	vector<Point> start_lefteye; //��¼find_pupil�۾�����ʼ���꣬��������ת��
	vector<Point> start_righteye;
	vector<Point> Harris_lefteye;//��¼�ǵ�����
	vector<Point> Harris_righteye;

	Mat left_eyeROI;    //��ȡ���۲�����
	Mat right_eyeROI;
	Mat Harris_Leye;    //ѡ���ǵ�������
	Mat Harris_Reye;
	Mat eyeROI;        //��ͥ���۷�׷�ٵ��ۿ�
	Mat frame_gray;
	int histSize = 200;
	float histR[2] = { 0,255 };
	const float *histRange = histR;
	int channels[2] = { 0,1 };
	POINT mouse_position;//�������
	Rect rect;
	vector<Rect> eyes;
	vector<Point> pt;           //����Ŀ��׷�ٹ켣
	vector<Rect> faces;
	vector<Point> Leye_line;     //�۲��˶��켣

	vector<Point> Reye_line;
	vector<Rect> faces_refer;   //��¼׷�ٿ�ʼʱ����������Ϣ
	vector<Point2f> center_comp;//���ڼ�¼����������
	int start_x, strat_y;       //��¼׷����ʼ����
	int start_referx, strat_refery;
	int eyeROI_width, eyeROI_height;
	int flag_n = 0;             //���Ƶݹ���ô���������������ѭ��
	int x_bias, y_bias;
	int x_speed = 5;           //����ٶ�����
    int y_speed = 5;
	int eyeclose_flag = 0;

	bool selectObject = false; 
	bool eyesInit = false;   //�۾���ʼ����������Ļӳ��
	int trackObject = -1;
	Point origin; 
	Rect selection; //ѡ��������������
	int vmin = 20, vmax = 256, smin = 100;
	Rect trackWindow; //���ٴ���
	int hsize = 16; //����ֱ��ͼbins����
	float hranges[2] = { 0,180 };
	const float* phranges = hranges;
	Mat  hsv, hue, mask, hist, backproj;
	//Mat g_srcImage, g_grayImage; //Ѱ�ҽǵ�
	int g_maxCornerNumber = 1;
	//int g_maxTrackbarNumber = 50;
	double qualityLevel = 0.01;//�ǵ���ɽ��ܵ���С����ֵ  0.01
	double minDistance = 5;//�ǵ�֮�����С����  
	int blockSize = 7;//���㵼������ؾ���ʱָ��������Χ  
	double k = 0.1;//Ȩ��ϵ��  0.04
};
