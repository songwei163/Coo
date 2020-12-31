#include "CamVideoStitch.h"

CamVideoStitch::CamVideoStitch()
{
}

void CamVideoStitch::CalcHMatrix()
{
    // 开启摄像头
    VideoCapture cap1(1);
    VideoCapture cap2(0);

    if (!cap1.isOpened() || !cap2.isOpened())
    {
        cout << "摄像头打开失败！！！" << endl;
        exit(EXIT_FAILURE);
    }

    Mat videoImage1;
    Mat videoImage2;

    cout << "请调整两个摄像头位置，保证有一定的重合区域。" << endl;
    cout << "调整好后按任意键退出。" << endl;

    while (true)
    {
        cap1 >> videoImage1;
        cap2 >> videoImage2;

        if (videoImage1.empty() || videoImage2.empty())
        {
            cout << "读取当前帧失败！！！" << endl;
            exit(EXIT_FAILURE);
        }

        imshow("Camera Video 1", videoImage1); //显示视频内容的图片
        imshow("Camera Video 2", videoImage2); //显示视频内容的图片

        if (waitKey(20) > 0)
        {
            break;
        }
    }
    destroyWindow("Camera Video 1");
    destroyWindow("Camera Video 2");

    // 计算H矩阵
    FindHMatrix(videoImage1, videoImage2);
    cout << "单应矩阵计算成功" << endl;

    //计算四个角的坐标
    CalcFourCorner(videoImage1);

    // 释放摄像头资源
    cap1.release();
    cap2.release();
}

void CamVideoStitch::CalcHMatrix(const String fileNameR, const String fileNameL)
{
    if (fileNameR.empty() || fileNameL.empty())
    {
        cout << "载入有效的视频文件路径！！！" << endl;
        exit(EXIT_FAILURE);
    }

    m_fileNameL = fileNameL;
    m_fileNameR = fileNameR;

    // 打开本地视频文件
    VideoCapture cap1(m_fileNameR);
    VideoCapture cap2(m_fileNameL);

    if (!cap1.isOpened() || !cap2.isOpened())
    {
        cout << "视频文件打开失败！！！" << endl;
        exit(EXIT_FAILURE);
    }

    Mat videoImage1;
    Mat videoImage2;

    while (true)
    {
        cap1 >> videoImage1;
        cap2 >> videoImage2;

        if (videoImage1.empty() || videoImage2.empty())
        {
            cout << "读取当前帧失败！！！" << endl;
            exit(EXIT_FAILURE);
        }

        imshow("Video 1", videoImage1); //显示视频内容的图片
        imshow("Video 2", videoImage2); //显示视频内容的图片

        if (waitKey(20) > 0)
        {
            break;
        }
    }

    destroyWindow("Video 1");
    destroyWindow("Video 2");

    // 计算H矩阵
    H = FindHMatrix(videoImage1, videoImage2);
    cout << "单应矩阵计算成功" << endl;

    //计算四个角的坐标
    CalcFourCorner(videoImage1);

    // 释放摄像头资源
    cap1.release();
    cap2.release();
}

Mat CamVideoStitch::FindHMatrix(Mat image1, Mat image2)
{
    if (image1.empty() || image2.empty())
    {
        cout << "图像为空" << endl;
        exit(-1);
    }

    cvtColor(image1, gray_image1, COLOR_RGB2GRAY);
    cvtColor(image2, gray_image2, COLOR_RGB2GRAY);

    int minHessian = 400;
    Ptr<xfeatures2d::SURF> surf = xfeatures2d::SURF::create(minHessian);

    surf->detect(gray_image1, keypoints_object);
    surf->detect(gray_image2, keypoints_scene);
    surf->compute(gray_image1, keypoints_object, descriptors_object);
    surf->compute(gray_image2, keypoints_scene, descriptors_scene);

    FlannBasedMatcher matcher;
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

    Mat imgMatches;
#if DBG
    drawMatches(image1, keypoints_object, image2, keypoints_scene, good_matches, imgMatches, Scalar::all(-1), Scalar(0, 0, 255), Mat(), DrawMatchesFlags::DEFAULT);

    imshow("匹配图", imgMatches);
#endif
    // 计算目标映射到场景的变换矩阵
    // 即右图映射到左图的H矩阵
    H = findHomography(obj, scene, RANSAC);

    {
        keypoints_object.resize(0);
        keypoints_scene.resize(0);
        matches.resize(0);
        good_matches.resize(0);
        obj.resize(0);
        scene.resize(0);
    }

    return H;
}

