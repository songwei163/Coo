// VideoLoadDlg.cpp: 实现文件
//

#include "pch.h"
#include "Cam.h"
#include "VideoLoadDlg.h"
#include "afxdialogex.h"


// VideoLoadDlg 对话框

IMPLEMENT_DYNAMIC(VideoLoadDlg, CDialog)

VideoLoadDlg::VideoLoadDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ViDEOLOADDLG, pParent)
{

}

VideoLoadDlg::~VideoLoadDlg()
{
}

void VideoLoadDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOGOUTPUT, m_logout);
}


BEGIN_MESSAGE_MAP(VideoLoadDlg, CDialog)
	ON_BN_CLICKED(IDC_BUSELVIDEO1, &VideoLoadDlg::OnBnClickedBuselvideo1)
    ON_BN_CLICKED(IDC_BUSELVIDEO2, &VideoLoadDlg::OnBnClickedBuselvideo2)
    ON_BN_CLICKED(IDC_BUFUSE, &VideoLoadDlg::OnBnClickedBufuse)
END_MESSAGE_MAP()


// VideoLoadDlg 消息处理程序


void VideoLoadDlg::OnBnClickedBuselvideo1()
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


void VideoLoadDlg::OnBnClickedBuselvideo2()
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


void VideoLoadDlg::OnBnClickedBufuse()
{
    // TODO: 在此添加控件通知处理程序代码

	// 如果两个文件都没有读入，则退出程序
    if(strVideoPath1.IsEmpty() || strVideoPath2.IsEmpty())
    {
        CString str;
        m_logout.GetWindowText(str);
        str.Format(TEXT("%s请载入本地视频一和本地视频二...\r\n"), str);
        m_logout.SetWindowText(str);
        m_logout.LineScroll(m_logout.GetLineCount());
        return;
    }
	
    // 打印日志
    CString str;
    m_logout.GetWindowText(str);
    str.Format(TEXT("%s开始视频融合，请稍等...\r\n"), str);
    m_logout.SetWindowText(str);
    m_logout.LineScroll(m_logout.GetLineCount());

	//
    // 视频融合代码放置处
	//
	
    // 打印日志
    // CString str;
    m_logout.GetWindowText(str);
    str.Format(TEXT("%s该接口暂未开放使用，请退出...\r\n"), str);
    m_logout.SetWindowText(str);
    m_logout.LineScroll(m_logout.GetLineCount());
}
