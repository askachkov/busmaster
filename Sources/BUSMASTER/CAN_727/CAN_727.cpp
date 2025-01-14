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
 * \file      CAN_STUB/CAN_STUB.cpp
 * \author    Pradeep Kadoor
 * \copyright Copyright (c) 2011, Robert Bosch Engineering and Business Solutions. All rights reserved.
 *
 * Implementation of CStub
 */

/* C++ includes */
#include <string>
#include <vector>
#include <stdio.h>
#include <time.h>

/* Project includes */
#include "CAN_727_stdafx.h"
#include "CAN_727.h"
//#include "include/Error.h"
//#include "Include/Struct_CAN.h"
//#include "Include/BaseDefs.h"
//#include "Include/CanUSBDefs.h"
//#include "Include/DIL_CommonDefs.h"

//#include "DataTypes/MsgBufAll_DataTypes.h"
#include "BusEmulation/BusEmulation.h"
#include "BusEmulation/BusEmulation_i.c"
//#include "DataTypes/DIL_DataTypes.h"
#include "Utility/Utility_Thread.h"
#include "Utility/Utility.h"
#include "BaseDIL_CAN_Controller.h"
//#include "../BusmasterKernel/BusmasterDriverInterface/Include/CANDriverDefines.h"

//#include "DIL_Interface/BaseDIL_CAN_Controller.h"
//#include "../Application/GettextBusmaster.h"

#include "Utility\MultiLanguageSupport.h"

#define USAGE_EXPORT
#include "CAN_727_Extern.h"

BEGIN_MESSAGE_MAP(CCAN_727App, CWinApp)
END_MESSAGE_MAP()

#define RETURN_ERROR_RESULT(func) HRESULT hResult = S_FALSE; m_LastError = #func; return hResult;

class Logger
{
	Logger()
	{
		
	}

	Logger(const Logger &);
	void operator=(const Logger &);
	static Logger m_Instance;
	
public:
	static Logger& instance()
	{
		return m_Instance;
	}
	void writeLine(const char * format, ...)
	{
		FILE * file;
		file = fopen("log.txt", "a");
		if (file){
			time_t ltime; /* calendar time */
			ltime = time(NULL); /* get current cal time */
			char *timeStr = asctime(localtime(&ltime));
			timeStr[strlen(timeStr) - 1] = 0;

			fprintf(file, "%s ", timeStr);
			va_list args;
			va_start(args, format);
			vfprintf(file, format, args);
			va_end(args);
			fprintf(file, "\r\n");
			fclose(file);
		}
	}
};

#define LOG(FRM, ...) Logger::instance().writeLine(FRM, ##__VA_ARGS__)

Logger Logger::m_Instance;

/**
 * CCAN_STUBApp construction
 */
CCAN_727App::CCAN_727App()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/**
 * The one and only CCAN_STUBApp object
 */
CCAN_727App theApp;

/**
 * CCAN_STUBApp initialization
 */
BOOL CCAN_727App::InitInstance()
{
    CWinApp::InitInstance();
    return TRUE;
}

/**
 * Different action codes
 */
enum
{
    CONNECT = 0x64,
    DISCONNECT,
    STOP_HARDWARE,
    START_HARDWARE,
    SEND_MESSAGE,
    GET_TIME_MAP,
    REGISTER,
    UNREGISTER,
};

/**
 * Starts code for the state machine
 */
enum
{
    STATE_PRIMORDIAL    = 0x0,
    STATE_RESET,
    STATE_REGISTERED,
    STATE_INITIALISED,
    STATE_CONNECTED
};

BYTE sg_bCurrState = STATE_PRIMORDIAL;

