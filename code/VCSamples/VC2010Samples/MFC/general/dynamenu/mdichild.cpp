﻿// mdichild.cpp
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
// Purpose: implementation of the CDynaMDIChildWnd class
//
// Functions:
//   Most of this file was generated by AppWizard.  The functions
//   which contain code specific to this sample are:
//
//      CDynaMDIChildWnd::OnMDIActivate()    -- handle activation/deactivation
//      CDynaMDIChildWnd::RefreshColorMenu() -- update Color submenu contents

#include "stdafx.h"
#include "dynamenu.h"
#include "mdichild.h"
#include "dmdoc.h"      // for color menu definitions

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDynaMDIChildWnd

IMPLEMENT_DYNCREATE(CDynaMDIChildWnd, CMDIChildWnd)

CDynaMDIChildWnd::CDynaMDIChildWnd()
{
}

CDynaMDIChildWnd::~CDynaMDIChildWnd()
{
}


BEGIN_MESSAGE_MAP(CDynaMDIChildWnd, CMDIChildWnd)
    //{{AFX_MSG_MAP(CDynaMDIChildWnd)
    ON_WM_MDIACTIVATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDynaMDIChildWnd message handlers


//***********************************************************************
// Function: CDynaMDIChildWnd::OnMDIActivate()
//
// Purpose:
//    OnMDIActivate is called for the MDI child window being
//    deactivated and the child window being activated.
//
//    We override this function so that we can update the Color submenu
//    when a child window is activated.  A call to RefreshColorMenu()
//    performs the actual update.
//
// Parameters:
//    bActivate      -- TRUE if child window being activated, otherwise
//                      FALSE
//    pActivateWnd   -- pointer to MDI child window being activated
//    pDeactivateWnd -- pointer to MDI child window being deactivated
//
// Returns:
//    none
//
// Comments:
//    see the CWnd::OnMDIActivate() docs for further information.
//
//***********************************************************************
void CDynaMDIChildWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
    // call the base class to let standard processing switch to
    // the top-level menu associated with this window
    CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

    // do our modifications, if we are being activated
    if (bActivate)
        RefreshColorMenu();
}

//***********************************************************************
// Function: CDynaMDIChildWnd::RefreshColorMenu()
//
// Purpose:
//    RefreshColorMenu updates the Color submenu so that it only
//    contains the text colors which are enabled for the active document.
//
//    The Color submenu is located by searching for a submenu with
//    ID_COLOR_OPTIONS as its first menu item.
//
// Parameters:
//    none
//
// Returns:
//    none
//
// Comments:
//    none
//
//***********************************************************************
void CDynaMDIChildWnd::RefreshColorMenu()
{
    // Get the active document
    CDynaMenuDoc* pDoc = (CDynaMenuDoc*)GetActiveDocument();
    ASSERT_KINDOF(CDynaMenuDoc, pDoc);

    // Locate the color submenu
    CMenu* pColorMenu = NULL;
    CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
    int iPos;
    for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
    {
        CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
        if (pMenu && pMenu->GetMenuItemID(0) == ID_COLOR_OPTIONS)
        {
            pColorMenu = pMenu;
            break;
        }
    }
    ENSURE(pColorMenu != NULL);

    // Update the color submenu to reflect the text colors available to
    // the active document

    // First, delete all items but ID_COLOR_OPTIONS at position 0
    for (iPos = pColorMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
        pColorMenu->DeleteMenu(iPos, MF_BYPOSITION);

    // Then, add a separator and an item for each available text color
    BOOL bNeedSeparator = TRUE;
    for (int i = 0; i < NUM_TEXTCOLOR; i++)
    {
        if (pDoc->m_abAllowColor[i] == TRUE)
        {
            if (bNeedSeparator)
            {
                pColorMenu->AppendMenu(MF_SEPARATOR);
                bNeedSeparator = FALSE;
            }
            CString strColor;
            strColor.LoadString(pDoc->m_aColorDef[i].m_nString);
            pColorMenu->AppendMenu(MF_STRING,
                                   pDoc->m_aColorDef[i].m_nID, strColor);
        }
    }
}
