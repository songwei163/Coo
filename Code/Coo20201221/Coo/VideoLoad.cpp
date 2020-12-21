// VideoLoad.cpp: 实现文件
//

#include "pch.h"
#include "Coo.h"
#include "VideoLoad.h"
#include "afxdialogex.h"
#include "CooView.h"
#include "MainFrm.h"


// VideoLoad 对话框

IMPLEMENT_DYNAMIC(VideoLoad, CDialog)

VideoLoad::VideoLoad(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_VIDEOLOAD, pParent)
{
    strVideoPath1 = "";
    strVideoPath2 = "";
}

VideoLoad::~VideoLoad()
{
}

void VideoLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGOUTPUT, m_logout);
}


BEGIN_MESSAGE_MAP(VideoLoad, CDialog)
	ON_BN_CLICKED(IDC_BUSELVIDEO1, &VideoLoad::OnBnClickedBuselvideo1)
	ON_BN_CLICKED(IDC_BUSELVIDEO2, &VideoLoad::OnBnClickedBuselvideo2)
    ON_BN_CLICKED(IDOK, &VideoLoad::OnBnClickedOk)
END_MESSAGE_MAP()


// VideoLoad 消息处理程序


void VideoLoad::OnBnClickedBuselvideo1()
{
	// TODO: 在此添加控件通知处理程序代码
    TCHAR szFilter[] = _T("视频文件(*.avi)|*.avi|所有文件(*.*)|*.*||");
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("avi"), nullptr, 0, szFilter, this);

    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())
    {
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strVideoPath1 = fileDlg.GetPathName();
        SetDlgItemText(IDC_VIDEOLOAD1, strVideoPath1);
        // 打印日志
        CString str;
        m_logout.GetWindowText(str);
        str.Format(TEXT("%s已加载本地视频一：\r\n%s\r\n"), str, strVideoPath1);
        m_logout.SetWindowText(str);
        m_logout.LineScroll(m_logout.GetLineCount());
    }
}


void VideoLoad::OnBnClickedBuselvideo2()
{
	// TODO: 在此添加控件通知处理程序代码
    TCHAR szFilter[] = _T("视频文件(*.avi)|*.avi|所有文件(*.*)|*.*||");
    // 构造打开文件对话框   
    CFileDialog fileDlg(TRUE, _T("avi"), nullptr, 0, szFilter, this);

    // 显示打开文件对话框   
    if (IDOK == fileDlg.DoModal())
    {
        // 如果点击了文件对话框上的“打开”按钮，则将选择的文件路径显示到编辑框里   
        strVideoPath2 = fileDlg.GetPathName();
        SetDlgItemText(IDC_VIDEOLOAD2, strVideoPath2);
        // 打印日志
        CString str;
        m_logout.GetWindowText(str);
        str.Format(TEXT("%s已加载本地视频二：\r\n%s\r\n"), str, strVideoPath2);
        m_logout.SetWindowText(str);
        m_logout.LineScroll(m_logout.GetLineCount());
    }
}

void VideoLoad::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    CMainFrame* p_Main = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    this->m_father->cap1.open(this->m_father->CStringToString(strVideoPath1));
    this->m_father->cap2.open(this->m_father->CStringToString(strVideoPath2));

    if (this->m_father->cap1.isOpened() && this->m_father->cap2.isOpened())
    {
        this->m_father->SetTimer(1, 20, nullptr);
        this->m_father->SetTimer(2, 20, nullptr);

        p_Main->m_wndOutput.DebugOutPut(_T("定时器设置成功"));
        p_Main->m_wndOutput.SendMessage(WM_VSCROLL, SB_BOTTOM);
    }
    else
    {
        p_Main->m_wndOutput.DebugOutPut(_T("打开视频失败"));
        p_Main->m_wndOutput.SendMessage(WM_VSCROLL, SB_BOTTOM);
        exit(-1);
    }

    CDialog::OnOK();
}

