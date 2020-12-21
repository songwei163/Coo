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

// CooView.h: CCooView 类的接口
//

#pragma once
#include "VideoLoad.h"
#include "CamVideoStitch.h"
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include "CooDoc.h"

using namespace cv;
using namespace std;

class CCooView : public CFormView
{
protected: // 仅从序列化创建
	CCooView() noexcept;
	DECLARE_DYNCREATE(CCooView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_COO_FORM };
#endif

// 特性
public:
	CCooDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCooView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	VideoCapture cap1;	
	VideoCapture cap2;
private:
	afx_msg void OnBtsetcam();
	afx_msg void OnBtstart();
	afx_msg void OnBtprocess();


	Mat frame1;
	CRect rect1;
	CString StrPath1;


	Mat frame2;
	CRect rect2;

	VideoLoad vdlg;
	CString strVideoPath1;
	CString strVideoPath2;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBtvideoload();
public:
	afx_msg void OnBtvideoprocess();
	string CStringToString(CString strMFC);
};

#ifndef _DEBUG  // CooView.cpp 中的调试版本
inline CCooDoc* CCooView::GetDocument() const
   { return reinterpret_cast<CCooDoc*>(m_pDocument); }
#endif

