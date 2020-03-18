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
 * \file
 */

#define SAMPLE_UTIL_C

#include "sample_util.h"

#include <stdarg.h>
#include <stdio.h>

#define UPNP_HAVE_TOOLS 1
#if !UPNP_HAVE_TOOLS
#error "Need upnptools.h to compile samples ; try ./configure --enable-tools"
#endif

static int initialize_init = 1;
static int initialize_register = 1;

/*! Function pointers to use for displaying formatted strings.
 * Set on Initialization of device. */
print_string gPrintFun = NULL;
state_update gStateUpdateFun = NULL;

/*! mutex to control displaying of events */
ithread_mutex_t display_mutex;

int SampleUtil_Initialize(print_string print_function)
{
	if (initialize_init) {
		ithread_mutexattr_t attr;

		ithread_mutexattr_init(&attr);
		ithread_mutexattr_setkind_np(&attr, ITHREAD_MUTEX_RECURSIVE_NP);
		ithread_mutex_init(&display_mutex, &attr);
		ithread_mutexattr_destroy(&attr);
		/* To shut up valgrind mutex warning. */
		ithread_mutex_lock(&display_mutex);
		gPrintFun = print_function;
		ithread_mutex_unlock(&display_mutex);
		/* Finished initializing. */
		initialize_init = 0;
	}

	return UPNP_E_SUCCESS;
}

int SampleUtil_RegisterUpdateFunction(state_update update_function)
{
	if (initialize_register) {
		gStateUpdateFun = update_function;
		initialize_register = 0;
	}

	return UPNP_E_SUCCESS;
}

int SampleUtil_Finish()
{
	ithread_mutex_destroy(&display_mutex);
	gPrintFun = NULL;
	gStateUpdateFun = NULL;
	initialize_init = 1;
	initialize_register = 1;

	return UPNP_E_SUCCESS;
}

char *SampleUtil_GetElementValue(IXML_Element *element)
{
	IXML_Node *child = ixmlNode_getFirstChild((IXML_Node *)element);
	char *temp = NULL;

	if (child != 0 && ixmlNode_getNodeType(child) == eTEXT_NODE)
		temp = strdup(ixmlNode_getNodeValue(child));

	return temp;
}

IXML_NodeList *SampleUtil_GetFirstServiceList(IXML_Document *doc)
{
	IXML_NodeList *ServiceList = NULL;
	IXML_NodeList *servlistnodelist = NULL;
	IXML_Node *servlistnode = NULL;

	servlistnodelist =
		ixmlDocument_getElementsByTagName(doc, "serviceList");
	if (servlistnodelist && ixmlNodeList_length(servlistnodelist)) {
		/* we only care about the first service list, from the root
		 * device */
		servlistnode = ixmlNodeList_item(servlistnodelist, 0);
		/* create as list of DOM nodes */
		ServiceList = ixmlElement_getElementsByTagName(
			(IXML_Element *)servlistnode, "service");
	}
	if (servlistnodelist)
		ixmlNodeList_free(servlistnodelist);

	return ServiceList;
}

#define OLD_FIND_SERVICE_CODE
#ifdef OLD_FIND_SERVICE_CODE
#else
/*
 * Obtain the service list
 *    n == 0 the first
 *    n == 1 the next in the device list, etc..
 */
static IXML_NodeList *SampleUtil_GetNthServiceList(
	/*! [in] . */
	IXML_Document *doc,
	/*! [in] . */
	unsigned int n)
{
	IXML_NodeList *ServiceList = NULL;
	IXML_NodeList *servlistnodelist = NULL;
	IXML_Node *servlistnode = NULL;

	/*  ixmlDocument_getElementsByTagName()
	 *  Returns a NodeList of all Elements that match the given
	 *  tag name in the order in which they were encountered in a preorder
	 *  traversal of the Document tree.
	 *
	 *  return (NodeList*) A pointer to a NodeList containing the
	 *                      matching items or NULL on an error. 	 */
	SampleUtilPrint("SampleUtil_GetNthServiceList called : n = %d\n", n);
	servlistnodelist =
		ixmlDocument_getElementsByTagName(doc, "serviceList");
	if (servlistnodelist && ixmlNodeList_length(servlistnodelist) &&
		n < ixmlNodeList_length(servlistnodelist)) {
		/* For the first service list (from the root device),
		 * we pass 0 */
		/*servlistnode = ixmlNodeList_item( servlistnodelist, 0 );*/

		/* Retrieves a Node from a NodeList} specified by a
		 *  numerical index.
		 *
		 *  return (Node*) A pointer to a Node or NULL if there was an
		 *                  error. */
		servlistnode = ixmlNodeList_item(servlistnodelist, n);
		if (!servlistnode) {
			/* create as list of DOM nodes */
			ServiceList = ixmlElement_getElementsByTagName(
				(IXML_Element *)servlistnode, "service");
		} else
			SampleUtilPrint("ixmlNodeList_item(nodeList, n) returned NULL\n");
	}
	if (servlistnodelist)
		ixmlNodeList_free(servlistnodelist);

	return ServiceList;
}
#endif