#if GPU
Mat CamVideoStitch::FindHMatrixGPU(Mat image1, Mat image2)
{
    cuda::GpuMat gpu_object_image, gpu_scene_image;
    gpu_object_image.upload(image1);
    gpu_scene_image.upload(image2);

    cuda::GpuMat gpu_gray_object_image, gpu_gray_scene_image;
    cuda::cvtColor(gpu_object_image, gpu_gray_object_image, CV_RGB2GRAY);
    cuda::cvtColor(gpu_scene_image, gpu_gray_scene_image, CV_RGB2GRAY);

    cuda::GpuMat gpu_keypoints_scene, gpu_keypoints_object;       // GPU 关键点
    vector<KeyPoint> keypoints_scene, keypoints_object; // CPU 关键点
    cuda::GpuMat gpu_descriptors_scene, gpu_descriptors_object;   // GPU 描述子
    Mat descriptors_object;

    int minHessian = 400;
    cuda::SURF_CUDA surf(minHessian);

    surf(gpu_gray_object_image, cuda::GpuMat(), gpu_keypoints_object, gpu_descriptors_object);
    surf(gpu_gray_scene_image, cuda::GpuMat(), gpu_keypoints_scene, gpu_descriptors_scene);

    // Brute Force 暴力匹配器
    Ptr<cuda::DescriptorMatcher> matcher = cuda::DescriptorMatcher::createBFMatcher();
    vector<DMatch> matches;
    matcher->match(gpu_descriptors_object, gpu_descriptors_scene, matches);// 最近邻 2个

    surf.downloadKeypoints(gpu_keypoints_object, keypoints_object);
    surf.downloadKeypoints(gpu_keypoints_scene, keypoints_scene);
    gpu_descriptors_object.download(descriptors_object);

    Mat img_matches;
    // drawMatches(Mat(gpu_object_image), keypoints_object,Mat(gpu_scene_image), keypoints_scene,  matches, img_matches);

    // imshow("matches", img_matches);


    vector<Point2f> obj;
    vector<Point2f> scene;
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


    H = findHomography(obj, scene, RANSAC);

    return H;
}
#endif

void CamVideoStitch::CalcFourCorner(Mat image1)
{
    Mat Htmp;
    H.copyTo(Htmp);
    CvMat cvH = Htmp;


    //计算图1的四个角经矩阵H变换后的坐标
    double v2[] = { 0,0,1 };//左上角
    double v1[3];//变换后的坐标值
    CvMat V2 = cvMat(3, 1, CV_64FC1, v2);
    CvMat V1 = cvMat(3, 1, CV_64FC1, v1);

    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//矩阵乘法
    leftTop.x = cvRound(v1[0] / v1[2]);
    leftTop.y = cvRound(v1[1] / v1[2]);
    cout << leftTop.x << " " << leftTop.y << endl;


    //将v2中数据设为左下角坐标
    v2[0] = 0;
    v2[1] = image1.rows;
    V2 = cvMat(3, 1, CV_64FC1, v2);
    V1 = cvMat(3, 1, CV_64FC1, v1);
    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//矩阵乘法
    leftBottom.x = cvRound(v1[0] / v1[2]);
    leftBottom.y = cvRound(v1[1] / v1[2]);
    cout << leftBottom.x << " " << leftBottom.y << endl;

    //将v2中数据设为右上角坐标
    v2[0] = image1.cols;
    v2[1] = 0;
    V2 = cvMat(3, 1, CV_64FC1, v2);
    V1 = cvMat(3, 1, CV_64FC1, v1);
    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//矩阵乘法
    rightTop.x = cvRound(v1[0] / v1[2]);
    rightTop.y = cvRound(v1[1] / v1[2]);
    cout << rightTop.x << " " << rightTop.y << endl;

    //将v2中数据设为右下角坐标
    v2[0] = image1.cols;
    v2[1] = image1.rows;
    V2 = cvMat(3, 1, CV_64FC1, v2);
    V1 = cvMat(3, 1, CV_64FC1, v1);
    cvGEMM(&cvH, &V2, 1, 0, 1, &V1);//矩阵乘法
    rightBottom.x = cvRound(v1[0] / v1[2]);
    rightBottom.y = cvRound(v1[1] / v1[2]);
    cout << rightBottom.x << " " << rightBottom.y << endl;
}

