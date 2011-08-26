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
 * \file      GUI_FormatMsgCAN.cpp
 * \brief     Implementation of CFormatMsgCAN class
 * \author    Anish Kumar
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * Implementation of CFormatMsgCAN class
 */


#include "CommonClass_stdafx.h"
#include "GUI_FormatMsgCAN.h"
#include "include/Utils_macro.h"
#include "Include/CAN_Error_Defs.h"

struct sERRORMSGINFO
{
    unsigned short m_usErrorCode; // Error code
    TCHAR* m_ptcErorMsg;          // Error message
};
typedef sERRORMSGINFO SERRORMSGINFO;
typedef sERRORMSGINFO* PERRORMSGINFO;
const int ERRORS_DEFINED = 13;


static SERRORMSGINFO sg_asErrorEntry[ERRORS_DEFINED] =
{
    {ERROR_UNKNOWN,       _T("Unknown Error")},
    {BIT_ERROR_RX,        _T("Bus Error - Bit Error(Rx)")},
    {FORM_ERROR_RX,       _T("Bus Error - Form Error(Rx)")},
    {STUFF_ERROR_RX,      _T("Error Frame - Stuff error(Rx)")},
    {OTHER_ERROR_RX,      _T("Bus Error - Other Error(Rx)")},
    {BIT_ERROR_TX,        _T("Bus Error - Bit Error(Tx)")},
    {FORM_ERROR_TX,       _T("Bus Error - Form Error(Tx)")},
    {STUFF_ERROR_TX,      _T("Bus Error - Stuff Error(Tx)")},
    {OTHER_ERROR_TX,      _T("Bus Error - Other Error(Tx)")},
    {ERROR_DEVICE_BUFF_OVERFLOW,        _T("Controller Buffer Overflow")},
    {ERROR_DRIVER_BUFF_OVERFLOW,        _T("Driver Buffer Overflow")},
    {ERROR_APPLICATION_BUFF_OVERFLOW,   _T("Application Buffer Overflow")},
	{ERROR_DLLMSG_BUFF_OVERFLOW,        _T("DLL message buffer Overflow")}   
};

/*******************************************************************************
  Function Name  : CFormatMsgCAN
  Input(s)       : -
  Output         : -
  Functionality  : Constructor
  Member of      : CFormatMsgCAN
  Author(s)      : Ratnadip
  Date Created   : 8.7.2009
  Modifications  : 
*******************************************************************************/
CFormatMsgCAN::CFormatMsgCAN(void)
{
}

/*******************************************************************************
  Function Name  : ~CFormatMsgCAN
  Input(s)       : -
  Output         : -
  Functionality  : Destructor
  Member of      : CFormatMsgCAN
  Author(s)      : Ratnadip
  Date Created   : 8.7.2009
  Modifications  : 
*******************************************************************************/
CFormatMsgCAN::~CFormatMsgCAN(void)
{
}

/*******************************************************************************
  Function Name  : vFormatTime
  Input(s)       : -
  Output         : -
  Functionality  : Format Time
  Member of      : CFormatMsgCAN
  Author(s)      : Ratnadip
  Date Created   : 8.7.2009
  Modifications  : 
*******************************************************************************/
void CFormatMsgCAN::vFormatTime(BYTE bExprnFlag, 
                                SFORMATTEDDATA_CAN* CurrDataCAN)
{
    BYTE bTmpExprnFlag = bExprnFlag;
    if (IS_TM_ABS_SET(bExprnFlag))
    {
        CLEAR_EXPR_TM_BITS(bTmpExprnFlag);
        SET_TM_ABS(bTmpExprnFlag);
        vCalculateAndFormatTM(bTmpExprnFlag, CurrDataCAN->m_u64TimeStamp, 
                              CurrDataCAN->m_acTimeAbs);
    }

    if (IS_TM_REL_SET(bExprnFlag))
    {
        CLEAR_EXPR_TM_BITS(bTmpExprnFlag);
        SET_TM_REL(bTmpExprnFlag);
        vCalculateAndFormatTM(bTmpExprnFlag, CurrDataCAN->m_u64TimeStamp, 
                              CurrDataCAN->m_acTimeRel);
    }

    if (IS_TM_SYS_SET(bExprnFlag))
    {
        CLEAR_EXPR_TM_BITS(bTmpExprnFlag);
        SET_TM_SYS(bTmpExprnFlag);
        vCalculateAndFormatTM(bTmpExprnFlag, CurrDataCAN->m_u64TimeStamp, 
                              CurrDataCAN->m_acTimeSys);
    }
}


