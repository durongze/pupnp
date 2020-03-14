#ifndef __TV_CONFIG_H__
#define __TV_CONFIG_H__

#define UNIVERSAL_RESOURCE_IDENTIFIER
#define UNIQUE_DEVICE_NAME     "uuid:Upnp-TVEmulator-1_0-1234567890001"
#define DESC_DOC_URL    "http://169.254.156.11:49157/tvdevicedesc.xml"
#define FRIENDLY_NAME   "UPnP Television Emulator"
#define PRES_URL        "http://169.254.156.11:49157/tvdevicepres.html"
#define ADVER_TIME_OUT  100

#define SERVICE_ID   "ServiceId"  
#define SERVICE_TYPE "ServiceType"
#define EVENT_URL    "EventURL"   
#define CONTROL_URL  "ControlURL" 
#define SUBSCRIBER_ID "SID"        

#define TV_CONTROL_SERVICE
#define CTRL_SRV_ID         "urn:upnp-org:serviceId:tvcontrol1"
#define CTRL_SRV_TYPE       "urn:schemas-upnp-org:service:tvcontrol:1"
#define CTRL_EVENT_URL      "http://169.254.156.11:49157/upnp/event/tvcontrol1"
#define CTRL_CTRL_URL       "http://169.254.156.11:49157/upnp/control/tvcontrol1"
#define CTRL_SID           "uuid:7e7fea9c-64ec-11ea-a58a-a412463dd9d8"

#define TV_PICTURE_SERVICE
#define PIC_SRV_ID         "urn:upnp-org:serviceId:tvpicture1"
#define PIC_SRV_TYPE       "urn:schemas-upnp-org:service:tvpicture:1"
#define PIC_EVENT_URL      "http://169.254.156.11:49157/upnp/event/tvpicture1"
#define PIC_CTRL_URL       "http://169.254.156.11:49157/upnp/control/tvpicture1"
#define PIC_SID           "uuid:7e853e34-64ec-11ea-a58a-a412463dd9d8"

#define SET_POWER_ON    "PowerOn"
#define SET_POWER_OFF   "PowerOff"
#define SET_CHANNEL "SetChannel"
#define SET_VOLUME  "SetVolume"
#define SET_COLOR   "SetColor"
#define SET_TINT    "SetTint"
#define SET_LOG     "SetLog"
#define SET_CONTRAST   "SetContrast"
#define SET_BRIGHTNESS "SetBrightness"

#define CTRL_SERVICE_STATE_TABLE
#define CTRL_POWER        "Power"       
#define CTRL_CHANNEL      "Channel"     
#define CTRL_VOLUME       "Volume"      
#define CTRL_LOGLVL       "Level" 

#define PIC_SERVICE_STATE_TABLE
#define PIC_COLOR         "Color"         
#define PIC_TINT          "Tint"          
#define PIC_CONTRAST      "Contrast"      
#define PIC_BRIGHTNESS    "Brightness"    

/*! Number of services. */
#define TV_SERVICE_SERVCOUNT  2

/*! Index of control service */
#define TV_SERVICE_CONTROL    0

/*! Index of picture service */
#define TV_SERVICE_PICTURE    1

/*! Number of control variables */
#define TV_CONTROL_VARCOUNT   4

/*! Index of power variable */
#define TV_CONTROL_POWER      0

/*! Index of channel variable */
#define TV_CONTROL_CHANNEL    1

/*! Index of volume variable */
#define TV_CONTROL_VOLUME     2

#define TV_CONTROL_LOGLVL     3

/*! Number of picture variables */
#define TV_PICTURE_VARCOUNT   4

/*! Index of color variable */
#define TV_PICTURE_COLOR      0

/*! Index of tint variable */
#define TV_PICTURE_TINT       1

/*! Index of contrast variable */
#define TV_PICTURE_CONTRAST   2

/*! Index of brightness variable */
#define TV_PICTURE_BRIGHTNESS 3

/*! Max value length */
#define TV_MAX_VAL_LEN 7

#endif