char *SampleUtil_GetFirstDocumentItem(IXML_Document *doc, const char *item)
{
	IXML_NodeList *nodeList = NULL;
	IXML_Node *textNode = NULL;
	IXML_Node *tmpNode = NULL;
	const char *nodeValue = NULL;
	char *ret = NULL;

	nodeList = ixmlDocument_getElementsByTagName(doc, (char *)item);
	if (nodeList) {
		tmpNode = ixmlNodeList_item(nodeList, 0);
		if (tmpNode) {
			textNode = ixmlNode_getFirstChild(tmpNode);
			if (!textNode) {
				SampleUtilPrint("(BUG) "
						 "ixmlNode_getFirstChild("
						 "tmpNode) returned NULL\n");
				ret = strdup("");
				goto epilogue;
			}
			nodeValue = ixmlNode_getNodeValue(textNode);
			if (!nodeValue) {
				SampleUtilPrint(
					" ixmlNode_getNodeValue "
					"returned NULL\n");
				ret = strdup("");
				goto epilogue;
			}
			ret = strdup(nodeValue);
			if (!ret) {
				SampleUtilPrint("Error allocating "
						 "memory for XML Node value\n");
				ret = strdup("");
			}
		} else
			SampleUtilPrint("ixmlNodeList_item(nodeList, "
					 "0) returned NULL\n");
	} else
		SampleUtilPrint("Error finding %s in XML Node\n",
			
			
			item);

epilogue:
	if (nodeList)
		ixmlNodeList_free(nodeList);

	return ret;
}

char *SampleUtil_GetFirstElementItem(IXML_Element *element, const char *item)
{
	IXML_NodeList *nodeList = NULL;
	IXML_Node *textNode = NULL;
	IXML_Node *tmpNode = NULL;
	char *ret = NULL;

	nodeList = ixmlElement_getElementsByTagName(element, (char *)item);
	if (nodeList == NULL) {
		SampleUtilPrint("Error finding %s in XML Node\n",
			
			
			item);
		return NULL;
	}
	tmpNode = ixmlNodeList_item(nodeList, 0);
	if (!tmpNode) {
		SampleUtilPrint("Error finding %s value in XML Node\n",
			
			
			item);
		ixmlNodeList_free(nodeList);
		return NULL;
	}
	textNode = ixmlNode_getFirstChild(tmpNode);
	ret = strdup(ixmlNode_getNodeValue(textNode));
	if (!ret) {
		SampleUtilPrint(
			"Error allocating memory for %s in XML Node\n",
			
			
			item);
		ixmlNodeList_free(nodeList);
		return NULL;
	}
	ixmlNodeList_free(nodeList);

	return ret;
}