/* CDIL_CAN_STUB class definition */
class CDIL_CAN_727 : public CBaseDIL_CAN_Controller
{
public:
	CDIL_CAN_727();
	typedef std::string String;
    /* STARTS IMPLEMENTATION OF THE INTERFACE FUNCTIONS... */
	HRESULT CAN_PerformInitOperations(void);
	HRESULT CAN_PerformClosureOperations(void);
	HRESULT CAN_GetTimeModeMapping(SYSTEMTIME& CurrSysTime, UINT64& TimeStamp, LARGE_INTEGER& QueryTickCount);
	HRESULT CAN_ListHwInterfaces(INTERFACE_HW_LIST& sSelHwInterface, INT& nCount, PSCONTROLLER_DETAILS InitData);
	HRESULT CAN_SelectHwInterface(const INTERFACE_HW_LIST& sSelHwInterface, INT nCount);
	HRESULT CAN_DeselectHwInterface(void);
	HRESULT CAN_SetConfigData(PSCONTROLLER_DETAILS InitData, int Length);
	HRESULT CAN_StartHardware(void);
	HRESULT CAN_StopHardware(void);
	HRESULT CAN_GetCurrStatus(STATUSMSG& StatusData);
	HRESULT CAN_GetTxMsgBuffer(BYTE*& pouFlxTxMsgBuffer);
	HRESULT CAN_SendMsg(DWORD dwClientID, const STCAN_MSG& sCanTxMsg);
	HRESULT CAN_GetBusConfigInfo(BYTE* BusInfo);
	HRESULT CAN_GetLastErrorString(std::string& acErrorStr);
	HRESULT CAN_GetControllerParams(LONG& lParam, UINT nChannel, ECONTR_PARAM eContrParam);
	HRESULT CAN_SetControllerParams(int nValue, ECONTR_PARAM eContrparam);
	HRESULT CAN_GetErrorCount(SERROR_CNT& sErrorCnt, UINT nChannel, ECONTR_PARAM eContrParam);

	// Specific function set
	HRESULT CAN_SetAppParams(HWND hWndOwner);
	HRESULT CAN_ManageMsgBuf(BYTE bAction, DWORD ClientID, CBaseCANBufFSE* pBufObj);
	HRESULT CAN_RegisterClient(BOOL bRegister, DWORD& ClientID, char* pacClientName);
	HRESULT CAN_GetCntrlStatus(const HANDLE& hEvent, UINT& unCntrlStatus);
	HRESULT CAN_LoadDriverLibrary(void);
	HRESULT CAN_UnloadDriverLibrary(void);
	HRESULT CAN_SetHardwareChannel(PSCONTROLLER_DETAILS, DWORD dwDriverId, bool bIsHardwareListed, unsigned int unChannelCount);

private:
	String m_LastError;
	String m_RegistregClient;
};

static CDIL_CAN_727* sg_pouDIL_CAN_STUB = nullptr;

CDIL_CAN_727::CDIL_CAN_727()
{
	LOG("+++++++++++++++++++New+++++++++++++++++++++");
}

/**
 * \return S_OK for success, S_FALSE for failure
 *
 * Returns the interface to controller
 */
USAGEMODE HRESULT GetIDIL_CAN_Controller(void** ppvInterface)
{
    HRESULT hResult = S_OK;
    if ( nullptr == sg_pouDIL_CAN_STUB )
    {
		if ((sg_pouDIL_CAN_STUB = new CDIL_CAN_727) == nullptr)
        {
            hResult = S_FALSE;
        }
    }
    *ppvInterface = (void*) sg_pouDIL_CAN_STUB;  /* Doesn't matter even if sg_pouDIL_CAN_Kvaser is null */
	LOG("GetIDIL_CAN_Controller");
    return hResult;
}

