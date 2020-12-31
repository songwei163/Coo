#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaarithm.hpp"
#include "opencv2/cudafeatures2d.hpp"
#include "opencv2/xfeatures2d/cuda.hpp"
#include "opencv2/cudaimgproc.hpp"
#include <iostream>

#define WRITE_VIDEO 0
#define DBG	0

using namespace cv;
using namespace std;

class CamVideoStitch
{
public:
	void CalcHMatrix();
	Mat FindHMatrix(Mat image1, Mat image2);
	Mat FindHMatrixGPU(Mat image1, Mat image2);
	void CalcFourCorner(Mat image1);
	void VideoStitch();
	Mat ImageMix(Mat image1, Mat image2);
	Mat MoveDetect(Mat frame1, Mat frame2);

private:
	Mat H;
	CvPoint leftTop, leftBottom, rightTop, rightBottom;
};

