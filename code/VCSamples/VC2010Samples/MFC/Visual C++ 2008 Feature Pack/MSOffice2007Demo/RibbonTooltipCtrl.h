﻿// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CRibbonTooltipCtrl window

class CRibbonTooltipCtrl : public CMFCToolTipCtrl
{
    DECLARE_DYNCREATE(CRibbonTooltipCtrl)

// Construction
public:
    CRibbonTooltipCtrl();

// Attributes
protected:
    CMFCRibbonPanelMenuBar* m_pParentMenuBar;
    CMFCRibbonBar* m_pParentRibbon;
    UINT m_nID;

// Overrides
public:
    virtual CSize GetIconSize();
    virtual BOOL OnDrawIcon(CDC* pDC, CRect rectImage);

    // Implementation
public:
    virtual ~CRibbonTooltipCtrl();

    // Generated message map functions
protected:
    //{{AFX_MSG(CRibbonTooltipCtrl)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnShow(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