/*******************************************************************************
  Function Name  : vFormatDataAndId
  Input(s)       : -
  Output         : -
  Functionality  : Format data and id
  Member of      : CFormatMsgCAN
  Author(s)      : Ratnadip
  Date Created   : 8.7.2009
  Modifications  : 
*******************************************************************************/
void CFormatMsgCAN::vFormatDataAndId(BYTE bExprnFlag, 
                                     SFORMATTEDDATA_CAN* CurrDataCAN)
{
    if (IS_NUM_HEX_SET(bExprnFlag))
    {
        _stprintf(CurrDataCAN->m_acMsgIDHex, FORMAT_STR_ID_HEX, CurrDataCAN->m_dwMsgID);
        int j = 0;  // j declared outside
        for (int i = 0; i < CurrDataCAN->m_byDataLength; i++)
        {
            BYTE CurrDat = CurrDataCAN->m_abData[i];
            _stprintf(&(CurrDataCAN->m_acDataHex[j]), FORMAT_STR_DATA_HEX, CurrDat);
            j += 3;
        }
        CurrDataCAN->m_acDataHex[j] = L'\0';
    }

    if (IS_NUM_DEC_SET(bExprnFlag))
    {
        _stprintf(CurrDataCAN->m_acMsgIDDec, FORMAT_STR_ID_DEC, CurrDataCAN->m_dwMsgID);
        int j = 0;
        for (int i = 0; i < CurrDataCAN->m_byDataLength; i++)
        {
            BYTE CurrDat = CurrDataCAN->m_abData[i];
            _stprintf(&(CurrDataCAN->m_acDataDec[j]), FORMAT_STR_DATA_DEC, CurrDat);
			j += 4;
			CurrDataCAN->m_acDataDec[j-1] = L' ';            
        }
        CurrDataCAN->m_acDataDec[j-1] = L'\0';
    }
}

/*******************************************************************************
  Function Name  : usProcessCurrErrorEntry
  Input(s)       : SERROR_INFO& sErrorInfo
  Output         : Returns Error ID as USHORT.
  Functionality  : Processes the current Error entry and returns the Error code.
  Member of      : CFormatMsgCAN
  Author(s)      : Arunkumar K
  Date Created   : 08-09-2010
  Modifications  : 
*******************************************************************************/
USHORT CFormatMsgCAN::usProcessCurrErrorEntry(SERROR_INFO& sErrorInfo)
{
    // Get the Error code
    USHORT usErrorID;

    if (sErrorInfo.m_ucErrType == ERROR_BUS)
    {
        // Update Statistics information
        usErrorID = sErrorInfo.m_ucReg_ErrCap & 0xE0;
    }
    else if (sErrorInfo.m_ucErrType == ERROR_WARNING_LIMIT_REACHED)
    {
        usErrorID = ERROR_UNKNOWN;
    }
    else if (sErrorInfo.m_ucErrType == ERROR_INTERRUPT)
    {
        usErrorID = ERROR_UNKNOWN;
    }
    else
    {
        usErrorID = sErrorInfo.m_ucErrType;
    }

    return usErrorID;
}

/*******************************************************************************
  Function Name  : vFormatCurrErrorEntry
  Input(s)       : usErrorID
  Output         : TCHAR* (Error Name)
  Functionality  : Gets the pointer to Message data Buffer from PSDI_CAN DLL
				   and updates the same in List Control.
  Member of      : CFormatMsgCAN
  Author(s)      : Arunkumar K
  Date Created   : 08-09-2010
  Modifications  : 
*******************************************************************************/
TCHAR* CFormatMsgCAN::vFormatCurrErrorEntry(USHORT usErrorID)
{    
    BOOL bErrProcessed = FALSE;
    int nCount = 0;

    while ((nCount < ERRORS_DEFINED) && (bErrProcessed == FALSE))
    {
		if (usErrorID == sg_asErrorEntry[nCount].m_usErrorCode)
        {
			bErrProcessed = TRUE;
            return sg_asErrorEntry[nCount].m_ptcErorMsg;            
        }
        nCount++;
    }
	return NULL;        
}