char *GetEventName(Upnp_EventType S)
{
    char *event = NULL;
	switch (S) {
	/* Discovery */
	case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
		event = ("UPNP_DISCOVERY_ADVERTISEMENT_ALIVE\n");
		break;
	case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
		event = ("UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE\n");
		break;
	case UPNP_DISCOVERY_SEARCH_RESULT:
		event = ("UPNP_DISCOVERY_SEARCH_RESULT\n");
		break;
	case UPNP_DISCOVERY_SEARCH_TIMEOUT:
		event = ("UPNP_DISCOVERY_SEARCH_TIMEOUT\n");
		break;
	/* SOAP */
	case UPNP_CONTROL_ACTION_REQUEST:
		event = ("UPNP_CONTROL_ACTION_REQUEST\n");
		break;
	case UPNP_CONTROL_ACTION_COMPLETE:
		event = ("UPNP_CONTROL_ACTION_COMPLETE\n");
		break;
	case UPNP_CONTROL_GET_VAR_REQUEST:
		event = ("UPNP_CONTROL_GET_VAR_REQUEST\n");
		break;
	case UPNP_CONTROL_GET_VAR_COMPLETE:
		event = ("UPNP_CONTROL_GET_VAR_COMPLETE\n");
		break;
	/* GENA */
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
		event = ("UPNP_EVENT_SUBSCRIPTION_REQUEST\n");
		break;
	case UPNP_EVENT_RECEIVED:
		event = ("UPNP_EVENT_RECEIVED\n");
		break;
	case UPNP_EVENT_RENEWAL_COMPLETE:
		event = ("UPNP_EVENT_RENEWAL_COMPLETE\n");
		break;
	case UPNP_EVENT_SUBSCRIBE_COMPLETE:
		event = ("UPNP_EVENT_SUBSCRIBE_COMPLETE\n");
		break;
	case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
		event = ("UPNP_EVENT_UNSUBSCRIBE_COMPLETE\n");
		break;
	case UPNP_EVENT_AUTORENEWAL_FAILED:
		event = ("UPNP_EVENT_AUTORENEWAL_FAILED\n");
		break;
	case UPNP_EVENT_SUBSCRIPTION_EXPIRED:
		event = ("UPNP_EVENT_SUBSCRIPTION_EXPIRED\n");
		break;
    default:
        event = ("Unknown Event");
        break;
	}
    return event;
}

void SampleUtil_PrintEventType(Upnp_EventType S)
{
	SampleUtilPrint("%s\n", GetEventName(S));
}

