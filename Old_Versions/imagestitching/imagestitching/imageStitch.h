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

	// ����ͼ��
    void LoadImage(string imgpath1,string imgpath2);

	// Ԥ����
    void PreProcess();

	// ��������
    void DetectFeaturePoint();

	// ������������
    void CalcuFeatureVector();

	// ͼ����׼
    void ImageMatch();

	// ���㵥Ӧ�Ծ���
    void FindHomography();

	// ͼ��ƴ��
    void WrapImage();

private:
    Mat image1, image2;
	Mat gray_image1, gray_image2;
    vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    vector< Point2f > obj;
    vector< Point2f > scene;
    Mat H;
    //ͼ2���ĸ��Ǿ�����H�任�������
    CvPoint leftTop, leftBottom, rightTop, rightBottom;
};
