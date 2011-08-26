/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file      CANDBConverter.cpp
 * \brief     Implementation file for the CCANDBConverterApp class.
 * \author    RBIN/EBS1 - Mahesh B S
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * Implementation file for the CCANDBConverterApp class.
 */
/**
* \file       CANDBConverter.cpp
* \brief      Implementation file for the CCANDBConverterApp class.
* \authors    RBIN/EBS1 - Mahesh B S
* \date       4.11.2004
* \copyright  Copyright &copy; 2011, Robert Bosch Engineering and Business Solutions.  All rights reserved.
*/

#include "stdafx.h"
#include "CANDBConverter.h"
#include "CANDBConverterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define APPLICATION_STARTUP_KEY _T("12B3BCDD-2EE1-4640-86C5-7C4340442D3C")
/////////////////////////////////////////////////////////////////////////////
// CCANDBConverterApp

BEGIN_MESSAGE_MAP(CCANDBConverterApp, CWinApp)
    //{{AFX_MSG_MAP(CCANDBConverterApp)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/**
* \brief      CCANDBConverterApp Converter
* \param[in]     
* \return     void
* \authors    
* \date       
*/
CCANDBConverterApp::CCANDBConverterApp()
{
    // Place all significant initialization in InitInstance	
}

CCANDBConverterApp theApp;

/**
* \brief      InitInstance function called when the application is stareted
The function checks for the licence key from the command line
parameters and if success application will open
* \param[in]  None   
* \return     BOOL - FALSE to  close the application.
TRUE to start the application   
* \authors    
* \date       
*/

BOOL CCANDBConverterApp::InitInstance()
{
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();			// Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
    CString check = GetCommandLine();

    if (check.Find(APPLICATION_STARTUP_KEY) == -1)
    {
        return FALSE;
    }

    CCANDBConverterDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

