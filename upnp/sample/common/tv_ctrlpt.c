/*******************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*!
 * \addtogroup UpnpSamples
 *
 * @{
 *
 * \name Control Point Sample Module
 *
 * @{
 *
 * \file
 */

#include "tv_ctrlpt.h"

#include "upnp.h"
#include "sample_util.h"

/*!
 * Mutex for protecting the global device list in a multi-threaded,
 * asynchronous environment. All functions should lock this mutex before
 * reading or writing the device list.
 */
ithread_mutex_t DeviceListMutex;

UpnpClient_Handle ctrlpt_handle = -1;

/*! Device type for tv device. */
const char TvDeviceType[] = "urn:schemas-upnp-org:device:tvdevice:1";
const char DuDeviceType[] = "urn:schemas-upnp-org:device:MediaRenderer:1";
/*! Service names.*/
const char *TvServiceName[] = {"Control", "Picture", ""};
const char *DuServiceName[] = {"AVTransport", "ConnectionManager", "RenderingControl"};

/*!
   Global arrays for storing variable names and counts for
   TvControl and TvPicture services
 */
const char *TvVarName[DU_SERVICE_SERVCOUNT][DU_MAXVARS] = {
	{CTRL_POWER, CTRL_CHANNEL, CTRL_VOLUME, CTRL_LOGLVL},
	{PIC_COLOR, PIC_TINT, PIC_CONTRAST, PIC_BRIGHTNESS}};
char TvVarCount[TV_SERVICE_SERVCOUNT] = {
	TV_CONTROL_VARCOUNT, TV_PICTURE_VARCOUNT};
//"LastChange", "RelativeTimePosition","AbsoluteCounterPosition", "RelativeCounterPosition",
// "A_ARG_TYPE_InstanceID", "AbsoluteTimePosition", "A_ARG_TYPE_SeekMode", 

// "PresetName",  "Channel",          "InstanceID",   "LastChange", 
const char *DuVarName[DU_SERVICE_SERVCOUNT][DU_MAXVARS] = {
    {"CurrentPlayMode",           "RecordStorageMedium",       "CurrentTrackURI", 
     "CurrentTrackDuration",      "CurrentRecordQualityMode",  "CurrentMediaDuration",  
     "InstanceID",                "AVTransportURI",            "TransportPlaySpeed",
	 "TransportState",            "CurrentTrackMetaData",      "NextAVTransportURI", 
	 "PossibleRecordQualityModes","CurrentTrack",              "TransportStatus",  
	 "NextAVTransportURIMetaData","PlaybackStorageMedium",     "CurrentTransportActions",
	 "RecordMediumWriteStatus",   "PossiblePlaybackStorageMedia", "AVTransportURIMetaData",
	 "NumberOfTracks",            "PossibleRecordStorageMedia", NULL,
 	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL},

	{"SourceProtocolInfo",        "SinkProtocolInfo",          "CurrentConnectionIDs",
	 "ProtocolInfo",              "ConnectionStatus",          "AVTransportID",
	 "RcsID",                     "ConnectionID",              "ConnectionManager",
	 "Direction",                 NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,	 
 	 NULL,                        NULL,                         NULL,
 	 NULL,                        NULL,                         NULL,	 
	 NULL,                        NULL},

	{"Volume",           "PresetNameList",               "VolumeDB",
	 "Mute",                      NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,	 
	 NULL,                        NULL,                         NULL,
	 NULL,                        NULL,                         NULL,	 
	 NULL,                        NULL},
    };
char DuVarCount[DU_SERVICE_SERVCOUNT] = {
	DU_TRANSPORT_VARCOUNT, DU_MANAGER_VARCOUNT, DU_RENDER_VARCOUNT};

/*!
   Timeout to request during subscriptions
 */
int default_timeout = 1801;

/*!
   The first node in the global device list, or NULL if empty
 */
struct TvDeviceNode *GlobalDeviceList = NULL;

/********************************************************************************
 * TvCtrlPointDeleteNode
 *
 * Description:
 *       Delete a device node from the global device list.  Note that this
 *       function is NOT thread safe, and should be called from another
 *       function that has already locked the global device list.
 *
 * Parameters:
 *   node -- The device node
 *
 ********************************************************************************/
int TvCtrlPointDeleteNode(struct TvDeviceNode *node)
{
	int rc, service, var;

	if (NULL == node) {
		SampleUtilPrintf(UPNP_ERROR, "Node is empty\n");
		return TV_ERROR;
	}

	for (service = 0; service < TV_SERVICE_SERVCOUNT; service++) {
		/*
		   If we have a valid control SID, then unsubscribe
		 */
		if (strcmp(node->device.TvService[service].SID, "") != 0) {
			rc = UpnpUnSubscribe(ctrlpt_handle,	node->device.TvService[service].SID);
			if (UPNP_E_SUCCESS == rc) {
				SampleUtilPrint("Unsubscribed from Tv %s EventURL with SID=%s\n",
					TvServiceName[service], node->device.TvService[service].SID);
			} else {
				SampleUtilPrintf(UPNP_ERROR, "unsubscribing to Tv %s EventURL %d\n",
					TvServiceName[service], rc);
			}
		}

		for (var = 0; var < TvVarCount[service]; var++) {
			if (node->device.TvService[service].VariableStrVal[var]) {
				free(node->device.TvService[service].VariableStrVal[var]);
			}
		}
	}

	/*Notify New Device Added */
	SampleUtil_StateUpdate(NULL, NULL, node->device.UDN, DEVICE_REMOVED);
	free(node);
	node = NULL;

	return TV_SUCCESS;
}

/********************************************************************************
 * TvCtrlPointRemoveDevice
 *
 * Description:
 *       Remove a device from the global device list.
 *
 * Parameters:
 *   UDN -- The Unique Device Name for the device to remove
 *
 ********************************************************************************/
int TvCtrlPointRemoveDevice(const char *UDN)
{
	struct TvDeviceNode *curdevnode;
	struct TvDeviceNode *prevdevnode;

	ithread_mutex_lock(&DeviceListMutex);

	curdevnode = GlobalDeviceList;
	if (!curdevnode) {
		SampleUtilPrintf(UPNP_ERROR, "TvCtrlPointRemoveDevice: Device list empty\n");
	} else {
		if (0 == strcmp(curdevnode->device.UDN, UDN)) {
			GlobalDeviceList = curdevnode->next;
			TvCtrlPointDeleteNode(curdevnode);
		} else {
			prevdevnode = curdevnode;
			curdevnode = curdevnode->next;
			while (curdevnode) {
				if (strcmp(curdevnode->device.UDN, UDN) == 0) {
					prevdevnode->next = curdevnode->next;
					TvCtrlPointDeleteNode(curdevnode);
					break;
				}
				prevdevnode = curdevnode;
				curdevnode = curdevnode->next;
			}
		}
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return TV_SUCCESS;
}

/********************************************************************************
 * TvCtrlPointRemoveAll
 *
 * Description:
 *       Remove all devices from the global device list.
 *
 * Parameters:
 *   None
 *
 ********************************************************************************/
int TvCtrlPointRemoveAll(void)
{
	struct TvDeviceNode *curdevnode, *next;

	ithread_mutex_lock(&DeviceListMutex);

	curdevnode = GlobalDeviceList;
	GlobalDeviceList = NULL;

	while (curdevnode) {
		next = curdevnode->next;
		TvCtrlPointDeleteNode(curdevnode);
		curdevnode = next;
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return TV_SUCCESS;
}

/********************************************************************************
 * TvCtrlPointRefresh
 *
 * Description:
 *       Clear the current global device list and issue new search
 *	 requests to build it up again from scratch.
 *
 * Parameters:
 *   None
 *
 ********************************************************************************/
int TvCtrlPointRefresh(void)
{
	int rc;

	TvCtrlPointRemoveAll();
	/* Search for all devices of type tvdevice version 1,
	 * waiting for up to 5 seconds for the response */
	rc = UpnpSearchAsync(ctrlpt_handle, 5, TvDeviceType, NULL);
	if (UPNP_E_SUCCESS != rc) {
		SampleUtil_Print("Error sending search request%d\n", rc);

		return TV_ERROR;
	}
	rc = UpnpSearchAsync(ctrlpt_handle, 5, DuDeviceType, NULL);
	if (UPNP_E_SUCCESS != rc) {
		SampleUtil_Print("Error sending search request%d\n", rc);

		return TV_ERROR;
	}

	return TV_SUCCESS;
}

/********************************************************************************
 * TvCtrlPointGetVar
 *
 * Description:
 *       Send a GetVar request to the specified service of a device.
 *
 * Parameters:
 *   service -- The service
 *   devnum -- The number of the device (order in the list,
 *             starting with 1)
 *   varname -- The name of the variable to request.
 *
 ********************************************************************************/
int TvCtrlPointGetVar(int service, int devnum, const char *varname)
{
	struct TvDeviceNode *devnode;
	int rc;

	ithread_mutex_lock(&DeviceListMutex);

	rc = TvCtrlPointGetDevice(devnum, &devnode);

	if (TV_SUCCESS == rc) {
		rc = UpnpGetServiceVarStatusAsync(ctrlpt_handle,
			devnode->device.TvService[service].ControlURL,
			varname,
			TvCtrlPointCallbackEventHandler,
			NULL);
		if (rc != UPNP_E_SUCCESS) {
			SampleUtilPrint(
				"Error in UpnpGetServiceVarStatusAsync -- %d\n",
				rc);
			rc = TV_ERROR;
		}
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return rc;
}

int TvCtrlPointGetPower(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_CONTROL, devnum, CTRL_POWER);
}

int TvCtrlPointGetChannel(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_CONTROL, devnum, CTRL_CHANNEL);
}

int TvCtrlPointGetVolume(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_CONTROL, devnum, CTRL_VOLUME);
}

