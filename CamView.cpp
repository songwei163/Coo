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

// CamView.cpp: CCamView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Cam.h"
#endif

#include "CamDoc.h"
#include "CamView.h"


#include "CamLoadView.h"
#include "VideoLoadDlg.h"
#include "VideoLoadDlg2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CCamView

IMPLEMENT_DYNCREATE(CCamView, CFormView)

BEGIN_MESSAGE_MAP(CCamView, CFormView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCamView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CCamView::OnTvnSelchangedTree1)
	ON_COMMAND(ID_BUTESTFUSE1, &CCamView::OnButestfuse1)
	ON_COMMAND(ID_BUTESTFUSE2, &CCamView::OnButestfuse2)
	ON_COMMAND(ID_BUTESTFUSE3, &CCamView::OnButestfuse3)
END_MESSAGE_MAP()



// CCamView 构造/析构

CCamView::CCamView() noexcept
	: CFormView(IDD_CAM_FORM)
{
	// TODO: 在此处添加构造代码

}

CCamView::~CCamView()
{
}

void CCamView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_treeCtrl);
}

BOOL CCamView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CFormView::PreCreateWindow(cs);
}

void CCamView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// TODO: 在此添加专用代码和/

	HICON icon[3];
	icon[0] = AfxGetApp()->LoadIconW(IDI_ICON1);
	icon[1] = AfxGetApp()->LoadIconW(IDI_ICON2);
	icon[2] = AfxGetApp()->LoadIconW(IDI_ICON3);
	// icon[3] = AfxGetApp()->LoadIconW(IDI_ICON4);

	//CImageList list; 已经放在.h中 做成员属性了
	//准备好list图标集合
	m_imageList.Create(30, 30, ILC_COLOR32, 3, 3);
	//加载图片
	for (int i = 0; i < 3; i++)
	{
		m_imageList.Add(icon[i]);
	}
	
	m_treeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);

	// 2 设置节点 参数   节点名称   默认显示图片  选中后显示图片  父节点
	HTREEITEM rootItem = m_treeCtrl.InsertItem(TEXT("摄像机组"), 0, 0, nullptr);

	HTREEITEM parentItem = m_treeCtrl.InsertItem(TEXT("摄像机组（左）"), 1, 1, rootItem);
	m_treeCtrl.InsertItem(TEXT("帧宽度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧高度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧率"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("总帧数"), 2, 2, parentItem);

	parentItem = m_treeCtrl.InsertItem(TEXT("摄像机组（右）"), 1, 1, rootItem);
	m_treeCtrl.InsertItem(TEXT("帧宽度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧高度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧率"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("总帧数"), 2, 2, parentItem);

	parentItem = m_treeCtrl.InsertItem(TEXT("摄像机组（前）"), 1, 1, rootItem);
	m_treeCtrl.InsertItem(TEXT("帧宽度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧高度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧率"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("总帧数"), 2, 2, parentItem);

	parentItem = m_treeCtrl.InsertItem(TEXT("摄像机组（后）"), 1, 1, rootItem);
	m_treeCtrl.InsertItem(TEXT("帧宽度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧高度"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("帧率"), 2, 2, parentItem);
	m_treeCtrl.InsertItem(TEXT("总帧数"), 2, 2, parentItem);
}


// CCamView 打印


void CCamView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCamView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCamView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCamView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CCamView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 在此处添加自定义打印代码
}

void CCamView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCamView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCamView 诊断

#ifdef _DEBUG
void CCamView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCamView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CCamDoc* CCamView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCamDoc)));
	return (CCamDoc*)m_pDocument;
}
#endif //_DEBUG


// CCamView 消息处理程序


void CCamView::OnTvnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CCamView::OnButestfuse1()
{
	// TODO: 在此添加命令处理程序代码
	VideoLoadDlg dlg;
	dlg.DoModal();
}


void CCamView::OnButestfuse2()
{
	// TODO: 在此添加命令处理程序代码
	VideoLoadDlg2 dlg;
	dlg.DoModal();
}


void CCamView::OnButestfuse3()
{
	// TODO: 在此添加命令处理程序代码
	CamLoadView dlg;
	dlg.DoModal();
}
