#pragma once

#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui_c.h>
using namespace cv;

// CamLoadView 对话框

class CamLoadView : public CDialog
{
	DECLARE_DYNAMIC(CamLoadView)

public:
	CamLoadView(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CamLoadView();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMLOADVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBucamfuse();
private:
	VideoCapture cap;
	Mat frame;
	CRect rect;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
};