int TvCtrlPointGetColor(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_PICTURE, devnum, PIC_COLOR);
}

int TvCtrlPointGetTint(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_PICTURE, devnum, PIC_TINT);
}

int TvCtrlPointGetContrast(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_PICTURE, devnum, PIC_CONTRAST);
}

int TvCtrlPointGetBrightness(int devnum)
{
	return TvCtrlPointGetVar(TV_SERVICE_PICTURE, devnum, PIC_BRIGHTNESS);
}

/********************************************************************************
 * TvCtrlPointSendAction
 *
 * Description:
 *       Send an Action request to the specified service of a device.
 *
 * Parameters:
 *   service -- The service
 *   devnum -- The number of the device (order in the list,
 *             starting with 1)
 *   actionname -- The name of the action.
 *   param_name -- An array of parameter names
 *   param_val -- The corresponding parameter values
 *   param_count -- The number of parameters
 *
 ********************************************************************************/
int TvCtrlPointSendAction(int service,
	int devnum,
	const char *actionname,
	const char **param_name,
	char **param_val,
	int param_count)
{
	struct TvDeviceNode *devnode;
	IXML_Document *actionNode = NULL;
	int rc = TV_SUCCESS;
	int param;
    int SrvTypeCnt = DU_SERVICE_SERVCOUNT;
    char *SrvType[DU_SERVICE_SERVCOUNT];
    
	ithread_mutex_lock(&DeviceListMutex);

	rc = TvCtrlPointGetDevice(devnum, &devnode);
	if (TV_SUCCESS == rc) {
        GetServiceTypeByDev(devnode->device.UDN, SrvType, &SrvTypeCnt);
        if (SrvType != NULL && service < SrvTypeCnt) {
    		if (0 == param_count) {
    			actionNode = UpnpMakeAction(
    				actionname, SrvType[service], 0, NULL);
    		} else {
    			for (param = 0; param < param_count; param++) {
    				int ret = UpnpAddToAction(&actionNode, actionname,
    					SrvType[service], param_name[param], param_val[param]);
    				if (ret != UPNP_E_SUCCESS) {
    					SampleUtilPrint("ERROR: UpnpAddToAction:%d\n", ret);
    				}
    			}
    		}

    		rc = UpnpSendActionAsync(ctrlpt_handle, devnode->device.TvService[service].ControlURL,
    			SrvType[service], NULL,	actionNode,	TvCtrlPointCallbackEventHandler, NULL);
            SampleUtilPrint("ctrl:%s\n SrvType:%s\n Action:%s\n",
                devnode->device.TvService[service].ControlURL,
    			SrvType[service], actionname);
    		if (rc != UPNP_E_SUCCESS) {
    			SampleUtilPrint("Error in UpnpSendActionAsync -- %d\n", rc);
    			rc = TV_ERROR;
    		}
        }
	}

	ithread_mutex_unlock(&DeviceListMutex);

	if (actionNode)
		ixmlDocument_free(actionNode);

	return rc;
}

/********************************************************************************
 * TvCtrlPointSendActionNumericArg
 *
 * Description:Send an action with one argument to a device in the global device
 *list.
 *
 * Parameters:
 *   devnum -- The number of the device (order in the list, starting with 1)
 *   service -- TV_SERVICE_CONTROL or TV_SERVICE_PICTURE
 *   actionName -- The device action, i.e., "SetChannel"
 *   paramName -- The name of the parameter that is being passed
 *   paramValue -- Actual value of the parameter being passed
 *
 ********************************************************************************/
int TvCtrlPointSendActionNumericArg(int devnum,
	int service,
	const char *actionName,
	const char *paramName,
	int paramValue)
{
	char param_val_a[50];
	char *param_val = param_val_a;

	sprintf(param_val_a, "%d", paramValue);
	return TvCtrlPointSendAction(
		service, devnum, actionName, &paramName, &param_val, 1);
}

int TvCtrlPointSendPowerOn(int devnum)
{
	return TvCtrlPointSendAction(
		TV_SERVICE_CONTROL, devnum, SET_POWER_ON, NULL, NULL, 0);
}

int TvCtrlPointSendPowerOff(int devnum)
{
	return TvCtrlPointSendAction(
		TV_SERVICE_CONTROL, devnum, SET_POWER_OFF, NULL, NULL, 0);
}

int TvCtrlPointSendSetChannel(int devnum, int channel)
{
	return TvCtrlPointSendActionNumericArg(
		devnum, TV_SERVICE_CONTROL, SET_CHANNEL, CTRL_CHANNEL, channel);
}

int TvCtrlPointSendSetVolume(int devnum, int volume)
{
	return TvCtrlPointSendActionNumericArg(
		devnum, TV_SERVICE_CONTROL, SET_VOLUME, CTRL_VOLUME, volume);
}

int TvCtrlPointSendSetLog(int devnum, int level, int mod)
{
    if (devnum == 0) {
        UpnpSetLogLevel(level);
        UpnpSetLogModule(mod);
    } else {
	    TvCtrlPointSendActionNumericArg(devnum,
		    TV_SERVICE_CONTROL, SET_LOG, CTRL_LOGLVL, (level << 16) + mod);
    }
    return 0;
}

int TvCtrlPointSendSetColor(int devnum, int color)
{
	return TvCtrlPointSendActionNumericArg(
		devnum, TV_SERVICE_PICTURE, SET_COLOR, PIC_COLOR, color);
}

int TvCtrlPointSendSetTint(int devnum, int tint)
{
	return TvCtrlPointSendActionNumericArg(
		devnum, TV_SERVICE_PICTURE, SET_TINT, PIC_TINT, tint);
}

int TvCtrlPointSendSetContrast(int devnum, int contrast)
{
	return TvCtrlPointSendActionNumericArg(devnum,
		TV_SERVICE_PICTURE,
		SET_CONTRAST,
		PIC_CONTRAST,
		contrast);
}

int TvCtrlPointSendSetBrightness(int devnum, int brightness)
{
	return TvCtrlPointSendActionNumericArg(devnum,
		TV_SERVICE_PICTURE,
		SET_BRIGHTNESS,
		PIC_BRIGHTNESS,
		brightness);
}

int TvCtrlPointSendPlay(int devnum, int brightness)
{
    char *paramName[2] = {"InstanceID", "Speed"};
	char *paramVal[2] = {"0", "1"};

	return TvCtrlPointSendAction(
		0, devnum, Play, paramName, paramVal, 2);

    return 0;
}

#define ccvt1 "http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8"
#define ccvt3 "http://ivi.bupt.edu.cn/hls/cctv3hd.m3u8"
#define ccvt5 "http://ivi.bupt.edu.cn/hls/cctv5hd.m3u8"
#define ccvt6 "http://ivi.bupt.edu.cn/hls/cctv6hd.m3u8"
#define locavi "http://192.168.137.1:53467/temp4.mp4"

#define vvv "&lt;DIDL-Lite "\
"xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" "\
"xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "\
"xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\"&gt;"\
"&lt;item id=\"temp4\" parentID=\"-1\" restricted=\"1\"&gt;"\
    "&lt;upnp:storageMedium&gt;UNKNOWN&lt;/upnp:storageMedium&gt;"\
    "&lt;upnp:writeStatus&gt;UNKNOWN&lt;/upnp:writeStatus&gt;"\
    "&lt;dc:title&gt;temp4.mp4&lt;/dc:title&gt;"\
    "&lt;upnp:class&gt;object.item.videoItem.movie&lt;/upnp:class&gt;"\
    "&lt;res size=\"153475138\" protocolInfo=\"http-get:*:video/mp4:*\"&gt;"\
        "http://192.168.137.1:53467/temp4.mp4"\
    "&lt;/res&gt;"\
"&lt;/item&gt;"\
"&lt;/DIDL-Lite&gt;"

int TvCtrlPointSendSetAVTransportURI(int devnum, int brightness)
{
    char *paramName[3] = {"InstanceID", "CurrentURI", "CurrentURIMetaData"};
	char *paramVal[3] = {"0", locavi, vvv};

	return TvCtrlPointSendAction(
		0, devnum, SetAVTransportURI, paramName, paramVal, 3);

    return 0;
}

