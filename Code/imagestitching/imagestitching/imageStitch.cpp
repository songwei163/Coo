#include "imageStitch.h"

void StitchImage::LoadImage(string imgpath1, string imgpath2)
{
    VideoCapture cap1(0);   // right
    VideoCapture cap2(1);   // left

	if (!cap1.isOpened() || !cap2.isOpened())
	{
        cout << "��������ͷ������" << endl;
        exit(EXIT_FAILURE);
	}

    while(1)
    {
	    cap1 >> image1;
		cap2 >> image2;
    	
        imshow("image1", image1);
        imshow("image2", image2);
    	
    	if(waitKey(200)>0)
    	{
            break;
    	}
    }
	
    cap1.release();
    cap2.release();

    imwrite("r.jpg", image1);
    imwrite("l.jpg", image2);
    if (!image1.data || !image2.data)
    {
        cout << "ͼ�����ʧ�ܣ�����" << endl;  
        exit(EXIT_FAILURE);
    }
}

void StitchImage::PreProcess()
{
    //ͼƬԤ����
    cvtColor(image1, gray_image1, CV_RGB2GRAY);
    cvtColor(image2, gray_image2, CV_RGB2GRAY);
    if (!gray_image1.data || !gray_image2.data)
    {
        cout << "�Ҷ�ͼ�����ʧ�ܣ�����" << endl;
        exit(EXIT_FAILURE);
    }
}