HRESULT CDIL_CAN_727::CAN_PerformInitOperations(void)
{
	//RETURN_ERROR_RESULT(CAN_PerformInitOperations)
	LOG("CAN_PerformInitOperations");
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_PerformClosureOperations(void)
{
	//RETURN_ERROR_RESULT(CAN_PerformClosureOperations)
	LOG("CAN_PerformClosureOperations");
	return S_OK;
}

static SYSTEMTIME       sg_CurrSysTime = { '\0' };
static UINT64           sg_TimeStampRef = 0x0;
static LARGE_INTEGER    sg_QueryTickCount;

HRESULT CDIL_CAN_727::CAN_GetTimeModeMapping(SYSTEMTIME& CurrSysTime, UINT64& TimeStamp, LARGE_INTEGER& QueryTickCount)
{
	LOG("CAN_GetTimeModeMapping");
	CurrSysTime = sg_CurrSysTime;
	TimeStamp = sg_TimeStampRef;
	QueryTickCount = sg_QueryTickCount;
	//RETURN_ERROR_RESULT(CAN_GetTimeModeMapping)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_ListHwInterfaces(INTERFACE_HW_LIST& sSelHwInterface, INT& nCount, PSCONTROLLER_DETAILS InitData)
{
	LOG("CAN_ListHwInterfaces");
	//RETURN_ERROR_RESULT(CAN_ListHwInterfaces)
	sSelHwInterface[0].m_dwIdInterface = 0x100;
	sSelHwInterface[0].m_acNameInterface = "727_Simulation";
	sSelHwInterface[0].m_acDescription = "Blablabla";
	nCount = 1;
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_SelectHwInterface(const INTERFACE_HW_LIST& sSelHwInterface, INT nCount)
{
	LOG("CAN_SelectHwInterface");
	//RETURN_ERROR_RESULT(CAN_SelectHwInterface)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_DeselectHwInterface(void)
{
	LOG("CAN_DeselectHwInterface");
	//RETURN_ERROR_RESULT(CAN_DeselectHwInterface)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_SetConfigData(PSCONTROLLER_DETAILS InitData, int Length)
{
	LOG("CAN_SetConfigData");
	//RETURN_ERROR_RESULT(CAN_SetConfigData)
	((PSCONTROLLER_DETAILS)InitData)[0].m_omHardwareDesc =
		"727 Simulation";
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_StartHardware(void)
{
	LOG("CAN_StartHardware");
	//RETURN_ERROR_RESULT(CAN_StartHardware)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_StopHardware(void)
{
	LOG("CAN_StopHardware");
	//RETURN_ERROR_RESULT(CAN_StopHardware)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_GetCurrStatus(STATUSMSG& StatusData)
{
	LOG("CAN_GetCurrStatus");
	//RETURN_ERROR_RESULT(CAN_GetCurrStatus)
	StatusData.wControllerStatus = NORMAL_ACTIVE;
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_GetTxMsgBuffer(BYTE*& pouFlxTxMsgBuffer)
{
	LOG("CAN_GetTxMsgBuffer");
	//RETURN_ERROR_RESULT(CAN_GetTxMsgBuffer)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_SendMsg(DWORD dwClientID, const STCAN_MSG& sCanTxMsg)
{
	LOG("CAN_SendMsg");
	//RETURN_ERROR_RESULT(CAN_SendMsg)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_GetBusConfigInfo(BYTE* BusInfo)
{
	LOG("CAN_GetBusConfigInfo");
	RETURN_ERROR_RESULT(CAN_GetBusConfigInfo)
}

HRESULT CDIL_CAN_727::CAN_GetLastErrorString(std::string& acErrorStr)
{
	LOG("CAN_GetLastErrorString: %s", m_LastError.c_str());
	acErrorStr = m_LastError;
	return S_OK;
}


#ifdef _MSC_VER                 // Windows includes

#include <windows.h>
#include <conio.h>
#include <winioctl.h>

#include <setupapi.h>
#include <initguid.h>           /* Required for GUID definition */
#include <Setupapi.h>

HRESULT CDIL_CAN_727::CAN_GetControllerParams(LONG& lParam, UINT nChannel, ECONTR_PARAM eContrParam)
{

	Handle = SetupDiGetClassDevs(&GUID_DEVINTERFACE_VOLUME, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	LOG("CAN_GetControllerParams");
	//RETURN_ERROR_RESULT(CAN_GetControllerParams)
	HRESULT hResult = S_OK;
	switch (eContrParam)
	{
		case NUMBER_HW:
			{
				lParam = CHANNEL_ALLOWED;
			}
			break;
		case NUMBER_CONNECTED_HW:
			{
				lParam = CHANNEL_ALLOWED;
			}
			break;
		case DRIVER_STATUS:
			{
				lParam = TRUE;
			}
			break;
		case HW_MODE:
			{
				lParam = defMODE_SIMULATE;
			}
			break;
		case CON_TEST:
			{
				lParam = (LONG)-1;
			}
			break;
		default:
			{
				hResult = S_FALSE;
			}
			break;

	}
	return hResult;
}

HRESULT CDIL_CAN_727::CAN_SetControllerParams(int nValue, ECONTR_PARAM eContrparam)
{
	LOG("CAN_SetControllerParams");
	//RETURN_ERROR_RESULT(CAN_SetControllerParams)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_GetErrorCount(SERROR_CNT& sErrorCnt, UINT nChannel, ECONTR_PARAM eContrParam)
{
	LOG("CAN_GetErrorCount");
	//RETURN_ERROR_RESULT(CAN_GetErrorCount)
	memset(&sErrorCnt, 0, sizeof(SERROR_CNT));
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_SetAppParams(HWND hWndOwner)
{
	//RETURN_ERROR_RESULT(CAN_SetAppParams)
	LOG("CAN_SetAppParams");
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_ManageMsgBuf(BYTE bAction, DWORD ClientID, CBaseCANBufFSE* pBufObj)
{
	LOG("CAN_ManageMsgBuf");
	//RETURN_ERROR_RESULT(CAN_ManageMsgBuf)
	HRESULT hResult = S_OK;
	if (ClientID != 0){
		hResult = ERR_NO_CLIENT_EXIST;
	}
	return hResult;
}

HRESULT CDIL_CAN_727::CAN_RegisterClient(BOOL bRegister, DWORD& ClientID, char* pacClientName)
{
	LOG("CAN_RegisterClient");
	//RETURN_ERROR_RESULT(CAN_RegisterClient)
	HRESULT hResult = S_FALSE;
	if (bRegister)
	{
		if (m_RegistregClient.empty()){
			ClientID = 0;
			m_RegistregClient = pacClientName;
			hResult = S_OK;
		}
		else if (m_RegistregClient.compare(pacClientName) == 0){
			ClientID = 0;
			hResult = ERR_CLIENT_EXISTS;
		}
		else {
			hResult = ERR_NO_MORE_CLIENT_ALLOWED;
		}
	}
	else
	{
		if (m_RegistregClient.empty()){
			hResult = ERR_NO_CLIENT_EXIST;
		}
		else {
			m_RegistregClient = "";
			hResult = S_OK;
		}
	}
	LOG("CAN_RegisterClient reg:%s; name:%s", bRegister ? "true" : "false", pacClientName != 0 ? pacClientName : "null");
	return hResult;
}

HRESULT CDIL_CAN_727::CAN_GetCntrlStatus(const HANDLE& hEvent, UINT& unCntrlStatus)
{
	LOG("CAN_GetCntrlStatus");
	//RETURN_ERROR_RESULT(CAN_GetCntrlStatus)
	HRESULT hResult = S_OK;
	unCntrlStatus = NORMAL_ACTIVE;
	return hResult;
}

HRESULT CDIL_CAN_727::CAN_LoadDriverLibrary(void)
{
	LOG("CAN_LoadDriverLibrary");
	//RETURN_ERROR_RESULT(CAN_LoadDriverLibrary)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_UnloadDriverLibrary(void)
{
	LOG("CAN_UnloadDriverLibrary");
	//RETURN_ERROR_RESULT(CAN_UnloadDriverLibrary)
	return S_OK;
}

HRESULT CDIL_CAN_727::CAN_SetHardwareChannel(PSCONTROLLER_DETAILS, DWORD dwDriverId, bool bIsHardwareListed, unsigned int unChannelCount)
{
	//RETURN_ERROR_RESULT(CAN_SetHardwareChannel)
	// no return values
	LOG("CAN_SetHardwareChannel %d", unChannelCount);
	return S_OK;
}
