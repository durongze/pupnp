
/*!
 * \file
 *
 * \brief SSDPResultData object implementation.
 *
 * \author Marcelo Roberto Jimenez
 */

#include "config.h"

#define TEMPLATE_GENERATE_SOURCE
#include "ssdp_ResultData.h"
#include "UpnpDebug.h"

void SSDPResultData_Callback(const SSDPResultData *p)
{
	Upnp_FunPtr callback = SSDPResultData_get_CtrlptCallback(p);
	if (callback == NULL) {
		SsdpPrintf(UPNP_ERROR, "callback is null\n");
		return;
	}
	callback(UPNP_DISCOVERY_SEARCH_RESULT,
		SSDPResultData_get_Param(p),
		SSDPResultData_get_Cookie(p));
}

