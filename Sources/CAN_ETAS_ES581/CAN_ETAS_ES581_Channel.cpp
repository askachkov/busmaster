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
 * \file      CAN_ETAS_ES581_Channel.cpp
 * \brief     Implementation file for CChannel class
 * \author    Pradeep Kadoor
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * Implementation file for CChannel class
 */

#include "CAN_ETAS_ES581_stdafx.h"
#include "CAN_ETAS_ES581_Channel.h" // For CChannel class declaration
#include "Include/CanUsbDefs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
  Function Name  : CChannel
  Description    : Standard default constructor
  Member of      : CChannel
  Functionality  : This will initialise local variables
  Author(s)      : Raja N
  Date Created   : 21.2.2005
  Modifications  :
*******************************************************************************/
CChannel::CChannel()
{
    // Init members with default value
    // Baud Rate
    m_usBaudRate = defBAUD_RATE;
    // Controller state
    m_ucControllerState = defMODE_ACTIVE;
    // Acceptance Filter information
    // This will accept all messages
    m_sFilter.m_ucACC_Mask0 = defINIT_MASK_VALUE;
    m_sFilter.m_ucACC_Mask1 = defINIT_MASK_VALUE;
    m_sFilter.m_ucACC_Mask2 = defINIT_MASK_VALUE;
    m_sFilter.m_ucACC_Mask3 = defINIT_MASK_VALUE;
    // Code value to 00
    m_sFilter.m_ucACC_Code0 = defINIT_CODE_VALUE;
    m_sFilter.m_ucACC_Code1 = defINIT_CODE_VALUE;
    m_sFilter.m_ucACC_Code2 = defINIT_CODE_VALUE;
    m_sFilter.m_ucACC_Code3 = defINIT_CODE_VALUE;
    // Set the filter type
    m_sFilter.m_ucACC_Filter_Type = 0;
    // Hardware Handle Associated with this channel
    m_hHardwareHandle = 0;
    // Network Handle Associated with this channel
    m_hNetworkHandle = 0;
    // Programmed warning limit of this channel
    m_ucWarningLimit = defWARNING_LIMIT_INT;
    // Tx Error counter value
    m_ucTxErrorCounter = 0;
    // Rx Error counter value
    m_ucRxErrorCounter = 0;
    // Peak values of Rx and Tx Error Counter
    m_ucPeakRxErrorCounter = m_ucPeakTxErrorCounter = 0;
    // Tx Error Handler execution state
    m_bTxErrorExecuted = FALSE;
    // Rx Error Handler execution state
    m_bRxErrorExecuted = FALSE;
}

/*******************************************************************************
  Function Name  : ~CChannel
  Description    : Standard Destructor
  Member of      : CChannel
  Functionality  : -
  Author(s)      : Raja N
  Date Created   : 21.2.2005
  Modifications  :
*******************************************************************************/
CChannel::~CChannel()
{

}

/*******************************************************************************
  Function Name  : vUpdateErrorCounter
  Input(s)       : ucTxError - Tx Error Value
                   ucRxError - Rx Error Value
  Output         : -
  Functionality  : Updates the error counter value and controller state as per
                   the error counter values
  Member of      : CChannel
  Author(s)      : Raja N
  Date Created   : 8.3.2005
  Modifications  : 
*******************************************************************************/
void CChannel::vUpdateErrorCounter(UCHAR ucTxError, UCHAR ucRxError)
{
    // Update Tx Error counter and peak Tx Error Counter
    m_ucTxErrorCounter = ucTxError;
    // Update Peak Value
    if( m_ucTxErrorCounter > m_ucPeakTxErrorCounter )
    {
        m_ucPeakTxErrorCounter = m_ucTxErrorCounter;
    }
    // Update Rx Error counter and peak Rx Error Counter
    m_ucRxErrorCounter = ucRxError;
    // Update Peak Value
    if( m_ucRxErrorCounter > m_ucPeakRxErrorCounter )
    {
        m_ucPeakRxErrorCounter = m_ucRxErrorCounter;
    }

    // Update Controller state
    if ((m_ucTxErrorCounter <= 127) && (m_ucRxErrorCounter <= 127))
    {
        // Error Active Mode
        m_ucControllerState = defCONTROLLER_ACTIVE;
    }

    // The sudden shift to the third state is to avoid 
    // "else if ((byTxError > 127) || (byRxError > 127))"
    else if (m_ucTxErrorCounter == 255)
    {
        // Bus off
        m_ucControllerState = defCONTROLLER_BUSOFF;
    }
    else
    {
        // Error passive
        m_ucControllerState = defCONTROLLER_PASSIVE;
    }
}

/*******************************************************************************
  Function Name  : vResetChannel
  Input(s)       : -
  Output         : -
  Functionality  : This function will reset all state related information. This
                   will be called during the hardware reset of the machine
  Member of      : CChannel
  Author(s)      : Raja N
  Date Created   : 8.3.2005
  Modifications  : 
*******************************************************************************/
void CChannel::vResetChannel()
{
     // Controller state
    m_ucControllerState = defMODE_ACTIVE;
    // Tx Error counter value
    m_ucTxErrorCounter = 0;
    // Rx Error counter value
    m_ucRxErrorCounter = 0;
    // Peak values of Rx and Tx Error Counter
    m_ucPeakRxErrorCounter = m_ucPeakTxErrorCounter = 0;
    // Tx Error Handler execution state
    m_bTxErrorExecuted = FALSE;
    // Rx Error Handler execution state
    m_bRxErrorExecuted = FALSE;
}