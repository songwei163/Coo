// CamLoadView.cpp: 实现文件
//

#include "pch.h"
#include "Cam.h"
#include "CamLoadView.h"
#include "afxdialogex.h"

// CamLoadView 对话框

class CMainFrame;
IMPLEMENT_DYNAMIC(CamLoadView, CDialog)

CamLoadView::CamLoadView(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CAMLOADVIEW, pParent)
{

}

CamLoadView::~CamLoadView()
{
}

void CamLoadView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CamLoadView, CDialog)
	ON_BN_CLICKED(IDC_BUCAMFUSE, &CamLoadView::OnBnClickedBucamfuse)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CamLoadView 消息处理程序


void CamLoadView::OnBnClickedBucamfuse()
{
	// TODO: 在此添加控件通知处理程序代码
	cap.open(0);

	if (cap.isOpened())
	{
		SetTimer(1, 30, nullptr);
	}
	else
	{
		MessageBox(_T("打开视频失败！"));
	}
}


void CamLoadView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		cap >> frame;
		if (frame.empty())
		{
			KillTimer(1);
		}
		else
		{
			resize(frame, frame, Size(rect.Width(), rect.Height()));
			imshow("video", frame);
		}
	}

	
	CDialog::OnTimer(nIDEvent);
}


BOOL CamLoadView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	// CMainFrame* pAppFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	// ASSERT_KINDOF(CMainFrame, pAppFrame);
	// CWnd* pWnd1 = pAppFrame->GetDlgItem(IDC_VIDEO5);
	// pWnd1->GetClientRect(&rect);
	// namedWindow("video", WINDOW_AUTOSIZE);
	// HWND hWnd1 = (HWND)cvGetWindowHandle("video");
	// HWND hParent1 = ::GetParent(hWnd1);
	// ::SetParent(hWnd1, GetDlgItem(IDC_VIDEO5)->m_hWnd);
	// ::ShowWindow(hParent1, SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
