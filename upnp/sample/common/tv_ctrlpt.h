#ifndef UPNP_TV_CTRLPT_H
#define UPNP_TV_CTRLPT_H

/**************************************************************************
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
 **************************************************************************/

/*!
 * \addtogroup UpnpSamples
 *
 * @{
 *
 * \name Contro Point Sample API
 *
 * @{
 *
 * \file
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "sample_util.h"

#include "upnp.h"
#include "UpnpString.h"
#include "upnptools.h"
#include "tv_config.h"
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

#define TV_SUCCESS		0
#define TV_ERROR		(-1)
#define TV_WARNING		1

struct tv_service {
    char ServiceId[NAME_SIZE];
    char ServiceType[NAME_SIZE];
    char *VariableStrVal[DU_MAXVARS];
    char EventURL[NAME_SIZE];
    char ControlURL[NAME_SIZE];
    char SID[NAME_SIZE];
};

struct TvDevice {
    char UDN[NAME_SIZE];
    char DescDocURL[NAME_SIZE];
    char FriendlyName[NAME_SIZE];
    char PresURL[NAME_SIZE];
    char DeviceType[NAME_SIZE];
    int  AdvrTimeOut;
    struct tv_service TvService[DU_SERVICE_SERVCOUNT];
};

struct TvDeviceNode {
    struct TvDevice device;
    struct TvDeviceNode *next;
};

void	TvCtrlPointPrintHelp(void);
int		TvCtrlPointDeleteNode(struct TvDeviceNode *);
int		TvCtrlPointRemoveDevice(const char *);
int		TvCtrlPointRemoveAll(void);
int		TvCtrlPointRefresh(void);

int		TvCtrlPointSendAction(int, int, const char *, const char **, char **, int);
int		TvCtrlPointSendActionNumericArg(int devnum, int service, const char *actionName, const char *paramName, int paramValue);
int		TvCtrlPointSendPowerOn(int devnum);
int		TvCtrlPointSendPowerOff(int devnum);
int		TvCtrlPointSendSetChannel(int, int);
int		TvCtrlPointSendSetVolume(int, int);
int		TvCtrlPointSendSetColor(int, int);
int		TvCtrlPointSendSetTint(int, int);
int		TvCtrlPointSendSetContrast(int, int);
int		TvCtrlPointSendSetBrightness(int, int);

int		TvCtrlPointGetVar(int, int, const char *);
int		TvCtrlPointGetPower(int devnum);
int		TvCtrlPointGetChannel(int);
int		TvCtrlPointGetVolume(int);
int		TvCtrlPointGetColor(int);
int		TvCtrlPointGetTint(int);
int		TvCtrlPointGetContrast(int);
int		TvCtrlPointGetBrightness(int);

int		TvCtrlPointGetDevice(int, struct TvDeviceNode **);
int		TvCtrlPointPrintList(void);
int		TvCtrlPointPrintDevice(int);
int  	TvCtrlPointAddDevice(IXML_Document *, const char *, int); 
void    TvCtrlPointHandleGetVar(const char *, const char *, const DOMString);

/*!
 * \brief Update a Tv state table. Called when an event is received.
 *
 * Note: this function is NOT thread save. It must be called from another
 * function that has locked the global device list.
 **/
void TvStateUpdate(
	/*! [in] The UDN of the parent device. */
	char *UDN,
	/*! [in] The service state table to update. */
	int Service,
	/*! [out] DOM document representing the XML received with the event. */
	IXML_Document *ChangedVariables,
	/*! [out] pointer to the state table for the Tv  service to update. */
	char **State);

void	TvCtrlPointHandleEvent(const char *, int, IXML_Document *); 
void	TvCtrlPointHandleSubscribeUpdate(const char *, const Upnp_SID, int); 
int		TvCtrlPointCallbackEventHandler(Upnp_EventType, const void *, void *);

/*!
 * \brief Checks the advertisement each device in the global device list.
 *
 * If an advertisement expires, the device is removed from the list.
 *
 * If an advertisement is about to expire, a search request is sent for that
 * device.
 */
void TvCtrlPointVerifyTimeouts(
	/*! [in] The increment to subtract from the timeouts each time the
	 * function is called. */
	int incr);

int		TvCtrlPointStart(print_string printFunctionPtr, state_update updateFunctionPtr, int combo);
int		TvCtrlPointStop(void);

/*!
 * \brief Print long help info for this application.
 */
void TvCtrlPointPrintLongHelp(void);



/*!
 * \brief Function that receives commands from the user at the command prompt
 * during the lifetime of the device, and calls the appropriate
 * functions for those commands.
 */
void *TvCtrlPointCommandLoop(void *args);

/*!
 * \brief
 */
int TvCtrlPointProcessCommand(char *cmdline);

#ifdef __cplusplus
};
#endif


/*! @} Device Sample */

/*! @} UpnpSamples */

#endif /* UPNP_TV_CTRLPT_H */
