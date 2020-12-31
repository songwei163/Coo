#pragma once

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


class CameraStitch 
{
public:

    void CalcFourCorner();

    void on_HMatrixCalcBtn_clicked();

    void on_videoStitchingBtn_clicked();

    Mat videoImageSrc1, videoImageSrc2;
    Mat image1, image2, gray_image1, gray_image2;
    vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    vector< Point2f > obj;
    vector< Point2f > scene;
    Mat H;
    //图2的四个角经矩阵H变换后的坐标
    CvPoint leftTop, leftBottom, rightTop, rightBottom;
};