int TvCtrlPointSendGetPositionInfo(int devnum, int brightness)
{
    char *paramName[1] = {"InstanceID"};
	char *paramVal[1] = {"0"};

	return TvCtrlPointSendAction(
		0, devnum, GetPositionInfo, paramName, paramVal, 1);

    return 0;
}

int TvCtrlPointSendGetTransportInfo(int devnum, int brightness)
{
    char *paramName[1] = {"InstanceID"};
	char *paramVal[1] = {"0"};

	return TvCtrlPointSendAction(
		0, devnum, GetTransportInfo, paramName, paramVal, 1);

    return 0;
}

/********************************************************************************
 * TvCtrlPointGetDevice
 *
 * Description:
 *       Given a list number, returns the pointer to the device
 *       node at that position in the global device list.  Note
 *       that this function is not thread safe.  It must be called
 *       from a function that has locked the global device list.
 *
 * Parameters:
 *   devnum -- The number of the device (order in the list,
 *             starting with 1)
 *   devnode -- The output device node pointer
 *
 ********************************************************************************/
int TvCtrlPointGetDevice(int devnum, struct TvDeviceNode **devnode)
{
	int count = devnum;
	struct TvDeviceNode *tmpdevnode = NULL;

	if (count)
		tmpdevnode = GlobalDeviceList;
	while (--count && tmpdevnode) {
		tmpdevnode = tmpdevnode->next;
	}
	if (!tmpdevnode) {
		SampleUtilPrintf(UPNP_ERROR, "finding TvDev num %d\n", devnum);
		return TV_ERROR;
	}
	*devnode = tmpdevnode;

	return TV_SUCCESS;
}

/********************************************************************************
 * TvCtrlPointPrintList
 *
 * Description:
 *       Print the universal device names for each device in the global device
 *list
 *
 * Parameters:
 *   None
 *
 ********************************************************************************/
int TvCtrlPointPrintList()
{
	struct TvDeviceNode *tmpdevnode;
	int i = 0;

	ithread_mutex_lock(&DeviceListMutex);

	SampleUtil_Print("TvCtrlPointPrintList:\n");
	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		SampleUtil_Print(" %3d -- %s\n", ++i, tmpdevnode->device.UDN);
		tmpdevnode = tmpdevnode->next;
	}
	SampleUtil_Print("\n");
	ithread_mutex_unlock(&DeviceListMutex);

	return TV_SUCCESS;
}

int TvCtrlPointPrintDeviceNode(struct TvDeviceNode *tmpdevnode)
{
	char spacer[256];
    int service = 0, var = 0;
    char* devType = tmpdevnode->device.DeviceType;
    int SrvNameCnt = DU_SERVICE_SERVCOUNT;
    char *SrvName[DU_SERVICE_SERVCOUNT];
	char *VarName[DU_SERVICE_SERVCOUNT][DU_MAXVARS] = { 0 };
    char *VarCount = NULL;
    GetServiceVarByDevType(devType, SrvName, &SrvNameCnt, &VarCount, VarName);

    SampleUtil_Print(
             "    |                  \n"
             "    +- UDN        = %s\n"
             "    +- DescDocURL     = %s\n"
             "    +- FriendlyName   = %s\n"
             "    +- PresURL        = %s\n"
             "    +- Adver. TimeOut = %d\n",
        tmpdevnode->device.UDN,
        tmpdevnode->device.DescDocURL,
        tmpdevnode->device.FriendlyName,
        tmpdevnode->device.PresURL,
        tmpdevnode->device.AdvrTimeOut);
    for (service = 0; service < SrvNameCnt; service++) {
        if (service < SrvNameCnt - 1)
            sprintf(spacer, "    |    ");
        else
            sprintf(spacer, "         ");
        SampleUtil_Print("    |                  \n"
                 "    +- Tv %s Service\n"
                 "%s+- ServiceId       = %s\n"
                 "%s+- ServiceType     = %s\n"
                 "%s+- EventURL        = %s\n"
                 "%s+- ControlURL      = %s\n"
                 "%s+- SID             = %s\n"
                 "%s+- ServiceStateTable\n",
            SrvName[service],
            spacer, tmpdevnode->device.TvService[service].ServiceId,
            spacer, tmpdevnode->device.TvService[service].ServiceType,
            spacer, tmpdevnode->device.TvService[service].EventURL,
            spacer, tmpdevnode->device.TvService[service].ControlURL,
            spacer, tmpdevnode->device.TvService[service].SID,
            spacer);
        for (var = 0; var < VarCount[service]; var++) {
            SampleUtil_Print("%s     +- %-10s = %s\n",
                spacer, VarName[service][var],
                tmpdevnode->device.TvService[service].VariableStrVal[var]);
        }
    }
    return 0;
}



/********************************************************************************
 * TvCtrlPointPrintDevice
 *
 * Description:
 *       Print the identifiers and state table for a device from
 *       the global device list.
 *
 * Parameters:
 *   devnum -- The number of the device (order in the list,
 *             starting with 1)
 *
 ********************************************************************************/
int TvCtrlPointPrintDevice(int devnum)
{
	struct TvDeviceNode *tmpdevnode;
	int i = 0, service, var;


	if (devnum <= 0) {
		SampleUtil_Print("Error in TvCtrlPointPrintDevice: "
				 "invalid devnum = %d\n", devnum);
		return TV_ERROR;
	}

	ithread_mutex_lock(&DeviceListMutex);

	SampleUtil_Print("TvCtrlPointPrintDevice:\n");
	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		i++;
		if (i == devnum)
			break;
		tmpdevnode = tmpdevnode->next;
	}
	if (!tmpdevnode) {
		SampleUtil_Print("Error in TvCtrlPointPrintDevice: "
			"invalid devnum = %d  --  actual device count = %d\n",
			devnum,	i);
	} else {
        TvCtrlPointPrintDeviceNode(tmpdevnode);
    }
	SampleUtil_Print("\n");
	ithread_mutex_unlock(&DeviceListMutex);

	return TV_SUCCESS;
}

int TvCtrlPointAddDeviceTv(IXML_Document * DescDoc, const char *UDN,
    struct TvDeviceNode **device, const char * location,int expires)
{
	int ret;
	char *friendlyName = NULL;
    char *baseURL = NULL;
	char *relURL = NULL;
    char *presURL = NULL;
	struct TvDeviceNode *deviceNode = NULL;
    char* deviceType = SampleUtil_GetFirstDocumentItem(DescDoc, "deviceType");    
	friendlyName = SampleUtil_GetFirstDocumentItem(DescDoc, "friendlyName");
	baseURL = SampleUtil_GetFirstDocumentItem(DescDoc, "URLBase");
	relURL = SampleUtil_GetFirstDocumentItem(DescDoc, "presentationURL");

	ret = UpnpResolveURL2((baseURL ? baseURL : location), relURL, &presURL);
	if (ret != UPNP_E_SUCCESS) {
	    return ret;
	}
    SampleUtilPrintf(UPNP_INFO, "gen presURL from %s:%s\n", baseURL, relURL);

    /* Create a new device node */
    deviceNode = (struct TvDeviceNode *)malloc(sizeof(struct TvDeviceNode));
    strcpy(deviceNode->device.UDN, UDN);
    strcpy(deviceNode->device.DeviceType, deviceType);
    strcpy(deviceNode->device.DescDocURL, location);
    strcpy(deviceNode->device.FriendlyName, friendlyName);
    strcpy(deviceNode->device.PresURL, presURL);
    deviceNode->device.AdvrTimeOut = expires;
	*device = deviceNode;
	if (friendlyName)
		free(friendlyName);
	if (deviceType)
		free(deviceType);    
	if (baseURL)
		free(baseURL);
	if (relURL)
		free(relURL);
	if (presURL)
		free(presURL);
    return ret;
}

int TvCtrlPointAddDeviceTvSrv(IXML_Document * DescDoc,
    struct TvDeviceNode *deviceNode, const char * location)
{
	SampleUtilPrint("Found Tv device\n");
	struct TvDeviceNode *tmpdevnode;
	char *serviceId[TV_SERVICE_SERVCOUNT] = { NULL, NULL };
	char *eventURL[TV_SERVICE_SERVCOUNT] = { NULL, NULL };
	char *controlURL[TV_SERVICE_SERVCOUNT] = { NULL, NULL };
	Upnp_SID eventSID[TV_SERVICE_SERVCOUNT] = { 0 };
	int TimeOut[TV_SERVICE_SERVCOUNT] = { default_timeout, default_timeout };
	int service;
	int ret = 1;
	int var;

    const char **ServiceType = TvServiceType; 
    char *VarCount = TvVarCount;
    int ServiceNum = TV_SERVICE_SERVCOUNT;

