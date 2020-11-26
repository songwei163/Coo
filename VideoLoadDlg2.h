#pragma once


// VideoLoadDlg2 对话框

class VideoLoadDlg2 : public CDialog
{
	DECLARE_DYNAMIC(VideoLoadDlg2)

public:
	VideoLoadDlg2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~VideoLoadDlg2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIDEOLOADDLG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBuselvideo21();
	afx_msg void OnBnClickedBuselvideo22();
private:
	// 载入本地视频一
	CString strVideoPath1;
	// 载入本地视频二
	CString strVideoPath2;
	CEdit m_logout;
public:
	afx_msg void OnBnClickedBufuse();
	afx_msg void OnBnClickedBufuse2();
};
