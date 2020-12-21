#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>

using namespace cv;

// VideoLoad 对话框

class CCooView;

class VideoLoad : public CDialog
{
	DECLARE_DYNAMIC(VideoLoad)

public:
	VideoLoad(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~VideoLoad();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEOLOAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBuselvideo1();
	afx_msg void OnBnClickedBuselvideo2();
	CString strVideoPath1;
	CString strVideoPath2;
	CEdit m_logout;
	CCooView* m_father;
	afx_msg void OnBnClickedOk();
};