void CamVideoStitch::CameraStitch()
{
#if TRACK
    // 追踪模块
    Ptr<Tracker> tracker;

    tracker = TrackerCSRT::create();
    Rect2d initBB;
#endif

    char key;
    VideoCapture cap1(1);
    VideoCapture cap2(0);
    cout << cap1.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << cap1.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << cap1.get(CAP_PROP_FPS) << endl;

    if (!cap1.isOpened() || !cap2.isOpened())
    {
        cout << "摄像头打开失败！！！" << endl;
        cout << __LINE__ << endl;
        exit(EXIT_FAILURE);
    }

    Mat videoImage1;
    Mat videoImage2;

    cap1 >> videoImage1;
    cap2 >> videoImage2;
#if WRITE_VIDEO
    VideoWriter writerResult("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(MIN(rightTop.x, rightBottom.x), videoImage1.rows));
#endif

    double fps;
    char string[10];  // 帧率字符串
    cv::namedWindow("Camera FPS");
    double t = 0;
    int count = 0;
    Mat temp;
    bool flag = 0;

#if MOVE_DECT
    while (key = waitKey(20)) {
        cap1 >> videoImage1;
        cap2 >> videoImage2;
        ImageMix(videoImage1, videoImage2);
        imshow("ImageMix", result);
        if (key == 'w')
        {
            rectROI = selectROI("ImageMix", result, false, false);
            break;
        }
    }    
    ResultROI = result(rectROI);
    destroyWindow("ImageMix");
#endif


    while (key = waitKey(20))
    {
        t = (double)cv::getTickCount();

        cap1 >> videoImage1;
        cap2 >> videoImage2;

#if DBG 	
        imshow("Camera Video 1", videoImage1); //显示视频内容的图片
        imshow("Camera Video 2", videoImage2); //显示视频内容的图片
#endif

        count++;

        if (videoImage1.empty() || videoImage2.empty()) {
            continue;
        }

#if MOVE_DECT
        cout << "result: rows" << result.rows << endl;
        cout << "result: cols" << result.cols << endl;

        cout << "rectROI: rows" << rectROI.height << endl;
        cout << "rectROI: cols" << rectROI.width << endl;

        cout << "Result: rows" << ResultROI.rows << endl;
        cout << "Result: cols" << ResultROI.cols << endl;

        if (count == 1)
        {
            flag = MoveDetect(ResultROI, ResultROI);
        }
        else
        {
            flag = MoveDetect(temp, ResultROI);
        }
        temp = ResultROI.clone();

        if (flag)
        {
            // 计算H矩阵
            FindHMatrix(videoImage1, videoImage2);
            cout << "单应矩阵计算成功" << endl;

            //计算四个角的坐标
            CalcFourCorner(videoImage1);
        }
#endif

        ImageMix(videoImage1, videoImage2);

#if TRACK
        if (key == 's')
        {
            if (!tracker->empty())
            {
                tracker.release();
                tracker = TrackerCSRT::create();
            }

            initBB = selectROI("Camera FPS", result, false, false);
            tracker->init(result, initBB);
        }
        else if (key == 'q')
        {
            break;
        }

        if (tracker->update(result, initBB))
        {
            rectangle(result, initBB, Scalar(0, 255, 0), 2);
        }
        else
        {
            putText(result, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
        }
#endif

#if WRITE_FPS
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        fps = 1.0 / t;
        sprintf_s(string, "%.2f", fps);
        std::string fpsString("FPS:");
        fpsString += string;
        // 将帧率信息写在输出帧上
        putText(result, fpsString, cv::Point(2, 50), cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(0, 255, 0), 2, 8);
#endif

        imshow("Camera FPS", result);

#if WRITE_VIDEO
        writerResult << result;
#endif

    }

    cout << "Success" << endl;
}

void CamVideoStitch::VideoStitch()
{
#if TRACK
    // 追踪模块
    Ptr<Tracker> tracker;

    tracker = TrackerCSRT::create();
    Rect2d initBB;
#endif

    char key;
    VideoCapture cap1(m_fileNameR);
    VideoCapture cap2(m_fileNameL);
    cout << cap1.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << cap1.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << cap1.get(CAP_PROP_FPS) << endl;

    if (!cap1.isOpened() || !cap2.isOpened())
    {
        cout << "摄像头打开失败！！！" << endl;
        cout << __LINE__ << endl;
        exit(EXIT_FAILURE);
    }

    Mat videoImage1;
    Mat videoImage2;

    cap1 >> videoImage1;
    cap2 >> videoImage2;
#if WRITE_VIDEO
    VideoWriter writerResult("result.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(MIN(rightTop.x, rightBottom.x), videoImage1.rows));
#endif

    double fps;
    char string[10];  // 帧率字符串
    cv::namedWindow("Camera FPS");
    double t = 0;
    int count = 0;
    Mat temp;
    bool flag = 0;

#if MOVE_DECT
    while (key = waitKey(20)) {
        cap1 >> videoImage1;
        cap2 >> videoImage2;
        ImageMix(videoImage1, videoImage2);
        imshow("ImageMix", result);
        if (key == 'w')
        {
            rectROI = selectROI("ImageMix", result, false, false);
            break;
        }
    }
    ResultROI = result(rectROI);
    destroyWindow("ImageMix");
#endif


    while (key = waitKey(20))
    {
        t = (double)cv::getTickCount();

        cap1 >> videoImage1;
        cap2 >> videoImage2;

#if DBG 	
        imshow("Camera Video 1", videoImage1); //显示视频内容的图片
        imshow("Camera Video 2", videoImage2); //显示视频内容的图片
#endif

        count++;

        if (videoImage1.empty() || videoImage2.empty()) {
            continue;
        }

#if MOVE_DECT
        cout << "result: rows" << result.rows << endl;
        cout << "result: cols" << result.cols << endl;

        cout << "rectROI: rows" << rectROI.height << endl;
        cout << "rectROI: cols" << rectROI.width << endl;

        cout << "Result: rows" << ResultROI.rows << endl;
        cout << "Result: cols" << ResultROI.cols << endl;

        if (count == 1)
        {
            flag = MoveDetect(ResultROI, ResultROI);
        }
        else
        {
            flag = MoveDetect(temp, ResultROI);
        }
        temp = ResultROI.clone();

        if (flag)
        {
            // 计算H矩阵
            FindHMatrix(videoImage1, videoImage2);
            cout << "单应矩阵计算成功" << endl;

            //计算四个角的坐标
            CalcFourCorner(videoImage1);
        }
#endif

        ImageMix(videoImage1, videoImage2);

#if TRACK
        if (key == 's')
        {
            if (!tracker->empty())
            {
                tracker.release();
                tracker = TrackerCSRT::create();
            }

            initBB = selectROI("Camera FPS", result, false, false);
            tracker->init(result, initBB);
        }
        else if (key == 'q')
        {
            break;
        }

        if (tracker->update(result, initBB))
        {
            rectangle(result, initBB, Scalar(0, 255, 0), 2);
        }
        else
        {
            putText(result, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
        }
#endif

#if WRITE_FPS
        t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
        fps = 1.0 / t;
        sprintf_s(string, "%.2f", fps);
        std::string fpsString("FPS:");
        fpsString += string;
        // 将帧率信息写在输出帧上
        putText(result, fpsString, cv::Point(2, 50), cv::FONT_HERSHEY_DUPLEX, 2, cv::Scalar(0, 255, 0), 2, 8);
#endif

        imshow("Camera FPS", result);

#if WRITE_VIDEO
        writerResult << result;
#endif

    }

    cout << "Success" << endl;
   
}

Mat CamVideoStitch::ImageMix(Mat image1, Mat image2)
{
    int start = fabs(MIN(leftTop.x, leftBottom.x));//开始位置，即重叠区域的左边界
    double processWidth = image2.cols - start;//重叠区域的宽度
    double alpha = 1;//img2中像素的权重

    // 对右图进行透视变换
    //warpPerspective(image1, result, H, cv::Size(MIN(rightTop.x, rightBottom.x), image1.rows));
    warpPerspective(image1, result, H, cv::Size(1280, 480));
#if DBG
    imshow("Result0", result);
#endif

    //  将左图拷贝到右图上去
    Mat image2Roi(image2, cv::Rect(0, 0, min(fabs(leftTop.x), fabs(leftBottom.x)), image2.rows));
    Mat resultRoi(result, cv::Rect(0, 0, min(fabs(leftTop.x), fabs(leftBottom.x)), result.rows));
    image2Roi.copyTo(resultRoi);

#if DBG
    imshow("obj and scene", result);
#endif

    // 去掉拼接缝，加权平均法
    for (int i = 0; i < result.rows; i++)//遍历行
    {
        uchar* image2P = image2.ptr<uchar>(i);
        uchar* resultP = result.ptr<uchar>(i);
        for (int j = start; j < image2.cols; j++)//遍历重叠区域的列
        {
            if (resultP[j * 3] < 50 && resultP[j * 3 + 1] < 50 && resultP[j * 3 + 2] < 50)
            {
                alpha = 1;
            }
            else
            {   //img1中像素的权重，与当前处理点距重叠区域左边界的距离成正比
                alpha = (processWidth - (j - start)) / processWidth;
            }
            resultP[j * 3] = image2P[j * 3] * alpha + resultP[j * 3] * (1 - alpha);//B通道
            resultP[j * 3 + 1] = image2P[j * 3 + 1] * alpha + resultP[j * 3 + 1] * (1 - alpha);//G通道
            resultP[j * 3 + 2] = image2P[j * 3 + 2] * alpha + resultP[j * 3 + 2] * (1 - alpha);//R通道
        }
    }
#if DBG
    rectangle(result, Rect(min(fabs(leftTop.x), fabs(leftBottom.x)), 0, image2.cols - MAX(fabs(leftTop.x), fabs(leftBottom.x)), result.rows), Scalar(0, 0, 255), 2);
#endif   
    return result;
}

bool CamVideoStitch::MoveDetect(Mat frame1, Mat frame2)
{
    Mat gray1, gray2;
    cvtColor(frame1, gray1, CV_BGR2GRAY);
    cvtColor(frame2, gray2, CV_BGR2GRAY);

    threshold(gray1, gray1, 45, 255, CV_THRESH_BINARY);
    threshold(gray2, gray2, 45, 255, CV_THRESH_BINARY);

    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));

#if DBG
    imshow("gray1", gray1);
    imshow("gray2", gray2);
#endif
    CalcTwoHist(gray1, gray2);
    double matchValue = compareHist(dHist1, dHist2, HISTCMP_CORREL);  //值越大相似度越高
    cout << matchValue << endl;
    return matchValue <= 0.99991;

}

void CamVideoStitch::CalcTwoHist(Mat frame1, Mat frame2)
{
    int dims = 1;  //需要统计的特征数目(只统计灰度值)
    float hranges[] = { 0, 256 };  //范围[0,256)注意是最大值加1
    const float* ranges[] = { hranges };
    int bins = 256;
    int channels = 0;
    //计算直方图
    calcHist(&frame1, 1, &channels, Mat(), dHist1, dims, &bins, ranges);
    calcHist(&frame2, 1, &channels, Mat(), dHist2, dims, &bins, ranges);
}