#include "CameraStitch.h"



void CameraStitch::CalcFourCorner()
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

void CameraStitch::on_HMatrixCalcBtn_clicked()
{
    // capture the images from cams
    VideoCapture cap1(0);
    VideoCapture cap2(1);
	
    while (1)
    {
        cap1 >> image1;
        cap2 >> image2;

        imshow("first image", image1);
        imshow("second image", image2);
        if (cvWaitKey(2) > 0)
        {
            break; //�����ڰ�������˳�
        }
    }

    if (!image1.data || !image2.data)
    {
        cout << "ͼ�����ʧ�ܣ�����" << endl;
        exit(EXIT_FAILURE);
    }

#if 1

    //ͼƬԤ����
    cvtColor(image1, gray_image1, CV_RGB2GRAY);
    cvtColor(image2, gray_image2, CV_RGB2GRAY);
    if (!gray_image1.data || !gray_image2.data)
    {
        cout << "�Ҷ�ͼ�����ʧ�ܣ�����" << endl;
        exit(EXIT_FAILURE);
    }

    int minHessian = 400;
    SurfFeatureDetector detector(minHessian);
    detector.detect(gray_image1, keypoints_object);
    detector.detect(gray_image2, keypoints_scene);
    // Draw keypoints
    Mat img_keypoints_1; Mat img_keypoints_2;
    drawKeypoints(image1,keypoints_object,img_keypoints_1,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
    drawKeypoints(image2,keypoints_scene,img_keypoints_2,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
    // Show detected (drawn) keypoints
    imshow("Keypoints 1", img_keypoints_1 );
    imshow("Keypoints 2", img_keypoints_2 );

    SurfDescriptorExtractor extractor;
    extractor.compute(gray_image1, keypoints_object, descriptors_object);
    extractor.compute(gray_image2, keypoints_scene, descriptors_scene);
    cout << "������������ɹ�" << endl;

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


    H = findHomography(obj, scene, CV_RANSAC);
    cout << "��Ӧ�������ɹ�" << endl;

    //�����ĸ��ǵ�����
    CalcFourCorner();

    cap1.release();
    cap2.release();
#endif
}

void CameraStitch::on_videoStitchingBtn_clicked()
{
    VideoCapture cap1(0);
    VideoCapture cap2(1);

    VideoWriter writerResult("result.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(MIN(rightTop.x, rightBottom.x), image1.rows));

    if (!cap1.isOpened())  // check if we succeeded
    {
        cout << "01��ʧ����������ͷ�������" << endl;
        exit(EXIT_FAILURE);
    }
    if (!cap2.isOpened())  // check if we succeeded
    {
        cout << "02��ʧ����������ͷ�������" << endl;
        exit(EXIT_FAILURE);
    }

    Mat result;
    int start = MIN(leftTop.x, leftBottom.x);//��ʼλ�ã����ص��������߽�
    double alpha = 1;//img2�����ص�Ȩ��

    double fps;
    char string[10];  // ���ڴ��֡�ʵ��ַ���
    cv::namedWindow("Camera FPS");
    double t = 0;
	
    while (1) //һֱѭ��
    {
        t = (double)cv::getTickCount();
    	
        cap1 >> image1;
        cap2 >> image2;

        imshow("Camera Video 1", image1); //��ʾ��Ƶ���ݵ�ͼƬ
        imshow("Camera Video 2", image2); //��ʾ��Ƶ���ݵ�ͼƬ

        warpPerspective(image1, result, H, cv::Size(MIN(rightTop.x, rightBottom.x), image1.rows));

        //��Ȩƽ��
        Mat image2Roi(image2, cv::Rect(0, 0, MIN(leftTop.x, leftBottom.x), image2.rows));
        Mat resultRoi(result, cv::Rect(0, 0, MIN(leftTop.x, leftBottom.x), result.rows));
        image2Roi.copyTo(resultRoi);
    
        double processWidth = image2.cols - start;//�ص�����Ŀ��

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

    // getTickcount���������شӲ���ϵͳ��������ǰ�������ĺ�����
    // getTickFrequency����������ÿ��ļ�ʱ������
    // tΪ�ô�����ִ�����ĵ�ʱ��,��λΪ��,fpsΪ�䵹��
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        fps = 1.0 / t;


        sprintf_s(string, "%.2f", fps);      // ֡�ʱ�����λС��
        std::string fpsString("FPS:");
        fpsString += string;                    // ��"FPS:"�����֡����ֵ�ַ���
        // ��֡����Ϣд�����֡��
        putText(result, // ͼ�����
            fpsString,                  // string����������
            cv::Point(2, 50),           // �������꣬�����½�Ϊԭ��
            cv::FONT_HERSHEY_DUPLEX,   // ��������
            2, // �����С
            cv::Scalar(0, 255, 0),2,8);       // ������ɫ
        imshow("Camera FPS", result);

        writerResult << result;

        if (waitKey(50) > 0)
        {
            break; //�����ڰ�������˳�
        }
    }
    cout<<"Success"<<endl;
    cap1.release();
    cap2.release();
}
