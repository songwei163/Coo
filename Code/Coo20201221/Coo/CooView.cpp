// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// CooView.cpp: CCooView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Coo.h"
#endif

#include "CooDoc.h"
#include "CooView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MainFrm.h"


// CCooView

IMPLEMENT_DYNCREATE(CCooView, CFormView)

BEGIN_MESSAGE_MAP(CCooView, CFormView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCooView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_BTSETCAM, &CCooView::OnBtsetcam)
	ON_COMMAND(ID_BTSTART, &CCooView::OnBtstart)
	ON_COMMAND(ID_BTPROCESS, &CCooView::OnBtprocess)
	ON_WM_TIMER()
	ON_COMMAND(ID_BTVIDEOLOAD, &CCooView::OnBtvideoload)
	ON_COMMAND(ID_BTVIDEOPROCESS, &CCooView::OnBtvideoprocess)
END_MESSAGE_MAP()

// CCooView 构造/析构

CCooView::CCooView() noexcept
	: CFormView(IDD_COO_FORM)
{
	// TODO: 在此处添加构造代码
	strVideoPath1 = "";
	strVideoPath2 = "";

}

CCooView::~CCooView()
{
}

void CCooView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CCooView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CCooView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	this->vdlg.m_father = this;

	CWnd* pWnd1 = GetDlgItem(IDC_CAM1);
	pWnd1->GetClientRect(&rect1);
	namedWindow("video1", WINDOW_AUTOSIZE);
	HWND hWnd1 = (HWND)cvGetWindowHandle("video1");
	HWND hParent1 = ::GetParent(hWnd1);
	::SetParent(hWnd1, GetDlgItem(IDC_CAM1)->m_hWnd);
	::ShowWindow(hParent1, SW_HIDE);;

	CWnd* pWnd2 = GetDlgItem(IDC_CAM2);
	pWnd1->GetClientRect(&rect2);
	namedWindow("video2", WINDOW_AUTOSIZE);
	HWND hWnd2 = (HWND)cvGetWindowHandle("video2");
	HWND hParent2 = ::GetParent(hWnd2);
	::SetParent(hWnd2, GetDlgItem(IDC_CAM2)->m_hWnd);
	::ShowWindow(hParent2, SW_HIDE);;
}


// CCooView 打印


void CCooView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCooView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCooView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCooView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CCooView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 在此处添加自定义打印代码
}

void CCooView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCooView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCooView 诊断

#ifdef _DEBUG
void CCooView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCooView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CCooDoc* CCooView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCooDoc)));
	return (CCooDoc*)m_pDocument;
}
#endif //_DEBUG


// CCooView 消息处理程序


void CCooView::OnBtsetcam()
{
	// TODO: 在此添加命令处理程序代码
}


void CCooView::OnBtstart()
{
	// TODO: 在此添加命令处理程序代码
	cap1.open(0);
	cap2.open(1);
	if (cap1.isOpened() && cap2.isOpened())
	{
		SetTimer(1, 20, nullptr);
		SetTimer(2, 20, nullptr);
	}
	else
	{
		MessageBox(_T("摄像头打开失败，程序退出"), _T("错误"), MB_ICONERROR);
		exit(-1);
	}
}


void CCooView::OnBtprocess()
{
	// TODO: 在此添加命令处理程序代码
	CamVideoStitch d;
	//CMainFrame* p_Main = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	d.CalcHMatrix();
	d.CameraStitch();
}


void CCooView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		cap1 >> frame1;
		if (frame1.empty())
			KillTimer(1);
		else
		{
			resize(frame1, frame1, Size(rect1.Width(), rect1.Height()));
			imshow("video1", frame1);
		}
	}

	if (nIDEvent == 2)
	{
		cap2 >> frame2;
		if (frame2.empty())
			KillTimer(2);
		else
		{
			resize(frame2, frame2, Size(rect2.Width(), rect2.Height()));
			imshow("video2", frame2);
		}
	}


	CFormView::OnTimer(nIDEvent);
}


void CCooView::OnBtvideoload()
{
	// TODO: 在此添加命令处理程序代码
	vdlg.DoModal();
	strVideoPath1 = vdlg.strVideoPath1;
	strVideoPath2 = vdlg.strVideoPath2;
}


void CCooView::OnBtvideoprocess()
{
	// TODO: 在此添加命令处理程序代码
	CamVideoStitch d;
	CMainFrame* p_Main = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (strVideoPath1.IsEmpty() || strVideoPath2.IsEmpty())
	{
		MessageBox(_T("文件打开失败，程序退出"),_T("错误"), MB_ICONERROR);
		exit(EXIT_FAILURE);
	}
	d.CalcHMatrix(CStringToString(strVideoPath1), CStringToString(strVideoPath2));
	d.VideoStitch();
}

string CCooView::CStringToString(CString strMFC)
{
	CStringA strA;
	strA = strMFC.GetBuffer(0);
	strMFC.ReleaseBuffer();

	string str;
	str = strA.GetBuffer(0);
	strA.ReleaseBuffer();

	return str;
}
