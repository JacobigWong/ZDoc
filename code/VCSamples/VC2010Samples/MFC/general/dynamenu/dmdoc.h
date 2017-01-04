﻿// dmdoc.h
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//
// Purpose: interface of the CDynaMenuDoc class
//
// Functions:
//   Most of this file was generated by AppWizard.  The functions
//   which contain code specific to this sample are:
//
//      CDynaMenuDoc::CDynaMenuDoc()         -- document constructor
//      CDynaMenuDoc::DoSelectColor()        -- handle text color commands
//      CDynaMenuDoc::DoUpdateSelectColor()  -- update text color UI objects
//      CDynaMenuDoc::GetMessageString()     -- get status msg for color cmds
//      CDynaMenuDoc::OnCmdMsg()             -- perform command routing
//      CDynaMenuDoc::OnColorOptions()       -- display color options dialog
//
//   These functions are implemented in dmdoc.cpp.

// This is the number of possible text colors.  It should match the
// number of checkboxes which appear in the color options dialog.
const int NUM_TEXTCOLOR = 4;

// This is the maximum length of a color name.
const int MAX_COLORLEN = 20;

// This structure defines the relationship between a color name,
// menu ID, and COLORREF value.  By defining this array, we don't
// depend on the menu IDs being in any particular order, or being
// sequential.
struct DynaMenuDef
{
    UINT     m_nString;                  // color name (in string table)
    UINT     m_nID;                      // menu ID
    COLORREF m_crf;                      // color value
};

/////////////////////////////////////////////////////////////////////////////
// CDynaMenuDoc

class CDynaMenuDoc : public CDocument
{
protected: // create from serialization only
    CDynaMenuDoc();
    DECLARE_DYNCREATE(CDynaMenuDoc)

// Attributes
public:
    int  m_iColor;                      // current text color
    BOOL m_abAllowColor[NUM_TEXTCOLOR]; // flags whether colors may be used

// This array defines the possible text colors.  There is a one-to-one
// correspondence with m_abAllowColor. m_iColor is an index into both arrays.
    static DynaMenuDef m_aColorDef[NUM_TEXTCOLOR];

// Operations
public:
    static BOOL GetMessageString(UINT nID, CString& strMessage);

// Implementation
public:
    virtual ~CDynaMenuDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o

    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
                          AFX_CMDHANDLERINFO* pHandlerInfo);

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual BOOL OnNewDocument();

    void DoSelectColor(int iColor);
    void DoUpdateSelectColor(int iColor, CCmdUI* pExtra);

// Generated message map functions
protected:
    //{{AFX_MSG(CDynaMenuDoc)
    afx_msg void OnColorOptions();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