/*******************************************************************************
  Function Name  : vFormatCANDataMsg
  Input(s)       : -
  Output         : -
  Functionality  : Format CAN data bytes
  Member of      : CFormatMsgCAN
  Author(s)      : Ratnadip
  Date Created   : 8.7.2009
  Modifications  : ArunKumar K
                   08.09.2010,
				   Added handler for error messages. ERR_FLAG
*******************************************************************************/
void CFormatMsgCAN::vFormatCANDataMsg(STCANDATA* pMsgCAN, 
                                      SFORMATTEDDATA_CAN* CurrDataCAN,
                                      BYTE bExprnFlag_Log)
{	
    if (RX_FLAG == pMsgCAN->m_ucDataType)
    {
        CurrDataCAN->m_eDirection = DIR_RX;
        CurrDataCAN->m_acMsgDir[0] = _T('R');
    }
    else if (TX_FLAG == pMsgCAN->m_ucDataType)
    {
        CurrDataCAN->m_eDirection = DIR_TX;
        CurrDataCAN->m_acMsgDir[0] = _T('T');
    }
	CurrDataCAN->m_acMsgDir[1] = _T('x');

    TYPE_CHANNEL CurrChannel = pMsgCAN->m_uDataInfo.m_sCANMsg.m_ucChannel;
    if ((CurrChannel >= CHANNEL_CAN_MIN) && (CurrChannel <= CHANNEL_CAN_MAX ))
    {
        _stprintf(CurrDataCAN->m_acChannel, _T("%d"), CurrChannel);
    }

	memset(CurrDataCAN->m_acType,_T('\0'),sizeof(CurrDataCAN->m_acType));
    if (pMsgCAN->m_uDataInfo.m_sCANMsg.m_ucEXTENDED != 0)
    {
        CurrDataCAN->m_byIDType = TYPE_ID_CAN_EXTENDED;
        CurrDataCAN->m_acType[0] = _T('x');
    }
    else
    {
        CurrDataCAN->m_byIDType = TYPE_ID_CAN_STANDARD;
        CurrDataCAN->m_acType[0] = _T('s');
    }

    if (pMsgCAN->m_uDataInfo.m_sCANMsg.m_ucRTR != 0)
    {
        CurrDataCAN->m_byMsgType |= TYPE_MSG_CAN_RTR;
        CurrDataCAN->m_acType[1] = _T('r');
    }
    _itot(pMsgCAN->m_uDataInfo.m_sCANMsg.m_ucDataLen, CurrDataCAN->m_acDataLen, 10);	
    _tcscpy(CurrDataCAN->m_acMsgDesc, _T("Description"));	

    CurrDataCAN->m_u64TimeStamp = pMsgCAN->m_lTickCount.QuadPart;
    CurrDataCAN->m_dwMsgID = pMsgCAN->m_uDataInfo.m_sCANMsg.m_unMsgID;
    CurrDataCAN->m_byDataLength = pMsgCAN->m_uDataInfo.m_sCANMsg.m_ucDataLen;

	memcpy(CurrDataCAN->m_abData, pMsgCAN->m_uDataInfo.m_sCANMsg.m_ucData,
		   CurrDataCAN->m_byDataLength);

	/*PROCESS ERROR MSGS: If Error Message type. Change the data and type fields. */
	if(ERR_FLAG == pMsgCAN->m_ucDataType)
	{		
		USHORT usErrCode = usProcessCurrErrorEntry(pMsgCAN->m_uDataInfo.m_sErrInfo);

		if( usErrCode != ERROR_UNKNOWN )
        {
			// Format error message
			TCHAR* ptrStrErrName = NULL;
            ptrStrErrName = vFormatCurrErrorEntry(usErrCode);	
			if(ptrStrErrName)
			{
				_tcscpy(CurrDataCAN->m_acDataDec, ptrStrErrName);			
				_tcscpy(CurrDataCAN->m_acDataHex, ptrStrErrName);			
			}
		}
		CurrDataCAN->m_dwMsgID = pMsgCAN->m_uDataInfo.m_sCANMsg.m_unMsgID;
		_stprintf(CurrDataCAN->m_acMsgIDDec, FORMAT_STR_ID_DEC, CurrDataCAN->m_dwMsgID);
		
		_tcscpy(CurrDataCAN->m_acType, _T("ERR"));					
	}
	/* PROCESS ERROR MSGS ENDS */

	vFormatTime(bExprnFlag_Log, CurrDataCAN);
    vFormatDataAndId(bExprnFlag_Log, CurrDataCAN);
}