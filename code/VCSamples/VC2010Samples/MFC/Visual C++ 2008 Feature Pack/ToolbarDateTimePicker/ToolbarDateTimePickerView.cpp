﻿// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "ToolbarDateTimePicker.h"

#include "ToolbarDateTimePickerDoc.h"
#include "ToolbarDateTimePickerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const CString strInfo =
    _T("This sample illustrates how to use a date/time picker integrated with the toolbar control.\r\n\r\n")
    _T("The project was generated by Application Wizard\r\n")
    _T("Please note //----Example----// comments in the example source code\r\n");

/////////////////////////////////////////////////////////////////////////////
// CToolbarDateTimePickerView

IMPLEMENT_DYNCREATE(CToolbarDateTimePickerView, CView)

BEGIN_MESSAGE_MAP(CToolbarDateTimePickerView, CView)
    //{{AFX_MSG_MAP(CToolbarDateTimePickerView)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    ON_WM_CONTEXTMENU()
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarDateTimePickerView construction/destruction

CToolbarDateTimePickerView::CToolbarDateTimePickerView()
{
    // TODO: add construction code here

}

CToolbarDateTimePickerView::~CToolbarDateTimePickerView()
{
}

BOOL CToolbarDateTimePickerView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CToolbarDateTimePickerView drawing

void CToolbarDateTimePickerView::OnDraw(CDC* pDC)
{
    CToolbarDateTimePickerDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    const int iOffset = 20;

    CFont* pFontOld = (CFont*) pDC->SelectStockObject (DEFAULT_GUI_FONT);
    ASSERT (pFontOld != NULL);

    CRect rectClient;
    GetClientRect (&rectClient);

    CRect rectText = rectClient;
    rectText.DeflateRect (iOffset, iOffset);
    pDC->DrawText (strInfo, rectText, DT_CALCRECT | DT_WORDBREAK);

    rectText.OffsetRect (	(rectClient.Width () - rectText.Width () - 2 * iOffset) / 2,
                            (rectClient.Height () - rectText.Height () - 2 * iOffset) / 2);

    CRect rectFrame = rectText;
    rectFrame.InflateRect (iOffset, iOffset);

    pDC->FillSolidRect (rectFrame, ::GetSysColor (COLOR_INFOBK));

    rectFrame.DeflateRect (1, 1);
    pDC->Draw3dRect (rectFrame, ::GetSysColor (COLOR_3DSHADOW),
                     ::GetSysColor (COLOR_3DLIGHT));

    rectFrame.DeflateRect (2, 2);
    pDC->Draw3dRect (rectFrame, ::GetSysColor (COLOR_3DSHADOW),
                     ::GetSysColor (COLOR_3DLIGHT));

    pDC->SetTextColor (::GetSysColor (COLOR_INFOTEXT));
    pDC->SetBkMode (TRANSPARENT);

    pDC->DrawText (strInfo, rectText, DT_WORDBREAK);

    pDC->SelectObject (pFontOld);
}

/////////////////////////////////////////////////////////////////////////////
// CToolbarDateTimePickerView printing

void CToolbarDateTimePickerView::OnFilePrintPreview()
{
    AFXPrintPreview (this);
}

BOOL CToolbarDateTimePickerView::OnPreparePrinting(CPrintInfo* pInfo)
{
    // default preparation
    return DoPreparePrinting(pInfo);
}

void CToolbarDateTimePickerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add extra initialization before printing
}

void CToolbarDateTimePickerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
    // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CToolbarDateTimePickerView diagnostics

#ifdef _DEBUG
void CToolbarDateTimePickerView::AssertValid() const
{
    CView::AssertValid();
}

void CToolbarDateTimePickerView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CToolbarDateTimePickerDoc* CToolbarDateTimePickerView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolbarDateTimePickerDoc)));
    return (CToolbarDateTimePickerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CToolbarDateTimePickerView message handlers

void CToolbarDateTimePickerView::OnContextMenu(CWnd*, CPoint point)
{
    theApp.ShowPopupMenu (IDR_CONTEXT_MENU, point, this);
}