	for (service = 0; service < ServiceNum; service++) {
		if (SampleUtil_FindAndParseService(DescDoc, location, ServiceType[service],
				&serviceId[service], &eventURL[service], &controlURL[service])) {
			SampleUtilPrint("Subscribing to EventURL[%d] %s\n", service, eventURL[service]);
			ret = UpnpSubscribe(ctrlpt_handle, eventURL[service], &TimeOut[service],eventSID[service]);
			if (ret == UPNP_E_SUCCESS) {
				SampleUtilPrint("Subscribed to EventURL with SID=%s\n",	eventSID[service]);
			} else {
				SampleUtilPrint("Error Subscribing to EventURL -- %d\n", ret);
				strcpy(eventSID[service], "");
			}
		} else {
			SampleUtilPrint("Error: Could not find Service: %s\n", ServiceType[service]);
		}
	}

	for (service = 0; service < ServiceNum; service++) {
		if (serviceId[service] == NULL) {
			/* not found */
			continue;
		}
		strcpy(deviceNode->device.TvService[service].ServiceId,	serviceId[service]);
		strcpy(deviceNode->device.TvService[service].ServiceType, ServiceType[service]);
		strcpy(deviceNode->device.TvService[service].ControlURL, controlURL[service]);
		strcpy(deviceNode->device.TvService[service].EventURL, eventURL[service]);
		strcpy(deviceNode->device.TvService[service].SID, eventSID[service]);
		for (var = 0; var < VarCount[service]; var++) {
			deviceNode->device.TvService[service].VariableStrVal[var] =	(char *)malloc(TV_MAX_VAL_LEN);
			strcpy(deviceNode->device.TvService[service].VariableStrVal[var],"");
		}
	}
	deviceNode->next = NULL;
	/* Insert the new device node in the list */
	if ((tmpdevnode = GlobalDeviceList)) {
		while (tmpdevnode) {
			if (tmpdevnode->next) {
				tmpdevnode = tmpdevnode->next;
			} else {
				tmpdevnode->next = deviceNode;
				break;
			}
		}
	} else {
		GlobalDeviceList = deviceNode;
	}
	/*Notify New Device Added */
	SampleUtil_StateUpdate(NULL,NULL,deviceNode->device.UDN, DEVICE_ADDED);
	
	for (service = 0; service < ServiceNum; service++) {
		if (serviceId[service])
			free(serviceId[service]);
		if (controlURL[service])
			free(controlURL[service]);
		if (eventURL[service])
			free(eventURL[service]);
	}
    return UPNP_E_SUCCESS;
}    

int TvCtrlPointAddDeviceDu(IXML_Document * DescDoc, const char *UDN,
    struct TvDeviceNode **device, const char * location,int expires)
{
	int ret = UPNP_E_SUCCESS;
	char *friendlyName = NULL;
	struct TvDeviceNode *deviceNode = NULL;
    char* deviceType = SampleUtil_GetFirstDocumentItem(DescDoc, "deviceType");
	friendlyName = SampleUtil_GetFirstDocumentItem(DescDoc, "friendlyName");

    SampleUtilPrintf(UPNP_INFO, "friendlyName:%s\n", friendlyName);

    /* Create a new device node */
    deviceNode = (struct TvDeviceNode *)malloc(sizeof(struct TvDeviceNode));
    memset(deviceNode, 0, sizeof(struct TvDeviceNode));
    strcpy(deviceNode->device.UDN, UDN);
    strcpy(deviceNode->device.DeviceType, deviceType);
    strcpy(deviceNode->device.DescDocURL, location);
    strcpy(deviceNode->device.FriendlyName, friendlyName);
    deviceNode->device.AdvrTimeOut = expires;
	*device = deviceNode;
	if (friendlyName)
		free(friendlyName);
	if (deviceType)
		free(deviceType);

    return ret;
}

int TvCtrlPointAddDeviceDuSrv(IXML_Document * DescDoc,
    struct TvDeviceNode *deviceNode, const char * location)
{
	SampleUtilPrint("Found Du device\n");
	struct TvDeviceNode *tmpdevnode;
	char *serviceId[DU_SERVICE_SERVCOUNT] = { 0 };
	char *eventURL[DU_SERVICE_SERVCOUNT] = { 0 };
	char *controlURL[DU_SERVICE_SERVCOUNT] = { 0 };
	Upnp_SID eventSID[DU_SERVICE_SERVCOUNT] = { 0 };
	int TimeOut[DU_SERVICE_SERVCOUNT] = { default_timeout, default_timeout , default_timeout};
	int service;
	int ret = 1;
	int var;
    const char **ServiceType = DuServiceType; 
    char *VarCount = DuVarCount;
    int ServiceNum = DU_SERVICE_SERVCOUNT;
	for (service = 0; service < ServiceNum; service++) {
		if (SampleUtil_FindAndParseService(DescDoc, location, ServiceType[service],
				&serviceId[service], &eventURL[service], &controlURL[service])) {
			SampleUtilPrint("Subscribing to EventURL[%d]: %s\n", service, eventURL[service]);
			ret = UpnpSubscribe(ctrlpt_handle, eventURL[service], &TimeOut[service],eventSID[service]);
			if (ret == UPNP_E_SUCCESS) {
				SampleUtilPrint("Subscribed to EventURL with SID=%s\n",	eventSID[service]);
			} else {
				SampleUtilPrintf(UPNP_ERROR,"Subscribing to EventURL %d\n", ret);
				strcpy(eventSID[service], "");
			}
		} else {
			SampleUtilPrintf(UPNP_ERROR,"Could not find Service: %s\n", ServiceType[service]);
            char *buf = ixmlDocumenttoString(DescDoc);
            SampleUtilPrintf(UPNP_ERROR, "%s", buf);
    		ixmlFreeDOMString(buf);
		}
	}

	for (service = 0; service < ServiceNum; service++) {
		if (serviceId[service] == NULL) {
			/* not found */
			continue;
		}
		strcpy(deviceNode->device.TvService[service].ServiceId,	serviceId[service]);
		strcpy(deviceNode->device.TvService[service].ServiceType, ServiceType[service]);
		strcpy(deviceNode->device.TvService[service].ControlURL, controlURL[service]);
		strcpy(deviceNode->device.TvService[service].EventURL, eventURL[service]);
		strcpy(deviceNode->device.TvService[service].SID, eventSID[service]);
		for (var = 0; var < VarCount[service]; var++) {
			deviceNode->device.TvService[service].VariableStrVal[var] =	(char *)malloc(TV_MAX_VAL_LEN);
			strcpy(deviceNode->device.TvService[service].VariableStrVal[var],"");
		}
	}
	deviceNode->next = NULL;
	/* Insert the new device node in the list */
	if ((tmpdevnode = GlobalDeviceList)) {
		while (tmpdevnode) {
			if (tmpdevnode->next) {
				tmpdevnode = tmpdevnode->next;
			} else {
				tmpdevnode->next = deviceNode;
				break;
			}
		}
	} else {
		GlobalDeviceList = deviceNode;
	}
	/*Notify New Device Added */
	SampleUtil_StateUpdate(NULL,NULL,deviceNode->device.UDN, DEVICE_ADDED);
	
	for (service = 0; service < ServiceNum; service++) {
		if (serviceId[service])
			free(serviceId[service]);
		if (controlURL[service])
			free(controlURL[service]);
		if (eventURL[service])
			free(eventURL[service]);
	}
    return UPNP_E_SUCCESS;
}    

int DeviceIsExist(const char *UDN, int expires)
{
	int found = 0;
	struct TvDeviceNode *tmpdevnode;
	/* Check if this device is already in the list */
	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		if (strcmp(tmpdevnode->device.UDN, UDN) == 0) {
			found = 1;
			break;
		}
		tmpdevnode = tmpdevnode->next;
	}
	if (found) {
		/* The device is already there, so just update  */
		/* the advertisement timeout field */
		tmpdevnode->device.AdvrTimeOut = expires;
	}
	return found;
}
/********************************************************************************
 * TvCtrlPointAddDevice
 *
 * Description:
 *       If the device is not already included in the global device list,
 *       add it.  Otherwise, update its advertisement expiration timeout.
 *
 * Parameters:
 *   DescDoc -- The description document for the device
 *   location -- The location of the description document URL
 *   expires -- The expiration time for this advertisement
 *
 ********************************************************************************/