int SampleUtil_PrintEventSearchResult(const void *Event)
{
    UpnpDiscovery *d_event = (UpnpDiscovery *)Event;
    SampleUtilPrint("ErrCode     =  %d\n"
             "Expires     =  %d\n"
             "DeviceId    =  %s\n"
             "DeviceType  =  %s\n"
             "ServiceType =  %s\n"
             "ServiceVer  =  %s\n"
             "Location    =  %s\n"
             "OS          =  %s\n"
             "Date        =  %s\n"
             "Ext         =  %s\n",
        UpnpDiscovery_get_ErrCode(d_event),
        UpnpDiscovery_get_Expires(d_event),
        UpnpString_get_String(UpnpDiscovery_get_DeviceID(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_DeviceType(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_ServiceType(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_ServiceVer(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_Location(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_Os(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_Date(d_event)),
        UpnpString_get_String(UpnpDiscovery_get_Ext(d_event)));
    return 0;
};

int SampleUtil_PrintEventActionRequest(const void *Event)
{
    UpnpActionRequest *a_event = (UpnpActionRequest *)Event;
    IXML_Document *actionRequestDoc = NULL;
    IXML_Document *actionResultDoc = NULL;
    char *xmlbuff = NULL;
    
    SampleUtilPrint("ErrCode     =  %d\n"
             "ErrStr      =  %s\n"
             "ActionName  =  %s\n"
             "UDN         =  %s\n"
             "ServiceID   =  %s\n",
        UpnpActionRequest_get_ErrCode(a_event),
        UpnpString_get_String(UpnpActionRequest_get_ErrStr(a_event)),
        UpnpString_get_String(UpnpActionRequest_get_ActionName(a_event)),
        UpnpString_get_String(UpnpActionRequest_get_DevUDN(a_event)),
        UpnpString_get_String(UpnpActionRequest_get_ServiceID(a_event)));

    actionRequestDoc = UpnpActionRequest_get_ActionRequest(a_event);
    if (actionRequestDoc) {
        xmlbuff = ixmlPrintNode((IXML_Node *)actionRequestDoc);
        if (xmlbuff) {
            SampleUtilPrint("ActRequest  =  %s\n", xmlbuff);
            ixmlFreeDOMString(xmlbuff);
        }
        xmlbuff = NULL;
    } else {
        SampleUtilPrint("ActRequest  =  (null)\n");
    }
    
    actionResultDoc = UpnpActionRequest_get_ActionResult(a_event);
    if (actionResultDoc) {
        xmlbuff = ixmlPrintNode((IXML_Node *)actionResultDoc);
        if (xmlbuff) {
            SampleUtilPrint("ActResult   =  %s\n", xmlbuff);
            ixmlFreeDOMString(xmlbuff);
        }
        xmlbuff = NULL;
    } else {
        SampleUtilPrint("ActResult   =  (null)\n");
    }
    return 0;
}

int SampleUtil_PrintEventActionComplete(const void *Event)
{
    UpnpActionComplete *a_event = (UpnpActionComplete *)Event;
    IXML_Document *actionRequest = NULL;
    IXML_Document *actionResult = NULL;    
    char *xmlbuff = NULL;

    SampleUtilPrint("ErrCode     =  %d\n"
             "CtrlUrl     =  %s\n",
        UpnpActionComplete_get_ErrCode(a_event),
        UpnpString_get_String(UpnpActionComplete_get_CtrlUrl(a_event)));
    
    actionRequest = UpnpActionComplete_get_ActionRequest(a_event);
    if (actionRequest) {
        xmlbuff = ixmlPrintNode((IXML_Node *)actionRequest);
        if (xmlbuff) {
            SampleUtilPrint("ActRequest  =  %s\n", xmlbuff);
            ixmlFreeDOMString(xmlbuff);
        }
        xmlbuff = NULL;
    } else {
        SampleUtilPrint("ActRequest  =  (null)\n");
    }
    
    actionResult = UpnpActionComplete_get_ActionResult(a_event);
    if (actionResult) {
        xmlbuff = ixmlPrintNode((IXML_Node *)actionResult);
        if (xmlbuff) {
            SampleUtilPrint("ActResult   =  %s\n", xmlbuff);
            ixmlFreeDOMString(xmlbuff);
        }
        xmlbuff = NULL;
    } else {
        SampleUtilPrint("ActResult   =  (null)\n");
    }
    return 0;
}

int SampleUtil_PrintEventVarRequest(const void *Event)
{
    UpnpStateVarRequest *sv_event = (UpnpStateVarRequest *)Event;
    
    SampleUtilPrint("ErrCode     =  %d\n"
             "ErrStr      =  %s\n"
             "UDN         =  %s\n"
             "ServiceID   =  %s\n"
             "StateVarName=  %s\n"
             "CurrentVal  =  %s\n",
        UpnpStateVarRequest_get_ErrCode(sv_event),
        UpnpString_get_String(UpnpStateVarRequest_get_ErrStr(sv_event)),
        UpnpString_get_String(UpnpStateVarRequest_get_DevUDN(sv_event)),
        UpnpString_get_String(UpnpStateVarRequest_get_ServiceID(sv_event)),
        UpnpString_get_String(UpnpStateVarRequest_get_StateVarName(sv_event)),
        UpnpStateVarRequest_get_CurrentVal(sv_event));
    return 0;
}

int SampleUtil_PrintEventVarComplete(const void *Event)
{
    UpnpStateVarComplete *sv_event = (UpnpStateVarComplete *)Event;
    
    SampleUtilPrint("ErrCode     =  %d\n"
             "CtrlUrl     =  %s\n"
             "StateVarName=  %s\n"
             "CurrentVal  =  %s\n",
        UpnpStateVarComplete_get_ErrCode(sv_event),
        UpnpString_get_String(UpnpStateVarComplete_get_CtrlUrl(sv_event)),
        UpnpString_get_String(UpnpStateVarComplete_get_StateVarName(sv_event)),
        UpnpStateVarComplete_get_CurrentVal(sv_event));
    return 0;
}

int SampleUtil_PrintEventSubscriptionRequest(const void *Event)
{
    UpnpSubscriptionRequest *sr_event =
        (UpnpSubscriptionRequest *)Event;
    
    SampleUtilPrint("ServiceID   =  %s\n"
             "UDN         =  %s\n"
             "SID         =  %s\n",
        UpnpString_get_String(UpnpSubscriptionRequest_get_ServiceId(sr_event)),
        UpnpString_get_String(UpnpSubscriptionRequest_get_UDN(sr_event)),
        UpnpString_get_String(UpnpSubscriptionRequest_get_SID(sr_event)));
    return 0;
}

int SampleUtil_PrintEventReceived(const void *Event)
{
    UpnpEvent *e_event = (UpnpEvent *)Event;
    char *xmlbuff = NULL;
    
    xmlbuff = ixmlPrintNode(
        (IXML_Node *)UpnpEvent_get_ChangedVariables(e_event));
    SampleUtilPrint("SID         =  %s\n"
             "EventKey    =  %d\n"
             "ChangedVars =  %s\n",
        UpnpString_get_String(UpnpEvent_get_SID(e_event)),
        UpnpEvent_get_EventKey(e_event),
        xmlbuff);
    ixmlFreeDOMString(xmlbuff);
    return 0;
}

int SampleUtil_PrintEventRenewComplete(const void *Event)
{
    UpnpEventSubscribe *es_event = (UpnpEventSubscribe *)Event;
    
    SampleUtilPrint("SID         =  %s\n"
             "ErrCode     =  %d\n"
             "TimeOut     =  %d\n",
        UpnpString_get_String(
            UpnpEventSubscribe_get_SID(es_event)),
        UpnpEventSubscribe_get_ErrCode(es_event),
        UpnpEventSubscribe_get_TimeOut(es_event));
    return 0;
}

int SampleUtil_PrintEventSubscribeComplete(const void *Event)
{
    UpnpEventSubscribe *es_event = (UpnpEventSubscribe *)Event;
    
    SampleUtilPrint("SID         =  %s\n"
             "ErrCode     =  %d\n"
             "PublisherURL=  %s\n"
             "TimeOut     =  %d\n",
        UpnpString_get_String(UpnpEventSubscribe_get_SID(es_event)),
        UpnpEventSubscribe_get_ErrCode(es_event),
        UpnpString_get_String(UpnpEventSubscribe_get_PublisherUrl(es_event)),
        UpnpEventSubscribe_get_TimeOut(es_event));
    return 0;
}

int SampleUtil_PrintEventException(const void *Event)
{
    UpnpEventSubscribe *es_event = (UpnpEventSubscribe *)Event;
    
    SampleUtilPrint("SID         =  %s\n"
             "ErrCode     =  %d\n"
             "PublisherURL=  %s\n"
             "TimeOut     =  %d\n",
        UpnpString_get_String(
            UpnpEventSubscribe_get_SID(es_event)),
        UpnpEventSubscribe_get_ErrCode(es_event),
        UpnpString_get_String(
            UpnpEventSubscribe_get_PublisherUrl(es_event)),
        UpnpEventSubscribe_get_TimeOut(es_event));
    return 0;
}

int SampleUtil_PrintEvent(Upnp_EventType EventType, const void *Event)
{
	ithread_mutex_lock(&display_mutex);

	SampleUtilPrint("<<<<<<<<<<<<<<< Event : %d Start <<<<<<<<<<<<<<<\n", EventType);
	SampleUtil_PrintEventType(EventType);
	switch (EventType) {
	/* SSDP */
	case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
	case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
	case UPNP_DISCOVERY_SEARCH_RESULT:
        SampleUtil_PrintEventSearchResult(Event);
		break;
	case UPNP_DISCOVERY_SEARCH_TIMEOUT:
		/* Nothing to print out here */
		break;
	    /* SOAP */
	case UPNP_CONTROL_ACTION_REQUEST:
        SampleUtil_PrintEventActionRequest(Event);
		break;
	case UPNP_CONTROL_ACTION_COMPLETE:
        SampleUtil_PrintEventActionComplete(Event);
		break;
	case UPNP_CONTROL_GET_VAR_REQUEST:
        SampleUtil_PrintEventVarRequest(Event);
		break;
	case UPNP_CONTROL_GET_VAR_COMPLETE:
        SampleUtil_PrintEventVarComplete(Event);
		break;
	    /* GENA */
	case UPNP_EVENT_SUBSCRIPTION_REQUEST:
        SampleUtil_PrintEventSubscriptionRequest(Event);
		break;
	case UPNP_EVENT_RECEIVED:
        SampleUtil_PrintEventReceived(Event);
		break;
	case UPNP_EVENT_RENEWAL_COMPLETE:
        SampleUtil_PrintEventRenewComplete(Event);
		break;
	case UPNP_EVENT_SUBSCRIBE_COMPLETE:
	case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
        SampleUtil_PrintEventSubscribeComplete(Event);
		break;
	case UPNP_EVENT_AUTORENEWAL_FAILED:
	case UPNP_EVENT_SUBSCRIPTION_EXPIRED:
        SampleUtil_PrintEventException(Event);
		break;
	}
	SampleUtilPrint("<<<<<<<<<<<<<<< Event : %d End <<<<<<<<<<<<<<<\n", EventType);

	ithread_mutex_unlock(&display_mutex);

	return 0;
}

int SampleUtil_FindAndParseServiceList(IXML_NodeList *srvList, const char *base,
    const char *serviceType,
	char **serviceId,
	char **eventURL,
	char **controlURL)
{
	int ret;
	int found = 0;    
	unsigned int i;
	unsigned long length;
	IXML_Element *service = NULL;
	char *SrvType = NULL;
	char *relctrlURL = NULL;
	char *releventURL = NULL;

    length = ixmlNodeList_length(srvList);
    for (i = 0; i < length; i++) {
        service = (IXML_Element *)ixmlNodeList_item(srvList, i);
        SrvType = SampleUtil_GetFirstElementItem((IXML_Element *)service, "serviceType");
        if (SrvType && strcmp(SrvType, serviceType) == 0) {
            SampleUtilPrint("Found service: %s\n", serviceType);
            *serviceId = SampleUtil_GetFirstElementItem(service, "serviceId");
            SampleUtilPrint("serviceId: %s\n", *serviceId);
            relctrlURL = SampleUtil_GetFirstElementItem(service, "controlURL");
            releventURL = SampleUtil_GetFirstElementItem(service, "eventSubURL");
            ret = UpnpResolveURL2(base, relctrlURL, controlURL);
            if (ret != UPNP_E_SUCCESS)
                SampleUtilPrint("Error relctrlURL from %s:%s\n", base, relctrlURL);
            ret = UpnpResolveURL2(base, releventURL, eventURL);
            if (ret != UPNP_E_SUCCESS)
                SampleUtilPrint("Error releventURL from %s:%s\n", base, releventURL);
            free(relctrlURL);
            free(releventURL);
            relctrlURL = NULL;
            releventURL = NULL;
            found = 1;
            break;
        }
        free(SrvType);
        SrvType = NULL;
    }
    free(SrvType);
    SrvType = NULL;

    return found;
}

int SampleUtil_FindAndParseService(IXML_Document *DescDoc,
	const char *location,
	const char *serviceType,
	char **serviceId,
	char **eventURL,
	char **controlURL)
{
	int found = 0;
	char *baseURL = NULL;
	const char *base = NULL;
	IXML_NodeList *srvList = NULL;
    
#ifdef OLD_FIND_SERVICE_CODE
#else  /* OLD_FIND_SERVICE_CODE */
        unsigned int sindex = 0;
#endif /* OLD_FIND_SERVICE_CODE */

	baseURL = SampleUtil_GetFirstDocumentItem(DescDoc, "URLBase");
	base = baseURL ? baseURL : location;
    
#ifdef OLD_FIND_SERVICE_CODE
	srvList = SampleUtil_GetFirstServiceList(DescDoc);
#else
	for (sindex = 0; (srvList = SampleUtil_GetNthServiceList(DescDoc, sindex));	sindex++) {
#endif /* OLD_FIND_SERVICE_CODE */
    	found = SampleUtil_FindAndParseServiceList(srvList, base,
    	    serviceType, serviceId, eventURL, controlURL);
        if (srvList)
            ixmlNodeList_free(srvList);
        srvList = NULL;
#ifdef OLD_FIND_SERVICE_CODE
#else
	}
#endif /* OLD_FIND_SERVICE_CODE */

	free(baseURL);

	return found;
}

int SampleUtil_Print(const char *fmt, ...)
{
#define MAX_BUF (8 * 1024)
	va_list ap;
	static char buf[MAX_BUF];
	int rc;

	/* Protect both the display and the static buffer with the mutex */
	ithread_mutex_lock(&display_mutex);

	va_start(ap, fmt);
	rc = vsnprintf(buf, MAX_BUF, fmt, ap);
	va_end(ap);
	if (gPrintFun)
		gPrintFun("%s", buf);

	ithread_mutex_unlock(&display_mutex);

	return rc;
}

void SampleUtil_StateUpdate(const char *varName,
	const char *varValue,
	const char *UDN,
	eventType type)
{
	/* TBD: Add mutex here? */
	if (gStateUpdateFun)
		gStateUpdateFun(varName, varValue, UDN, type);
}

/*!
 * \brief Prints a string to standard out.
 */
void linux_print(const char *format, ...)
{
	va_list argList;

	va_start(argList, format);
	vfprintf(stdout, format, argList);
	fflush(stdout);
	va_end(argList);
}

/*! @} UpnpSamples */
