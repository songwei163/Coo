#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


class StitchImage
{
public:
    void CalcuFourCorner();

	// 载入图像
    void LoadImage(string imgpath1,string imgpath2);

	// 预处理
    void PreProcess();

	// 特征点检测
    void DetectFeaturePoint();

	// 计算特征向量
    void CalcuFeatureVector();

	// 图像配准
    void ImageMatch();

	// 计算单应性矩阵
    void FindHomography();

	// 图像拼接
    void WrapImage();

private:
    Mat image1, image2;
	Mat gray_image1, gray_image2;
    vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    vector< Point2f > obj;
    vector< Point2f > scene;
    Mat H;
    //图2的四个角经矩阵H变换后的坐标
    CvPoint leftTop, leftBottom, rightTop, rightBottom;
};