int TvCtrlPointAddDevice(
	IXML_Document *DescDoc, const char *location, int expires)
{
	struct TvDeviceNode *deviceNode = NULL;
	char *deviceType = NULL;
	char *UDN = NULL;
	int ret = 1;

	ithread_mutex_lock(&DeviceListMutex);

	/* Read key elements from description document */
	UDN = SampleUtil_GetFirstDocumentItem(DescDoc, "UDN");
	deviceType = SampleUtil_GetFirstDocumentItem(DescDoc, "deviceType");
	if (UDN == NULL || deviceType == NULL) {
        char *buf = ixmlDocumenttoString(DescDoc);
        SampleUtilPrintf(UPNP_ERROR, "%s", buf);
		ixmlFreeDOMString(buf);
    }

	if (strcmp(deviceType, TvDeviceType) == 0) {
		if (!DeviceIsExist(UDN, expires)) {
			ret = TvCtrlPointAddDeviceTv(DescDoc, UDN, &deviceNode, location, expires);
            if (ret == UPNP_E_SUCCESS) {
                TvCtrlPointAddDeviceTvSrv(DescDoc, deviceNode, location);                
            } else {
                if (deviceNode) free(deviceNode);
            }
		}
    } else if (strcmp(deviceType, DuDeviceType) == 0){
        if (!DeviceIsExist(UDN, expires)) {
			ret = TvCtrlPointAddDeviceDu(DescDoc, UDN, &deviceNode, location, expires);
            if (ret == UPNP_E_SUCCESS) {
                TvCtrlPointAddDeviceDuSrv(DescDoc, deviceNode, location);                
            } else {
                if (deviceNode) free(deviceNode);
            }
        }
    } else {
        SampleUtilPrintf(UPNP_ERROR, "UDN:%s, deviceType:%s no supported", UDN, deviceType);
    }

	ithread_mutex_unlock(&DeviceListMutex);

	if (deviceType)
		free(deviceType);
	if (UDN)
		free(UDN);
    return ret;
}

char *GetDeviceType(char *UDN)
{
    char *devType = NULL;

	struct TvDeviceNode *tmpdevnode;
	/* Check if this device is already in the list */
	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		if (strcmp(tmpdevnode->device.UDN, UDN) == 0) {
			devType = tmpdevnode->device.DeviceType;
			break;
		}
		tmpdevnode = tmpdevnode->next;
	}
    return devType;
}

int GetServiceTypeByDev(char *dev, char*SrvType[], int *SrvTypeNum)
{
    char *devType = GetDeviceType(dev);
    if (devType && strncmp(DuDeviceType, devType, strlen(devType)) == 0) {
        memcpy(SrvType, DuServiceType, sizeof(char*) * (*SrvTypeNum));
        *SrvTypeNum = DU_SERVICE_SERVCOUNT;
    } else {
        memcpy(SrvType, TvServiceType, sizeof(char*) * (*SrvTypeNum));
        *SrvTypeNum = TV_SERVICE_SERVCOUNT;
    }
    return 0;
}

int GetServiceVarByDevType(char *devType, char*SrvName[], int *SrvNameNum,
    char **VarCount, char(*VarName)[DU_MAXVARS])
{
    if (devType && strncmp(DuDeviceType, devType, strlen(devType)) == 0) {
        *VarCount = DuVarCount;
        memcpy(VarName, DuVarName, sizeof(char*) * DU_MAXVARS * (*SrvNameNum));
        memcpy(SrvName, DuServiceName, sizeof(char*) * (*SrvNameNum));
        *SrvNameNum = DU_SERVICE_SERVCOUNT;
    } else {
        *VarCount = TvVarCount;
        memcpy(VarName, TvVarName, sizeof(char*) * DU_MAXVARS * (*SrvNameNum));
        memcpy(SrvName, TvServiceName, sizeof(char*) * (*SrvNameNum));
        *SrvNameNum = TV_SERVICE_SERVCOUNT;
    }
    return 0;
}

int GetServiceVar(char *UDN, char*SrvName[], int *SrvNameNum,
    char **VarCount, char (*VarName)[DU_MAXVARS])
{
    char *devType = GetDeviceType(UDN);
    return GetServiceVarByDevType(devType, SrvName, SrvNameNum,
        VarCount, VarName);
}

void TvGetValueByVarName(char *UDN, IXML_Element *property, const char *VarName, char *State)
{
	IXML_NodeList *variables;
	long unsigned int length1;
	IXML_Element *variable;
	char *tmpstate = NULL;

    variables = ixmlElement_getElementsByTagName(property, VarName);
    /* If a match is found, extract
     * the value, and update the state table */
    if (variables == NULL) { 
        char *xmlbuff = ixmlPrintNode(property ? &property->n : NULL);
        if (xmlbuff) free(xmlbuff);
        return;
    }
    length1 = ixmlNodeList_length(variables);
    if (length1 == NULL) {
        return;
    }
    variable = (IXML_Element *)ixmlNodeList_item(variables, 0);
    tmpstate = SampleUtil_GetElementValue(variable);
    if (tmpstate) {
        strcpy(State, tmpstate);
        SampleUtilPrint("Variable:%s Value:'%s'\n", VarName, State);
        free(tmpstate);
        tmpstate = NULL;
    }

    ixmlNodeList_free(variables);
    variables = NULL;
    return;
}

int DuGetValueByVarName(char *UDN, IXML_Element *property, const char *VarName, char *State)
{
	IXML_NodeList *variables;
	long unsigned int length1;
	IXML_Element *variable;
    IXML_Element *LastChange;
    IXML_Element *Event;
    IXML_Element *InstanceID;
	char *tmpstate = NULL;

    variables = ixmlElement_getElementsByTagName(property, VarName);
    /* If a match is found, extract
     * the value, and update the state table */
    if (variables == NULL) { 
        return -1;
    }
    length1 = ixmlNodeList_length(variables);
    if (length1 == 0) {
        return -2;
    }
    variable = (IXML_Element *)ixmlNodeList_item(variables, 0);
    tmpstate = SampleUtil_GetAttrValue(variable);
    if (tmpstate) {
        strcpy(State, tmpstate);
        SampleUtilPrint("Variable %d:%s Value:'%s'\n", length1, VarName, State);
        free(tmpstate);
        tmpstate = NULL;
    } else {
        SampleUtilPrintf(UPNP_ERROR, "Variable %d:%s\n", length1, VarName);
    }

    ixmlNodeList_free(variables);
    variables = NULL;
    return 0;
}

void TvStateUpdateProperty(char *UDN, int Service, IXML_Element *property, char **State)
{
	int j;
    int SrvNameCnt = DU_SERVICE_SERVCOUNT;
    char *SrvName[DU_SERVICE_SERVCOUNT];
	char *VarName[DU_SERVICE_SERVCOUNT][DU_MAXVARS] = { 0 };
    char *VarCount = NULL;

    GetServiceVar(UDN, SrvName, &SrvNameCnt, &VarCount, VarName);
    char *devType = GetDeviceType(UDN);

    /* For each variable name in the state table,
     * check if this is a corresponding property change */
    for (j = 0; j < VarCount[Service]; j++) {
        if (devType && strncmp(DuDeviceType, devType, strlen(devType)) == 0 &&
            Service != 1) {
            DuGetValueByVarName(UDN, property, VarName[Service][j], State[j]);
        } else {
            TvGetValueByVarName(UDN, property, VarName[Service][j], State[j]);
        }
    }
    return ;
}

IXML_NodeList *TvStateUpdateGetProperty(IXML_NodeList *propertys)
{
	IXML_NodeList *ps = NULL;
    IXML_NodeList *props = NULL;
	IXML_Element *LastChange = NULL;
	if (propertys != NULL) {
		int len = ixmlNodeList_length(propertys);
		LastChange = ixmlNode_getFirstChild(ixmlNodeList_item(propertys, 0));
	}

    if (LastChange != NULL && strncmp(LastChange->n.nodeName, "LastChange", strlen("LastChange")) == 0){
        IXML_Element *Event = ixmlNode_getFirstChild(LastChange);
#if 1
		char *xmlbuff = ixmlPrintDocument(Event);
        if (xmlbuff) {
            SampleUtil_Print(xmlbuff);
            free(xmlbuff);
        }
#endif
        // IXML_Element *InstanceID = ixmlNode_getFirstChild(Event);
        IXML_Document *doc = ixmlParseBuffer(ixmlNode_getNodeValue(Event));
		ps = ixmlDocument_getElementsByTagName(doc, "InstanceID");
        IXML_Element *EventNew = ixmlNode_getFirstChild(&doc->n);
        IXML_Element *InstanceID = ixmlNode_getFirstChild(EventNew);
        props = ixmlNode_getChildNodes(InstanceID);
        ixmlNodeList_free(ps);
    }
    return props;
}

void TvStateUpdate(char *UDN, int Service, IXML_Document *ChangedVariables, char **State)
{
	IXML_NodeList *properties;
    IXML_NodeList *props;
	IXML_Element *property;
	long unsigned int length;
	long unsigned int i;

	SampleUtilPrint("Tv State Update (service %d):\n", Service);
	/* Find all of the e:property tags in the document */
	properties = ixmlDocument_getElementsByTagName(ChangedVariables, "e:property");
	if (properties == NULL) {
        return;
    }
#if 1
	char *xmlbuff = ixmlPrintDocument(ChangedVariables);
	if (xmlbuff) {
		SampleUtil_Print(xmlbuff);
		free(xmlbuff);
	}
#endif
    props = TvStateUpdateGetProperty(properties);
    if (props != NULL) {
        ixmlNodeList_free(properties);
        properties = props;
    }
	length = ixmlNodeList_length(properties);
	for (i = 0; i < length; i++) {
		/* Loop through each property change found */
		property = (IXML_Element *)ixmlNodeList_item(properties, i);
		TvStateUpdateProperty(UDN, Service, property, State);
	}
	ixmlNodeList_free(properties);

	return;
}

