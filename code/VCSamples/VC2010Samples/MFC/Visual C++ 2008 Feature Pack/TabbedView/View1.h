﻿// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CView1 view

class CView1 : public CScrollView
{
protected:
    CView1();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CView1)

// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual void OnInitialUpdate();
    virtual CScrollBar* GetScrollBarCtrl(int nBar) const;
protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view

// Implementation
protected:
    virtual ~CView1();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()
};
