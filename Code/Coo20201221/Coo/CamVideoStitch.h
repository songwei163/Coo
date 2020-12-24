#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/tracking.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include <iostream>
 
#define WRITE_VIDEO 1
#define DBG	0
#define WRITE_FPS 1
#define TRACK 1

#define MOVE_DECT 0

//#define MIN3(x,y,z) min(min((x),(y)),(z))

#define GPU 0


using namespace cv;
using namespace std;

class CamVideoStitch
{
public:
	CamVideoStitch();
	void CalcHMatrix();
	void CalcHMatrix(const String fileName1,const String fileName2);
	Mat FindHMatrix(Mat image1, Mat image2);
#if GPU	
	Mat FindHMatrixGPU(Mat image1, Mat image2);
#endif	
	void CalcFourCorner(Mat image1);
	void CameraStitch();
	void VideoStitch();
	Mat ImageMix(Mat image1, Mat image2);
	bool MoveDetect(Mat frame1, Mat frame2);
	void CalcTwoHist(Mat frame1, Mat frame2);
private:
	Mat H;
	CvPoint leftTop, leftBottom, rightTop, rightBottom;
	String m_fileNameL, m_fileNameR;
	Mat dHist1, dHist2;
	Mat ResultROI;
	Mat gray_image1, gray_image2;
	vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptors_object, descriptors_scene;
	vector< Point2f > obj;
	vector< Point2f > scene;


	vector< DMatch > matches;
	vector< DMatch > good_matches;
	Mat result;
	Rect rectROI;
};