/********************************************************************************
 * TvCtrlPointHandleEvent
 *
 * Description:
 *       Handle a UPnP event that was received.  Process the event and update
 *       the appropriate service state table.
 *
 * Parameters:
 *   sid -- The subscription id for the event
 *   eventkey -- The eventkey number for the event
 *   changes -- The DOM document representing the changes
 *
 ********************************************************************************/
void TvCtrlPointHandleEvent(
	const char *sid, int evntkey, IXML_Document *changes)
{
	struct TvDeviceNode *tmpdevnode;
	int service;

	ithread_mutex_lock(&DeviceListMutex);

	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		for (service = 0; service < DU_SERVICE_SERVCOUNT; ++service) {
			if (strcmp(tmpdevnode->device.TvService[service].SID, sid) == 0) {
				SampleUtilPrint("Received Tv %s Event: %d for SID %s\n",
					TvServiceName[service],	evntkey, sid);
                SampleUtilPrint("Received Du %s Event: %d for SID %s\n",
					DuServiceName[service],	evntkey, sid);
				TvStateUpdate(tmpdevnode->device.UDN, service, changes,
					(char **)&tmpdevnode->device.TvService[service].VariableStrVal);
				break;
			}
		}
		tmpdevnode = tmpdevnode->next;
	}

	ithread_mutex_unlock(&DeviceListMutex);
}

/********************************************************************************
 * TvCtrlPointHandleSubscribeUpdate
 *
 * Description:
 *       Handle a UPnP subscription update that was received.  Find the
 *       service the update belongs to, and update its subscription
 *       timeout.
 *
 * Parameters:
 *   eventURL -- The event URL for the subscription
 *   sid -- The subscription id for the subscription
 *   timeout  -- The new timeout for the subscription
 *
 ********************************************************************************/
void TvCtrlPointHandleSubscribeUpdate(
	const char *eventURL, const Upnp_SID sid, int timeout)
{
	struct TvDeviceNode *tmpdevnode;
	int service;
	(void)timeout;

	ithread_mutex_lock(&DeviceListMutex);

	tmpdevnode = GlobalDeviceList;
	while (tmpdevnode) {
		for (service = 0; service < DU_SERVICE_SERVCOUNT; service++) {
			if (strcmp(tmpdevnode->device.TvService[service].EventURL, eventURL) == 0) {
				SampleUtilPrint("Received Tv %s Event Renewal for eventURL %s\n",
					TvServiceName[service],	eventURL);
				SampleUtilPrint("Received Du %s Event Renewal for eventURL %s\n",
					DuServiceName[service],	eventURL);
				strcpy(tmpdevnode->device.TvService[service].SID, sid);
				break;
			}
		}

		tmpdevnode = tmpdevnode->next;
	}

	ithread_mutex_unlock(&DeviceListMutex);

	return;
}

void TvCtrlPointHandleGetVar(
	const char *controlURL, const char *varName, const DOMString varValue)
{

	struct TvDeviceNode *devnode;
	int service;

	ithread_mutex_lock(&DeviceListMutex);

	devnode = GlobalDeviceList;
	while (devnode) {
		for (service = 0; service < TV_SERVICE_SERVCOUNT; service++) {
			if (strcmp(devnode->device.TvService[service].ControlURL, controlURL) == 0) {
				SampleUtil_StateUpdate(varName,	varValue, devnode->device.UDN, GET_VAR_COMPLETE);
				break;
			}
		}
		devnode = devnode->next;
	}

	ithread_mutex_unlock(&DeviceListMutex);
}

void TvCtrlPointHandleGetVarRequest(const void *Event)
{
    UpnpStateVarComplete *sv_event = (UpnpStateVarComplete *)Event;
    int errCode = UpnpStateVarComplete_get_ErrCode(sv_event);
    if (errCode != UPNP_E_SUCCESS) {
        SampleUtilPrintf(UPNP_ERROR, "UpnpStateVarComplete_get_ErrCode:%d\n", errCode);
    } else {
        TvCtrlPointHandleGetVar(
            UpnpString_get_String(UpnpStateVarComplete_get_CtrlUrl(sv_event)),
            UpnpString_get_String(UpnpStateVarComplete_get_StateVarName(sv_event)),
            UpnpStateVarComplete_get_CurrentVal(sv_event));
    }
    return ;
}

void TvCtrlPointHandleDiscovery(const void *Event)
{
    const UpnpDiscovery *d_event = (UpnpDiscovery *)Event;
    IXML_Document *DescDoc = NULL;
    const char *location = NULL;
    int errCode = UpnpDiscovery_get_ErrCode(d_event);
    if (errCode != UPNP_E_SUCCESS) {
        SampleUtilPrintf(UPNP_ERROR, "UpnpDiscovery_get_ErrCode %d\n", errCode);
    }
    
    location = UpnpString_get_String(UpnpDiscovery_get_Location(d_event));
    errCode = UpnpDownloadXmlDoc(location, &DescDoc);
    if (errCode != UPNP_E_SUCCESS) {
        SampleUtilPrintf(UPNP_ERROR, "UpnpDownloadXmlDoc from %s, error:%d\n", location, errCode);
    } else {
        int ret = TvCtrlPointAddDevice(DescDoc, location, UpnpDiscovery_get_Expires(d_event));
        if (ret == UPNP_E_SUCCESS) {
            TvCtrlPointPrintList();
        }
    }
    if (DescDoc) {
        ixmlDocument_free(DescDoc);
    }
    return ;
}

void TvCtrlPointHandleActionRequest(const void *Event)
{
    UpnpActionComplete *a_event = (UpnpActionComplete *)Event;
    int errCode = UpnpActionComplete_get_ErrCode(a_event);
    if (errCode != UPNP_E_SUCCESS) {
        SampleUtilPrintf(UPNP_ERROR, "UpnpActionComplete_get_ErrCode:%d\n", errCode);
    }
    /* No need for any processing here, just print out results.
     * Service state table updates are handled by events. */
    return ;
}

void TvCtrlPointHandleSubscribeRequest(const void *Event)
{
	int errCode = 0;
    UpnpEventSubscribe *es_event = (UpnpEventSubscribe *)Event;
    
    errCode = UpnpEventSubscribe_get_ErrCode(es_event);
    if (errCode != UPNP_E_SUCCESS) {
        SampleUtilPrintf(UPNP_ERROR, "UpnpEventSubscribe_get_ErrCode:%d\n", errCode);
    } else {
        TvCtrlPointHandleSubscribeUpdate(
            UpnpString_get_String(UpnpEventSubscribe_get_PublisherUrl(es_event)),
            UpnpString_get_String(UpnpEventSubscribe_get_SID(es_event)),
            UpnpEventSubscribe_get_TimeOut(es_event));
    }
    return ;
}

void TvCtrlPointHandleAdvert(const void *Event)
{
    UpnpDiscovery *d_event = (UpnpDiscovery *)Event;
    int errCode = UpnpDiscovery_get_ErrCode(d_event);
    const char *deviceId = UpnpString_get_String(UpnpDiscovery_get_DeviceID(d_event));
    if (errCode != UPNP_E_SUCCESS) {
        SampleUtilPrintf(UPNP_ERROR, "UpnpDiscovery_get_DeviceID:%d\n", errCode);
    }
    SampleUtilPrintf(UPNP_INFO, "Received ByeBye for Device:%s\n", deviceId);
    TvCtrlPointRemoveDevice(deviceId);
    TvCtrlPointPrintList();
    return ;
}

/********************************************************************************
 * TvCtrlPointCallbackEventHandler
 *
 * Description:
 *       The callback handler registered with the SDK while registering
 *       the control point.  Detects the type of callback, and passes the
 *       request on to the appropriate function.
 *
 * Parameters:
 *   EventType -- The type of callback event
 *   Event -- Data structure containing event data
 *   Cookie -- Optional data specified during callback registration
 *
 ********************************************************************************/
