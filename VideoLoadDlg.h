#pragma once


// VideoLoadDlg 对话框

class VideoLoadDlg : public CDialog
{
	DECLARE_DYNAMIC(VideoLoadDlg)

public:
	VideoLoadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~VideoLoadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ViDEOLOADDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBuselvideo1();
	afx_msg void OnBnClickedBuselvideo2();
private:
	// 载入本地视频一
	CString strVideoPath1;
	// 载入本地视频二
	CString strVideoPath2;
	CEdit m_logout;
public:
	afx_msg void OnBnClickedBufuse();
};