void StitchImage::DetectFeaturePoint()
{
    int minHessian = 400;
    SurfFeatureDetector detector(minHessian); // SURF�㷨�е�hessian��ֵ
    // ����detect��������SURF�����ؼ��㣬������vector������
    detector.detect(gray_image1, keypoints_object);
    detector.detect(gray_image2, keypoints_scene);

    //-- Draw keypoints
    Mat img_keypoints_1; Mat img_keypoints_2;
	// ��һ��������ԭͼ���ڶ�����������Ҫ���Ƶĵ㣬������������Ŀ��ͼ�񣬵��ĸ������������ɫ��Ϣ��Ĭ��Ϊ�����ɫ��
	/*
	 * 	
	������Ļ���ģʽ����ʵ�����������������Щ��Ϣ��Ҫ���ƣ���Щ����Ҫ���ƣ������¼���ģʽ��ѡ��
	DRAW_MATCHES_FLAGS_DEFAULT��ֻ���������������㣬��ʾ��ͼ���Ͼ���һ����СԲ�㣬ÿ��СԲ���Բ�����궼������������ꡣ
	DRAW_MATCHES_FLAGS_DRAW_OVER_OUTIMG�����������������ͼ�񣬶���ֱ�������ͼ������ռ���ƣ�Ҫ�������ͼ���������һ����ʼ�����˵ģ�size��type�����Ѿ���ʼ���õı�����
	DRAW_MATCHES_FLAGS_NOT_DRAW_SINGLE_POINTS ������������㲻�����ơ�
	DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS�������������ʱ����Ƶ���һ�������з����Բ�����ַ���ͬʱ��ʾͼ������꣬size�ͷ�����������ʾ������һ�ֻ��Ʒ�ʽ
	 */
    drawKeypoints(image1, keypoints_object, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    drawKeypoints(image2, keypoints_scene, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    //-- Show detected (drawn) keypoints
    imshow("Keypoints 1", img_keypoints_1);
    imshow("Keypoints 2", img_keypoints_2);
}

void StitchImage::CalcuFeatureVector()
{
    // ����������������������
    SurfDescriptorExtractor extractor;
    extractor.compute(gray_image1, keypoints_object, descriptors_object);
    extractor.compute(gray_image2, keypoints_scene, descriptors_scene);
    cout << "������������ɹ�" << endl;
}

void StitchImage:: ImageMatch()
{
    FlannBasedMatcher matcher;
    vector< DMatch > matches;
    matcher.match(descriptors_object, descriptors_scene, matches);
    double max_dist = 0;
    double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for (int i = 0; i < descriptors_object.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }
    //-- Use only "good" matches (i.e. whose distance is less than 3*min_dist )
    vector< DMatch > good_matches;
    for (int i = 0; i < descriptors_object.rows; i++)
    {
        if (matches[i].distance < 3 * min_dist)
        {
            good_matches.push_back(matches[i]);
        }
    }

    for (unsigned int i = 0; i < good_matches.size(); i++)
    {
        //-- Get the keypoints from the good matches
        obj.push_back(keypoints_object[good_matches[i].queryIdx].pt);
        scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
    }
    cout << "����������׼�ɹ�" << endl;
}

void StitchImage::FindHomography()
{
    H = findHomography(obj, scene, CV_RANSAC);
    cout << "��Ӧ�������ɹ�" << endl;
    CalcuFourCorner();
}

void StitchImage::WrapImage()
{
    Mat result;
    warpPerspective(image1, result, H, cv::Size(MIN(rightTop.x, rightBottom.x), image1.rows));
    imshow("result0", result);

    // // ��ƴ��
    // Mat half(result,cv::Rect(0,0,image2.cols,image2.rows));
    // image2.copyTo(half);
    // imshow( "Result", result );

    //��Ȩƽ��
    Mat image2Roi(image2, cv::Rect(0, 0, MIN(leftTop.x, leftBottom.x), image2.rows));
    Mat resultRoi(result, cv::Rect(0, 0, MIN(leftTop.x, leftBottom.x), result.rows));
    image2Roi.copyTo(resultRoi);
    imshow("result1", result);//��ʾΪ�ں�֮ǰ��ͼ��

    int start = MIN(leftTop.x, leftBottom.x);//��ʼλ�ã����ص��������߽�
    double processWidth = image2.cols - start;//�ص�����Ŀ��
    double alpha = 1;//img2�����ص�Ȩ��

    for (int i = 0; i < result.rows; i++)//������
    {
        uchar* image2P = image2.ptr<uchar>(i);
        uchar* resultP = result.ptr<uchar>(i);
        for (int j = start; j < image2.cols; j++)//�����ص��������
        {
            if (resultP[j * 3] < 50 && resultP[j * 3 + 1] < 50 && resultP[j * 3 + 2] < 50)
            {
                alpha = 1;
            }
            else
            {   //img1�����ص�Ȩ�أ��뵱ǰ�������ص�������߽�ľ��������
                alpha = (processWidth - (j - start)) / processWidth;
            }
            resultP[j * 3] = image2P[j * 3] * alpha + resultP[j * 3] * (1 - alpha);//Bͨ��
            resultP[j * 3 + 1] = image2P[j * 3 + 1] * alpha + resultP[j * 3 + 1] * (1 - alpha);//Gͨ��
            resultP[j * 3 + 2] = image2P[j * 3 + 2] * alpha + resultP[j * 3 + 2] * (1 - alpha);//Rͨ��
        }
    }
    imshow("result", result);
    imwrite("result.jpg", result);
}

//����ͼ1���ĸ��Ǿ�����H�任�������
void StitchImage::CalcuFourCorner()
{
    Mat Htmp;
    H.copyTo(Htmp);
    CvMat cvH = Htmp;


    //����ͼ1���ĸ��Ǿ�����H�任�������
    double v2[] = { 0,0,1 };//���Ͻ�
    double v1[3];//�任�������ֵ
    CvMat V2 = cvMat(3, 1, CV_64FC1, v2);
    CvMat V1 = cvMat(3, 1, CV_64FC1, v1);

    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//����˷�
    leftTop.x = cvRound(v1[0] / v1[2]);
    leftTop.y = cvRound(v1[1] / v1[2]);
    cout << leftTop.x << " " << leftTop.y << endl;


    //��v2��������Ϊ���½�����
    v2[0] = 0;
    v2[1] = image1.rows;
    V2 = cvMat(3, 1, CV_64FC1, v2);
    V1 = cvMat(3, 1, CV_64FC1, v1);
    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//����˷�
    leftBottom.x = cvRound(v1[0] / v1[2]);
    leftBottom.y = cvRound(v1[1] / v1[2]);
    cout << leftBottom.x << " " << leftBottom.y << endl;

    //��v2��������Ϊ���Ͻ�����
    v2[0] = image1.cols;
    v2[1] = 0;
    V2 = cvMat(3, 1, CV_64FC1, v2);
    V1 = cvMat(3, 1, CV_64FC1, v1);
    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//����˷�
    rightTop.x = cvRound(v1[0] / v1[2]);
    rightTop.y = cvRound(v1[1] / v1[2]);
    cout << rightTop.x << " " << rightTop.y << endl;

    //��v2��������Ϊ���½�����
    v2[0] = image1.cols;
    v2[1] = image1.rows;
    V2 = cvMat(3, 1, CV_64FC1, v2);
    V1 = cvMat(3, 1, CV_64FC1, v1);
    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//����˷�
    rightBottom.x = cvRound(v1[0] / v1[2]);
    rightBottom.y = cvRound(v1[1] / v1[2]);
    cout << rightBottom.x << " " << rightBottom.y << endl;
}