int TvCtrlPointCallbackEventHandler(
	Upnp_EventType EventType, const void *Event, void *Cookie)
{
	int errCode = 0;
	(void)Cookie;

	SampleUtil_PrintEvent(EventType, Event);
	switch (EventType) {
	/* SSDP Stuff */
	case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
	case UPNP_DISCOVERY_SEARCH_RESULT: {
        TvCtrlPointHandleDiscovery(Event);
		break;
	}
	case UPNP_DISCOVERY_SEARCH_TIMEOUT:
		/* Nothing to do here... */
		break;
	case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE: {
        TvCtrlPointHandleAdvert(Event);
		break;
	}
	/* SOAP Stuff */
	case UPNP_CONTROL_ACTION_COMPLETE: {
        TvCtrlPointHandleActionRequest(Event);
		break;
	}
	case UPNP_CONTROL_GET_VAR_COMPLETE: {
        TvCtrlPointHandleGetVarRequest(Event);
		break;
	}
	/* GENA Stuff */
	case UPNP_EVENT_RECEIVED: {
		UpnpEvent *e_event = (UpnpEvent *)Event;
		TvCtrlPointHandleEvent(UpnpEvent_get_SID_cstr(e_event),
			UpnpEvent_get_EventKey(e_event),
			UpnpEvent_get_ChangedVariables(e_event));
		break;
	}
	case UPNP_EVENT_SUBSCRIBE_COMPLETE:
	case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
	case UPNP_EVENT_RENEWAL_COMPLETE: {
        TvCtrlPointHandleSubscribeRequest(Event);
		break;
	}
	case UPNP_EVENT_AUTORENEWAL_FAILED:
	case UPNP_EVENT_SUBSCRIPTION_EXPIRED: {
		UpnpEventSubscribe *es_event = (UpnpEventSubscribe *)Event;
		int TimeOut = default_timeout;
		Upnp_SID newSID;
        char *PublisherUrl = UpnpString_get_String(UpnpEventSubscribe_get_PublisherUrl(es_event));
		errCode = UpnpSubscribe(ctrlpt_handle, PublisherUrl, &TimeOut, newSID);
		if (errCode == UPNP_E_SUCCESS) {
			SampleUtilPrintf(UPNP_INFO, "UpnpSubscribe SID=%s\n", newSID);
			TvCtrlPointHandleSubscribeUpdate(PublisherUrl, newSID, TimeOut);
		} else {
			SampleUtilPrintf(UPNP_ERROR, "UpnpSubscribe SID=%s, err:%d\n", newSID, errCode);
		}
		break;
	}
	/* ignore these cases, since this is not a device */
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
	case UPNP_CONTROL_GET_VAR_REQUEST:
	case UPNP_CONTROL_ACTION_REQUEST:
		break;
	}

	return 0;
}

void TvCtrlPointVerifyTimeouts(int incr)
{
	struct TvDeviceNode *prevdevnode;
	struct TvDeviceNode *curdevnode;
	int ret;

	ithread_mutex_lock(&DeviceListMutex);

	prevdevnode = NULL;
	curdevnode = GlobalDeviceList;
	while (curdevnode) {
		curdevnode->device.AdvrTimeOut -= incr;
		/*SampleUtilPrint("Advertisement Timeout: %d\n",
		 * curdevnode->device.AdvrTimeOut); */
		if (curdevnode->device.AdvrTimeOut <= 0) {
			/* This advertisement has expired, so we should remove
			 * the device from the list */
			if (GlobalDeviceList == curdevnode)
				GlobalDeviceList = curdevnode->next;
			else
				prevdevnode->next = curdevnode->next;
			TvCtrlPointDeleteNode(curdevnode);
			if (prevdevnode)
				curdevnode = prevdevnode->next;
			else
				curdevnode = GlobalDeviceList;
		} else {
			if (curdevnode->device.AdvrTimeOut < 2 * incr) {
				/* This advertisement is about to expire, so
				 * send out a search request for this device
				 * UDN to try to renew */
				ret = UpnpSearchAsync(ctrlpt_handle, incr, curdevnode->device.UDN, NULL);
				if (ret != UPNP_E_SUCCESS)
					SampleUtilPrintf(UPNP_ERROR, "search request for Device UDN:%s err:%d\n",
						curdevnode->device.UDN,	ret);
			}
			prevdevnode = curdevnode;
			curdevnode = curdevnode->next;
		}
	}

	ithread_mutex_unlock(&DeviceListMutex);
}

/*!
 * \brief Function that runs in its own thread and monitors advertisement
 * and subscription timeouts for devices in the global device list.
 */
static int TvCtrlPointTimerLoopRun = 1;
void *TvCtrlPointTimerLoop(void *args)
{
	/* how often to verify the timeouts, in seconds */
	int incr = 30;
	(void)args;

	while (TvCtrlPointTimerLoopRun) {
		isleep((unsigned int)incr);
		TvCtrlPointVerifyTimeouts(incr);
	}

	return NULL;
}

/*!
 * \brief Call this function to initialize the UPnP library and start the TV
 * Control Point.  This function creates a timer thread and provides a
 * callback handler to process any UPnP events that are received.
 *
 * \return TV_SUCCESS if everything went well, else TV_ERROR.
 */
int TvCtrlPointStart(print_string printFunctionPtr,
	state_update updateFunctionPtr,
	int combo)
{
	ithread_t timer_thread;
	int rc;
	unsigned short port = 0;
    char *ip_address = NULL;
	char *if_name = "share";

	SampleUtil_Initialize(printFunctionPtr);
	SampleUtil_RegisterUpdateFunction(updateFunctionPtr);

	ithread_mutex_init(&DeviceListMutex, 0);

	rc = UpnpInit2(if_name, port);
	SampleUtil_Print("Init UPnP Sdk with if_name:%s,port:%u\n",
		if_name ? if_name : "{NULL}",	port);
	if (rc != UPNP_E_SUCCESS) {
		SampleUtil_Print("UpnpInit2() Error: %d\n", rc);
		if (!combo) {
			UpnpFinish();
			return TV_ERROR;
		}
	}
	if (!ip_address) {
		ip_address = UpnpGetServerIpAddress();
	}
	if (!port) {
		port = UpnpGetServerPort();
	}

	SampleUtil_Print("UPnP Init ip:%s,port:%u\n",
		ip_address ? ip_address : "{NULL}",	port);
	rc = UpnpRegisterClient(TvCtrlPointCallbackEventHandler,
		&ctrlpt_handle,
		&ctrlpt_handle);
	if (rc != UPNP_E_SUCCESS) {
		SampleUtilPrintf(UPNP_ERROR, "UpnpRegisterClient Error: %d\n", rc);
		UpnpFinish();

		return TV_ERROR;
	}

	SampleUtilPrint("Ctrl Point Registered\n");

	TvCtrlPointRefresh();

	/* start a timer thread */
	ithread_create(&timer_thread, NULL, TvCtrlPointTimerLoop, NULL);
	ithread_detach(timer_thread);

	return TV_SUCCESS;
}

int TvCtrlPointStop(void)
{
	TvCtrlPointTimerLoopRun = 0;
	TvCtrlPointRemoveAll();
	UpnpUnRegisterClient(ctrlpt_handle);
	UpnpFinish();
	SampleUtil_Finish();

	return TV_SUCCESS;
}

void TvCtrlPointPrintLongHelp(void)
{
	SampleUtil_Print(
		"\n"
		"******************************\n"
		"* TV Control Point Help Info *\n"
		"******************************\n"
		"\n"
		"This sample control point application automatically searches\n"
		"for and subscribes to the services of television device "
		"emulator\n"
		"devices, described in the tvdevicedesc.xml description "
		"document.\n"
		"It also registers itself as a tv device.\n"
		"\n"
		"Commands:\n"
		"  Help\n"
		"       Print this help info.\n"
		"  ListDev\n"
		"       Print the current list of TV Device Emulators that "
		"this\n"
		"         control point is aware of.  Each device is preceded "
		"by a\n"
		"         device number which corresponds to the devnum "
		"argument of\n"
		"         commands listed below.\n"
		"  Refresh\n"
		"       Delete all of the devices from the device list and "
		"issue new\n"
		"         search request to rebuild the list from scratch.\n"
		"  PrintDev       <devnum>\n"
		"       Print the state table for the device <devnum>.\n"
		"         e.g., 'PrintDev 1' prints the state table for the "
		"first\n"
		"         device in the device list.\n"
		"  PowerOn        <devnum>\n"
		"       Sends the PowerOn action to the Control Service of\n"
		"         device <devnum>.\n"
		"  PowerOff       <devnum>\n"
		"       Sends the PowerOff action to the Control Service of\n"
		"         device <devnum>.\n"
		"  SetChannel     <devnum> <channel>\n"
		"       Sends the SetChannel action to the Control Service of\n"
		"         device <devnum>, requesting the channel to be "
		"changed\n"
		"         to <channel>.\n"
		"  SetVolume      <devnum> <volume>\n"
		"       Sends the SetVolume action to the Control Service of\n"
		"         device <devnum>, requesting the volume to be "
		"changed\n"
		"         to <volume>.\n"
		"  SetColor       <devnum> <color>\n"
		"       Sends the SetColor action to the Control Service of\n"
		"         device <devnum>, requesting the color to be changed\n"
		"         to <color>.\n"
		"  SetTint        <devnum> <tint>\n"
		"       Sends the SetTint action to the Control Service of\n"
		"         device <devnum>, requesting the tint to be changed\n"
		"         to <tint>.\n"
		"  SetContrast    <devnum> <contrast>\n"
		"       Sends the SetContrast action to the Control Service "
		"of\n"
		"         device <devnum>, requesting the contrast to be "
		"changed\n"
		"         to <contrast>.\n"
		"  SetBrightness  <devnum> <brightness>\n"
		"       Sends the SetBrightness action to the Control Service "
		"of\n"
		"         device <devnum>, requesting the brightness to be "
		"changed\n"
		"         to <brightness>.\n"
		"  CtrlAction     <devnum> <action>\n"
		"       Sends an action request specified by the string "
		"<action>\n"
		"         to the Control Service of device <devnum>.  This "
		"command\n"
		"         only works for actions that have no arguments.\n"
		"         (e.g., \"CtrlAction 1 IncreaseChannel\")\n"
		"  PictAction     <devnum> <action>\n"
		"       Sends an action request specified by the string "
		"<action>\n"
		"         to the Picture Service of device <devnum>.  This "
		"command\n"
		"         only works for actions that have no arguments.\n"
		"         (e.g., \"PictAction 1 DecreaseContrast\")\n"
		"  CtrlGetVar     <devnum> <varname>\n"
		"       Requests the value of a variable specified by the "
		"string <varname>\n"
		"         from the Control Service of device <devnum>.\n"
		"         (e.g., \"CtrlGetVar 1 Volume\")\n"
		"  PictGetVar     <devnum> <action>\n"
		"       Requests the value of a variable specified by the "
		"string <varname>\n"
		"         from the Picture Service of device <devnum>.\n"
		"         (e.g., \"PictGetVar 1 Tint\")\n"
		"  Exit\n"
		"       Exits the control point application.\n");
}

/*! Mappings between command text names, command tag,
 * and required command arguments for command line
 * commands */
static cmdloop cpCmdList[] = {
    {"Help", PRTHELP, 1, ""},
	{"HelpFull", PRTFULLHELP, 1, ""},
	{"ListDev", LSTDEV, 1, ""},
	{"Refresh", REFRESH, 1, ""},
	{"PrintDev", PRTDEV, 2, "<devnum>"},
	{SET_POWER_ON, POWON, 2, "<devnum>"},
	{SET_POWER_OFF, POWOFF, 2, "<devnum>"},
	{SET_CHANNEL, SETCHAN, 3, "<devnum> <channel (int)>"},
	{SET_VOLUME, SETVOL, 3, "<devnum> <volume (int)>"},
	{SET_COLOR, SETCOL, 3, "<devnum> <color (int)>"},
	{SET_TINT, SETTINT, 3, "<devnum> <tint (int)>"},
	{SET_CONTRAST, SETCONT, 3, "<devnum> <contrast (int)>"},
	{SET_BRIGHTNESS, SETBRT, 3, "<devnum> <brightness (int)>"},
	{SET_LOG, SETLOG, 3, "<devnum> <level (int)> <module (int)>"},
	{"CtrlAction", CTRLACTION, 2, "<devnum> <action (string)>"},
	{"PictAction", PICTACTION, 2, "<devnum> <action (string)>"},
	{"CtrlGetVar", CTRLGETVAR, 2, "<devnum> <varname (string)>"},
	{"PictGetVar", PICTGETVAR, 2, "<devnum> <varname (string)>"},
	{"Play", PLAY, 3, "<devnum> <varname (string)>"},
	{"SetAV", SET_AV_TRANSPORT_URI, 3, "<devnum> <varname (string)>"},	
	{"GetPos", GET_POS, 3, "<devnum> <varname (string)>"},		
	{"GetTransInfo", GET_TRANSPORT_INFO, 3, "<devnum> <varname (string)>"},			
	{"Exit", EXITCMD, 1, ""}};

void *TvCtrlPointCommandLoop(void *args)
{
	char cmdline[100];
	char *s;
	(void)args;
	SampleUtil_Print("\n>> ");
	while (1) {
		s = fgets(cmdline, 100, stdin);
		if (!s)
			break;
		TvCtrlPointProcessCommand(cmdline);
		SampleUtil_Print("\n>> ");
	}

	return NULL;
}

int TvCtrlPointProcessCommand(char *cmdline)
{
	char cmd[100] = {0};
	char strarg[100] = {0};
	int arg_val_err = -99999;
    int devnum = arg_val_err;
    int arg1 = arg_val_err;
	int arg2 = arg_val_err;
	int cmdnum = -1;
	int numofcmds = (sizeof cpCmdList) / sizeof(cmdloop);
	int cmdfound = 0;
	int rc;
	int invalidargs = 0;
	int validargs;

	validargs = sscanf(cmdline, "%s %d %d", cmd, &arg1, &arg2);
	for (int i = 0; i < numofcmds; ++i) {
		if (strcasecmp(cmd, cpCmdList[i].str) == 0) {
			cmdnum = cpCmdList[i].cmdnum;
			cmdfound++;
			if (validargs != cpCmdList[i].numargs)
				invalidargs++;
			break;
		}
	}
	if (!cmdfound) {
		SampleUtil_Print("Command not found; try 'Help'\n");
		return TV_SUCCESS;
	}
	if (invalidargs) {
		SampleUtil_Print("Invalid arguments; try 'Help'\n");
		return TV_SUCCESS;
	}
	switch (cmdnum) {
	case PRTHELP:
		TvPrintCommands(sizeof(cpCmdList) / sizeof(cmdloop), cpCmdList);
		break;
	case PRTFULLHELP:
		TvCtrlPointPrintLongHelp();
		break;
	case POWON:
		TvCtrlPointSendPowerOn(arg1);
		break;
	case POWOFF:
		TvCtrlPointSendPowerOff(arg1);
		break;
	case SETCHAN:
		TvCtrlPointSendSetChannel(arg1, arg2);
		break;
	case SETVOL:
		TvCtrlPointSendSetVolume(arg1, arg2);
		break;
	case SETLOG:
		/* re-parse commandline since second arg is string. */
		validargs = sscanf(cmdline, "%s %d %d %d", cmd, &devnum, &arg1, &arg2);
		if (validargs == 4)
			TvCtrlPointSendSetLog(devnum, arg1, arg2);
		else
			invalidargs++;
		break;                
	case SETCOL:
		TvCtrlPointSendSetColor(arg1, arg2);
		break;
	case SETTINT:
		TvCtrlPointSendSetTint(arg1, arg2);
		break;
	case SETCONT:
		TvCtrlPointSendSetContrast(arg1, arg2);
		break;
	case SETBRT:
		TvCtrlPointSendSetBrightness(arg1, arg2);
		break;
	case CTRLACTION:
		/* re-parse commandline since second arg is string. */
		validargs = sscanf(cmdline, "%s %d %s", cmd, &arg1, strarg);
		if (validargs == 3)
			TvCtrlPointSendAction(TV_SERVICE_CONTROL, arg1, strarg, NULL, NULL, 0);
		else
			invalidargs++;
		break;
	case PICTACTION:
		/* re-parse commandline since second arg is string. */
		validargs = sscanf(cmdline, "%s %d %s", cmd, &arg1, strarg);
		if (validargs == 3)
			TvCtrlPointSendAction(TV_SERVICE_PICTURE, arg1, strarg, NULL, NULL, 0);
		else
			invalidargs++;
		break;
	case CTRLGETVAR:
		/* re-parse commandline since second arg is string. */
		validargs = sscanf(cmdline, "%s %d %s", cmd, &arg1, strarg);
		if (validargs == 3)
			TvCtrlPointGetVar(TV_SERVICE_CONTROL, arg1, strarg);
		else
			invalidargs++;
		break;
	case PICTGETVAR:
		/* re-parse commandline since second arg is string. */
		validargs = sscanf(cmdline, "%s %d %s", cmd, &arg1, strarg);
		if (validargs == 3)
			TvCtrlPointGetVar(TV_SERVICE_PICTURE, arg1, strarg);
		else
			invalidargs++;
		break;
	case PRTDEV:
		TvCtrlPointPrintDevice(arg1);
		break;
	case LSTDEV:
		TvCtrlPointPrintList();
		break;
	case REFRESH:
		TvCtrlPointRefresh();
		break;
	case PLAY:
		TvCtrlPointSendPlay(arg1, arg2);
		break;     
    case SET_AV_TRANSPORT_URI:
        TvCtrlPointSendSetAVTransportURI(arg1, arg2);
        break;
    case GET_POS:
        TvCtrlPointSendGetPositionInfo(arg1, arg2);
        break;
    case GET_TRANSPORT_INFO:
        TvCtrlPointSendGetTransportInfo(arg1, arg2);
        break;        
	case EXITCMD:
		rc = TvCtrlPointStop();
		exit(rc);
		break;
	default:
		SampleUtil_Print("Command not implemented; see 'Help'\n");
		break;
	}
	if (invalidargs)
		SampleUtil_Print("Invalid args in command; see 'Help'\n");

	return TV_SUCCESS;
}

/*! @} Control Point Sample Module */

/*! @} UpnpSamples */
