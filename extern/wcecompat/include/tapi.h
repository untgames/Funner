//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
/*++

The  Telephony  API  is jointly copyrighted by Intel and Microsoft.  You are
granted  a royalty free worldwide, unlimited license to make copies, and use
the   API/SPI  for  making  applications/drivers  that  interface  with  the
specification provided that this paragraph and the Intel/Microsoft copyright
statement is maintained as is in the text and source code files.

Portions copyright 1992, 1993 Intel/Microsoft, all rights reserved.

Module Name:

    tapi.h

Notes:

    Additions to the Telephony Application Programming Interface (TAPI) since
    version 1.0 are noted by version number (e.g. "TAPI v1.4").

--*/

#ifndef TAPI_H
#define TAPI_H



//
//  -- TAPI VERSION INFO -- TAPI VERSION INFO -- TAPI VERSION INFO --
//  -- TAPI VERSION INFO -- TAPI VERSION INFO -- TAPI VERSION INFO --
//  -- TAPI VERSION INFO -- TAPI VERSION INFO -- TAPI VERSION INFO --
//
// To build  a TAPI 1.4 application put a define as below in your source
// file before you include TAPI.H:
//
// #define TAPI_CURRENT_VERSION 0x00010004
//
//
#ifndef TAPI_CURRENT_VERSION
#define TAPI_CURRENT_VERSION 0x00020000
#endif

#include <windows.h>

// Type definitions of the data types used in tapi

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


//
// TAPI type definitions
//

#define DECLARE_OPAQUE32(name)  struct name##__ { int unused; }; \
                typedef const struct name##__ FAR* name

DECLARE_OPAQUE32(HCALL);
typedef HCALL FAR * LPHCALL;

DECLARE_OPAQUE32(HLINE);
typedef HLINE FAR * LPHLINE;

DECLARE_OPAQUE32(HPHONE);
typedef HPHONE FAR * LPHPHONE;

DECLARE_OPAQUE32(HLINEAPP);
typedef HLINEAPP FAR * LPHLINEAPP;

DECLARE_OPAQUE32(HPHONEAPP);
typedef HPHONEAPP FAR * LPHPHONEAPP;

typedef HICON FAR * LPHICON;

typedef void (CALLBACK * LINECALLBACK)(
    DWORD               hDevice,
    DWORD               dwMessage,
    DWORD               dwInstance,
    DWORD               dwParam1,
    DWORD               dwParam2,
    DWORD               dwParam3
    );

typedef void (CALLBACK * PHONECALLBACK)(
    DWORD               hDevice,
    DWORD               dwMessage,
    DWORD               dwInstance,
    DWORD               dwParam1,
    DWORD               dwParam2,
    DWORD               dwParam3
    );


// Messages for Phones and Lines

#define LINE_ADDRESSSTATE                       0L
#define LINE_CALLINFO                           1L
#define LINE_CALLSTATE                          2L
#define LINE_CLOSE                              3L
#define LINE_DEVSPECIFIC                        4L
#define LINE_DEVSPECIFICFEATURE                 5L
#define LINE_GATHERDIGITS                       6L
#define LINE_GENERATE                           7L
#define LINE_LINEDEVSTATE                       8L
#define LINE_MONITORDIGITS                      9L
#define LINE_MONITORMEDIA                       10L
#define LINE_MONITORTONE                        11L
#define LINE_REPLY                              12L
#define LINE_REQUEST                            13L
#define PHONE_BUTTON                            14L
#define PHONE_CLOSE                             15L
#define PHONE_DEVSPECIFIC                       16L
#define PHONE_REPLY                             17L
#define PHONE_STATE                             18L
#define LINE_CREATE                             19L             // TAPI v1.4
#define PHONE_CREATE                            20L             // TAPI v1.4

#define LINE_AGENTSPECIFIC                      21L             // TAPI v2.0
#define LINE_AGENTSTATUS                        22L             // TAPI v2.0
#define LINE_APPNEWCALL                         23L             // TAPI v2.0
#define LINE_PROXYREQUEST                       24L             // TAPI v2.0
#define LINE_REMOVE                             25L             // TAPI v2.0
#define PHONE_REMOVE                            26L             // TAPI v2.0

#define INITIALIZE_NEGOTIATION                  0xFFFFFFFFL

#define LINEADDRCAPFLAGS_FWDNUMRINGS            0x00000001
#define LINEADDRCAPFLAGS_PICKUPGROUPID          0x00000002
#define LINEADDRCAPFLAGS_SECURE                 0x00000004
#define LINEADDRCAPFLAGS_BLOCKIDDEFAULT         0x00000008
#define LINEADDRCAPFLAGS_BLOCKIDOVERRIDE        0x00000010
#define LINEADDRCAPFLAGS_DIALED                 0x00000020
#define LINEADDRCAPFLAGS_ORIGOFFHOOK            0x00000040
#define LINEADDRCAPFLAGS_DESTOFFHOOK            0x00000080
#define LINEADDRCAPFLAGS_FWDCONSULT             0x00000100
#define LINEADDRCAPFLAGS_SETUPCONFNULL          0x00000200
#define LINEADDRCAPFLAGS_AUTORECONNECT          0x00000400
#define LINEADDRCAPFLAGS_COMPLETIONID           0x00000800
#define LINEADDRCAPFLAGS_TRANSFERHELD           0x00001000
#define LINEADDRCAPFLAGS_TRANSFERMAKE           0x00002000
#define LINEADDRCAPFLAGS_CONFERENCEHELD         0x00004000
#define LINEADDRCAPFLAGS_CONFERENCEMAKE         0x00008000
#define LINEADDRCAPFLAGS_PARTIALDIAL            0x00010000
#define LINEADDRCAPFLAGS_FWDSTATUSVALID         0x00020000
#define LINEADDRCAPFLAGS_FWDINTEXTADDR          0x00040000
#define LINEADDRCAPFLAGS_FWDBUSYNAADDR          0x00080000
#define LINEADDRCAPFLAGS_ACCEPTTOALERT          0x00100000
#define LINEADDRCAPFLAGS_CONFDROP               0x00200000
#define LINEADDRCAPFLAGS_PICKUPCALLWAIT         0x00400000

#define LINEADDRCAPFLAGS_PREDICTIVEDIALER       0x00800000      // TAPI v2.0
#define LINEADDRCAPFLAGS_QUEUE                  0x01000000      // TAPI v2.0
#define LINEADDRCAPFLAGS_ROUTEPOINT             0x02000000      // TAPI v2.0
#define LINEADDRCAPFLAGS_HOLDMAKESNEW           0x04000000      // TAPI v2.0
#define LINEADDRCAPFLAGS_NOINTERNALCALLS        0x08000000      // TAPI v2.0
#define LINEADDRCAPFLAGS_NOEXTERNALCALLS        0x10000000      // TAPI v2.0
#define LINEADDRCAPFLAGS_SETCALLINGID           0x20000000      // TAPI v2.0


#define LINEADDRESSMODE_ADDRESSID               0x00000001
#define LINEADDRESSMODE_DIALABLEADDR            0x00000002

#define LINEADDRESSSHARING_PRIVATE              0x00000001
#define LINEADDRESSSHARING_BRIDGEDEXCL          0x00000002
#define LINEADDRESSSHARING_BRIDGEDNEW           0x00000004
#define LINEADDRESSSHARING_BRIDGEDSHARED        0x00000008
#define LINEADDRESSSHARING_MONITORED            0x00000010

#define LINEADDRESSSTATE_OTHER                  0x00000001
#define LINEADDRESSSTATE_DEVSPECIFIC            0x00000002
#define LINEADDRESSSTATE_INUSEZERO              0x00000004
#define LINEADDRESSSTATE_INUSEONE               0x00000008
#define LINEADDRESSSTATE_INUSEMANY              0x00000010
#define LINEADDRESSSTATE_NUMCALLS               0x00000020
#define LINEADDRESSSTATE_FORWARD                0x00000040
#define LINEADDRESSSTATE_TERMINALS              0x00000080
#define LINEADDRESSSTATE_CAPSCHANGE             0x00000100      // TAPI v1.4

#define LINEADDRESSTYPE_PHONENUMBER             0x00000001
#define LINEADDRESSTYPE_SDP                     0x00000002
#define LINEADDRESSTYPE_EMAILNAME               0x00000004
#define LINEADDRESSTYPE_DOMAINNAME              0x00000008
#define LINEADDRESSTYPE_IPADDRESS               0x00000010


#define LINEADDRFEATURE_FORWARD                 0x00000001
#define LINEADDRFEATURE_MAKECALL                0x00000002
#define LINEADDRFEATURE_PICKUP                  0x00000004
#define LINEADDRFEATURE_SETMEDIACONTROL         0x00000008
#define LINEADDRFEATURE_SETTERMINAL             0x00000010
#define LINEADDRFEATURE_SETUPCONF               0x00000020
#define LINEADDRFEATURE_UNCOMPLETECALL          0x00000040
#define LINEADDRFEATURE_UNPARK                  0x00000080

#define LINEADDRFEATURE_PICKUPHELD              0x00000100      // TAPI v2.0
#define LINEADDRFEATURE_PICKUPGROUP             0x00000200      // TAPI v2.0
#define LINEADDRFEATURE_PICKUPDIRECT            0x00000400      // TAPI v2.0
#define LINEADDRFEATURE_PICKUPWAITING           0x00000800      // TAPI v2.0
#define LINEADDRFEATURE_FORWARDFWD              0x00001000      // TAPI v2.0
#define LINEADDRFEATURE_FORWARDDND              0x00002000      // TAPI v2.0



#define LINEAGENTFEATURE_SETAGENTGROUP          0x00000001      // TAPI v2.0
#define LINEAGENTFEATURE_SETAGENTSTATE          0x00000002      // TAPI v2.0
#define LINEAGENTFEATURE_SETAGENTACTIVITY       0x00000004      // TAPI v2.0
#define LINEAGENTFEATURE_AGENTSPECIFIC          0x00000008      // TAPI v2.0
#define LINEAGENTFEATURE_GETAGENTACTIVITYLIST   0x00000010      // TAPI v2.0
#define LINEAGENTFEATURE_GETAGENTGROUP          0x00000020      // TAPI v2.0

#define LINEAGENTSTATE_LOGGEDOFF                0x00000001      // TAPI v2.0
#define LINEAGENTSTATE_NOTREADY                 0x00000002      // TAPI v2.0
#define LINEAGENTSTATE_READY                    0x00000004      // TAPI v2.0
#define LINEAGENTSTATE_BUSYACD                  0x00000008      // TAPI v2.0
#define LINEAGENTSTATE_BUSYINCOMING             0x00000010      // TAPI v2.0
#define LINEAGENTSTATE_BUSYOUTBOUND             0x00000020      // TAPI v2.0
#define LINEAGENTSTATE_BUSYOTHER                0x00000040      // TAPI v2.0
#define LINEAGENTSTATE_WORKINGAFTERCALL         0x00000080      // TAPI v2.0
#define LINEAGENTSTATE_UNKNOWN                  0x00000100      // TAPI v2.0
#define LINEAGENTSTATE_UNAVAIL                  0x00000200      // TAPI v2.0

#define LINEAGENTSTATUS_GROUP                   0x00000001      // TAPI v2.0
#define LINEAGENTSTATUS_STATE                   0x00000002      // TAPI v2.0
#define LINEAGENTSTATUS_NEXTSTATE               0x00000004      // TAPI v2.0
#define LINEAGENTSTATUS_ACTIVITY                0x00000008      // TAPI v2.0
#define LINEAGENTSTATUS_ACTIVITYLIST            0x00000010      // TAPI v2.0
#define LINEAGENTSTATUS_GROUPLIST               0x00000020      // TAPI v2.0
#define LINEAGENTSTATUS_CAPSCHANGE              0x00000040      // TAPI v2.0
#define LINEAGENTSTATUS_VALIDSTATES             0x00000080      // TAPI v2.0
#define LINEAGENTSTATUS_VALIDNEXTSTATES         0x00000100      // TAPI v2.0



#define LINEANSWERMODE_NONE                     0x00000001
#define LINEANSWERMODE_DROP                     0x00000002
#define LINEANSWERMODE_HOLD                     0x00000004

#define LINEBEARERMODE_VOICE                    0x00000001
#define LINEBEARERMODE_SPEECH                   0x00000002
#define LINEBEARERMODE_MULTIUSE                 0x00000004
#define LINEBEARERMODE_DATA                     0x00000008
#define LINEBEARERMODE_ALTSPEECHDATA            0x00000010
#define LINEBEARERMODE_NONCALLSIGNALING         0x00000020
#define LINEBEARERMODE_PASSTHROUGH              0x00000040      // TAPI v1.4

#define LINEBEARERMODE_RESTRICTEDDATA           0x00000080      // TAPI v2.0


#define LINEBUSYMODE_STATION                    0x00000001
#define LINEBUSYMODE_TRUNK                      0x00000002
#define LINEBUSYMODE_UNKNOWN                    0x00000004
#define LINEBUSYMODE_UNAVAIL                    0x00000008

#define LINECALLCOMPLCOND_BUSY                  0x00000001
#define LINECALLCOMPLCOND_NOANSWER              0x00000002

#define LINECALLCOMPLMODE_CAMPON                0x00000001
#define LINECALLCOMPLMODE_CALLBACK              0x00000002
#define LINECALLCOMPLMODE_INTRUDE               0x00000004
#define LINECALLCOMPLMODE_MESSAGE               0x00000008

#define LINECALLFEATURE_ACCEPT                  0x00000001
#define LINECALLFEATURE_ADDTOCONF               0x00000002
#define LINECALLFEATURE_ANSWER                  0x00000004
#define LINECALLFEATURE_BLINDTRANSFER           0x00000008
#define LINECALLFEATURE_COMPLETECALL            0x00000010
#define LINECALLFEATURE_COMPLETETRANSF          0x00000020
#define LINECALLFEATURE_DIAL                    0x00000040
#define LINECALLFEATURE_DROP                    0x00000080
#define LINECALLFEATURE_GATHERDIGITS            0x00000100
#define LINECALLFEATURE_GENERATEDIGITS          0x00000200
#define LINECALLFEATURE_GENERATETONE            0x00000400
#define LINECALLFEATURE_HOLD                    0x00000800
#define LINECALLFEATURE_MONITORDIGITS           0x00001000
#define LINECALLFEATURE_MONITORMEDIA            0x00002000
#define LINECALLFEATURE_MONITORTONES            0x00004000
#define LINECALLFEATURE_PARK                    0x00008000
#define LINECALLFEATURE_PREPAREADDCONF          0x00010000
#define LINECALLFEATURE_REDIRECT                0x00020000
#define LINECALLFEATURE_REMOVEFROMCONF          0x00040000
#define LINECALLFEATURE_SECURECALL              0x00080000
#define LINECALLFEATURE_SENDUSERUSER            0x00100000
#define LINECALLFEATURE_SETCALLPARAMS           0x00200000
#define LINECALLFEATURE_SETMEDIACONTROL         0x00400000
#define LINECALLFEATURE_SETTERMINAL             0x00800000
#define LINECALLFEATURE_SETUPCONF               0x01000000
#define LINECALLFEATURE_SETUPTRANSFER           0x02000000
#define LINECALLFEATURE_SWAPHOLD                0x04000000
#define LINECALLFEATURE_UNHOLD                  0x08000000
#define LINECALLFEATURE_RELEASEUSERUSERINFO     0x10000000      // TAPI v1.4

#define LINECALLFEATURE_SETTREATMENT            0x20000000      // TAPI v2.0
#define LINECALLFEATURE_SETQOS                  0x40000000      // TAPI v2.0
#define LINECALLFEATURE_SETCALLDATA             0x80000000      // TAPI v2.0



#define LINECALLFEATURE2_NOHOLDCONFERENCE       0x00000001      // TAPI v2.0
#define LINECALLFEATURE2_ONESTEPTRANSFER        0x00000002      // TAPI v2.0
#define LINECALLFEATURE2_COMPLCAMPON            0x00000004      // TAPI v2.0
#define LINECALLFEATURE2_COMPLCALLBACK          0x00000008      // TAPI v2.0
#define LINECALLFEATURE2_COMPLINTRUDE           0x00000010      // TAPI v2.0
#define LINECALLFEATURE2_COMPLMESSAGE           0x00000020      // TAPI v2.0
#define LINECALLFEATURE2_TRANSFERNORM           0x00000040      // TAPI v2.0
#define LINECALLFEATURE2_TRANSFERCONF           0x00000080      // TAPI v2.0
#define LINECALLFEATURE2_PARKDIRECT             0x00000100      // TAPI v2.0
#define LINECALLFEATURE2_PARKNONDIRECT          0x00000200      // TAPI v2.0


#define LINECALLINFOSTATE_OTHER                 0x00000001
#define LINECALLINFOSTATE_DEVSPECIFIC           0x00000002
#define LINECALLINFOSTATE_BEARERMODE            0x00000004
#define LINECALLINFOSTATE_RATE                  0x00000008
#define LINECALLINFOSTATE_MEDIAMODE             0x00000010
#define LINECALLINFOSTATE_APPSPECIFIC           0x00000020
#define LINECALLINFOSTATE_CALLID                0x00000040
#define LINECALLINFOSTATE_RELATEDCALLID         0x00000080
#define LINECALLINFOSTATE_ORIGIN                0x00000100
#define LINECALLINFOSTATE_REASON                0x00000200
#define LINECALLINFOSTATE_COMPLETIONID          0x00000400
#define LINECALLINFOSTATE_NUMOWNERINCR          0x00000800
#define LINECALLINFOSTATE_NUMOWNERDECR          0x00001000
#define LINECALLINFOSTATE_NUMMONITORS           0x00002000
#define LINECALLINFOSTATE_TRUNK                 0x00004000
#define LINECALLINFOSTATE_CALLERID              0x00008000
#define LINECALLINFOSTATE_CALLEDID              0x00010000
#define LINECALLINFOSTATE_CONNECTEDID           0x00020000
#define LINECALLINFOSTATE_REDIRECTIONID         0x00040000
#define LINECALLINFOSTATE_REDIRECTINGID         0x00080000
#define LINECALLINFOSTATE_DISPLAY               0x00100000
#define LINECALLINFOSTATE_USERUSERINFO          0x00200000
#define LINECALLINFOSTATE_HIGHLEVELCOMP         0x00400000
#define LINECALLINFOSTATE_LOWLEVELCOMP          0x00800000
#define LINECALLINFOSTATE_CHARGINGINFO          0x01000000
#define LINECALLINFOSTATE_TERMINAL              0x02000000
#define LINECALLINFOSTATE_DIALPARAMS            0x04000000
#define LINECALLINFOSTATE_MONITORMODES          0x08000000

#define LINECALLINFOSTATE_TREATMENT             0x10000000      // TAPI v2.0
#define LINECALLINFOSTATE_QOS                   0x20000000      // TAPI v2.0
#define LINECALLINFOSTATE_CALLDATA              0x40000000      // TAPI v2.0


#define LINECALLORIGIN_OUTBOUND                 0x00000001
#define LINECALLORIGIN_INTERNAL                 0x00000002
#define LINECALLORIGIN_EXTERNAL                 0x00000004
#define LINECALLORIGIN_UNKNOWN                  0x00000010
#define LINECALLORIGIN_UNAVAIL                  0x00000020
#define LINECALLORIGIN_CONFERENCE               0x00000040
#define LINECALLORIGIN_INBOUND                  0x00000080      // TAPI v1.4

#define LINECALLPARAMFLAGS_SECURE               0x00000001
#define LINECALLPARAMFLAGS_IDLE                 0x00000002
#define LINECALLPARAMFLAGS_BLOCKID              0x00000004
#define LINECALLPARAMFLAGS_ORIGOFFHOOK          0x00000008
#define LINECALLPARAMFLAGS_DESTOFFHOOK          0x00000010

#define LINECALLPARAMFLAGS_NOHOLDCONFERENCE     0x00000020      // TAPI v2.0
#define LINECALLPARAMFLAGS_PREDICTIVEDIAL       0x00000040      // TAPI v2.0
#define LINECALLPARAMFLAGS_ONESTEPTRANSFER      0x00000080      // TAPI v2.0


#define LINECALLPARTYID_BLOCKED                 0x00000001
#define LINECALLPARTYID_OUTOFAREA               0x00000002
#define LINECALLPARTYID_NAME                    0x00000004
#define LINECALLPARTYID_ADDRESS                 0x00000008
#define LINECALLPARTYID_PARTIAL                 0x00000010
#define LINECALLPARTYID_UNKNOWN                 0x00000020
#define LINECALLPARTYID_UNAVAIL                 0x00000040

#define LINECALLPRIVILEGE_NONE                  0x00000001
#define LINECALLPRIVILEGE_MONITOR               0x00000002
#define LINECALLPRIVILEGE_OWNER                 0x00000004

#define LINECALLREASON_DIRECT                   0x00000001
#define LINECALLREASON_FWDBUSY                  0x00000002
#define LINECALLREASON_FWDNOANSWER              0x00000004
#define LINECALLREASON_FWDUNCOND                0x00000008
#define LINECALLREASON_PICKUP                   0x00000010
#define LINECALLREASON_UNPARK                   0x00000020
#define LINECALLREASON_REDIRECT                 0x00000040
#define LINECALLREASON_CALLCOMPLETION           0x00000080
#define LINECALLREASON_TRANSFER                 0x00000100
#define LINECALLREASON_REMINDER                 0x00000200
#define LINECALLREASON_UNKNOWN                  0x00000400
#define LINECALLREASON_UNAVAIL                  0x00000800
#define LINECALLREASON_INTRUDE                  0x00001000      // TAPI v1.4
#define LINECALLREASON_PARKED                   0x00002000      // TAPI v1.4

#define LINECALLREASON_CAMPEDON                 0x00004000      // TAPI v2.0
#define LINECALLREASON_ROUTEREQUEST             0x00008000      // TAPI v2.0


#define LINECALLSELECT_LINE                     0x00000001
#define LINECALLSELECT_ADDRESS                  0x00000002
#define LINECALLSELECT_CALL                     0x00000004
#define LINECALLSELECT_DEVICEID                 0x00000008      // TAPI v2.1
#define LINECALLSELECT_CALLID                   0x00000010      // TAPI v3.0

#define LINECALLSTATE_IDLE                      0x00000001
#define LINECALLSTATE_OFFERING                  0x00000002
#define LINECALLSTATE_ACCEPTED                  0x00000004
#define LINECALLSTATE_DIALTONE                  0x00000008
#define LINECALLSTATE_DIALING                   0x00000010
#define LINECALLSTATE_RINGBACK                  0x00000020
#define LINECALLSTATE_BUSY                      0x00000040
#define LINECALLSTATE_SPECIALINFO               0x00000080
#define LINECALLSTATE_CONNECTED                 0x00000100
#define LINECALLSTATE_PROCEEDING                0x00000200
#define LINECALLSTATE_ONHOLD                    0x00000400
#define LINECALLSTATE_CONFERENCED               0x00000800
#define LINECALLSTATE_ONHOLDPENDCONF            0x00001000
#define LINECALLSTATE_ONHOLDPENDTRANSFER        0x00002000
#define LINECALLSTATE_DISCONNECTED              0x00004000
#define LINECALLSTATE_UNKNOWN                   0x00008000


#define LINECALLTREATMENT_SILENCE               0x00000001      // TAPI v2.0
#define LINECALLTREATMENT_RINGBACK              0x00000002      // TAPI v2.0
#define LINECALLTREATMENT_BUSY                  0x00000003      // TAPI v2.0
#define LINECALLTREATMENT_MUSIC                 0x00000004      // TAPI v2.0


#define LINECARDOPTION_PREDEFINED               0x00000001      // TAPI v1.4
#define LINECARDOPTION_HIDDEN                   0x00000002      // TAPI v1.4

#define LINECONNECTEDMODE_ACTIVE                0x00000001      // TAPI v1.4
#define LINECONNECTEDMODE_INACTIVE              0x00000002      // TAPI v1.4

#define LINECONNECTEDMODE_ACTIVEHELD            0x00000004      // TAPI v2.0
#define LINECONNECTEDMODE_INACTIVEHELD          0x00000008      // TAPI v2.0
#define LINECONNECTEDMODE_CONFIRMED             0x00000010      // TAPI v2.0


#define LINEDEVCAPFLAGS_CROSSADDRCONF           0x00000001
#define LINEDEVCAPFLAGS_HIGHLEVCOMP             0x00000002
#define LINEDEVCAPFLAGS_LOWLEVCOMP              0x00000004
#define LINEDEVCAPFLAGS_MEDIACONTROL            0x00000008
#define LINEDEVCAPFLAGS_MULTIPLEADDR            0x00000010
#define LINEDEVCAPFLAGS_CLOSEDROP               0x00000020
#define LINEDEVCAPFLAGS_DIALBILLING             0x00000040
#define LINEDEVCAPFLAGS_DIALQUIET               0x00000080
#define LINEDEVCAPFLAGS_DIALDIALTONE            0x00000100
#define LINEDEVCAPFLAGS_MSP                     0x00000200      // TAPI v3.0
#define LINEDEVCAPFLAGS_CALLHUB                 0x00000400      // TAPI v3.0
#define LINEDEVCAPFLAGS_CALLHUBTRACKING         0x00000800      // TAPI v3.0
#define LINEDEVCAPFLAGS_PRIVATEOBJECTS          0x00001000      // TAPI v3.0

#define LINEDEVSTATE_OTHER                      0x00000001
#define LINEDEVSTATE_RINGING                    0x00000002
#define LINEDEVSTATE_CONNECTED                  0x00000004
#define LINEDEVSTATE_DISCONNECTED               0x00000008
#define LINEDEVSTATE_MSGWAITON                  0x00000010
#define LINEDEVSTATE_MSGWAITOFF                 0x00000020
#define LINEDEVSTATE_INSERVICE                  0x00000040
#define LINEDEVSTATE_OUTOFSERVICE               0x00000080
#define LINEDEVSTATE_MAINTENANCE                0x00000100
#define LINEDEVSTATE_OPEN                       0x00000200
#define LINEDEVSTATE_CLOSE                      0x00000400
#define LINEDEVSTATE_NUMCALLS                   0x00000800
#define LINEDEVSTATE_NUMCOMPLETIONS             0x00001000
#define LINEDEVSTATE_TERMINALS                  0x00002000
#define LINEDEVSTATE_ROAMMODE                   0x00004000
#define LINEDEVSTATE_BATTERY                    0x00008000
#define LINEDEVSTATE_SIGNAL                     0x00010000
#define LINEDEVSTATE_DEVSPECIFIC                0x00020000
#define LINEDEVSTATE_REINIT                     0x00040000
#define LINEDEVSTATE_LOCK                       0x00080000
#define LINEDEVSTATE_CAPSCHANGE                 0x00100000      // TAPI v1.4
#define LINEDEVSTATE_CONFIGCHANGE               0x00200000      // TAPI v1.4
#define LINEDEVSTATE_TRANSLATECHANGE            0x00400000      // TAPI v1.4
#define LINEDEVSTATE_COMPLCANCEL                0x00800000      // TAPI v1.4
#define LINEDEVSTATE_REMOVED                    0x01000000      // TAPI v1.4

#define LINEDEVSTATUSFLAGS_CONNECTED            0x00000001
#define LINEDEVSTATUSFLAGS_MSGWAIT              0x00000002
#define LINEDEVSTATUSFLAGS_INSERVICE            0x00000004
#define LINEDEVSTATUSFLAGS_LOCKED               0x00000008

#define LINEDIALTONEMODE_NORMAL                 0x00000001
#define LINEDIALTONEMODE_SPECIAL                0x00000002
#define LINEDIALTONEMODE_INTERNAL               0x00000004
#define LINEDIALTONEMODE_EXTERNAL               0x00000008
#define LINEDIALTONEMODE_UNKNOWN                0x00000010
#define LINEDIALTONEMODE_UNAVAIL                0x00000020
    
#define LINEDIGITMODE_PULSE                     0x00000001
#define LINEDIGITMODE_DTMF                      0x00000002
#define LINEDIGITMODE_DTMFEND                   0x00000004
    
#define LINEDISCONNECTMODE_NORMAL               0x00000001
#define LINEDISCONNECTMODE_UNKNOWN              0x00000002
#define LINEDISCONNECTMODE_REJECT               0x00000004
#define LINEDISCONNECTMODE_PICKUP               0x00000008
#define LINEDISCONNECTMODE_FORWARDED            0x00000010
#define LINEDISCONNECTMODE_BUSY                 0x00000020
#define LINEDISCONNECTMODE_NOANSWER             0x00000040
#define LINEDISCONNECTMODE_BADADDRESS           0x00000080
#define LINEDISCONNECTMODE_UNREACHABLE          0x00000100
#define LINEDISCONNECTMODE_CONGESTION           0x00000200
#define LINEDISCONNECTMODE_INCOMPATIBLE         0x00000400
#define LINEDISCONNECTMODE_UNAVAIL              0x00000800
#define LINEDISCONNECTMODE_NODIALTONE           0x00001000      // TAPI v1.4

#define LINEDISCONNECTMODE_NUMBERCHANGED        0x00002000      // TAPI v2.0
#define LINEDISCONNECTMODE_OUTOFORDER           0x00004000      // TAPI v2.0
#define LINEDISCONNECTMODE_TEMPFAILURE          0x00008000      // TAPI v2.0
#define LINEDISCONNECTMODE_QOSUNAVAIL           0x00010000      // TAPI v2.0
#define LINEDISCONNECTMODE_BLOCKED              0x00020000      // TAPI v2.0
#define LINEDISCONNECTMODE_DONOTDISTURB         0x00040000      // TAPI v2.0
#define LINEDISCONNECTMODE_CANCELLED            0x00080000      // TAPI v2.0


#define LINEERR_ALLOCATED                       0x80000001
#define LINEERR_BADDEVICEID                     0x80000002
#define LINEERR_BEARERMODEUNAVAIL               0x80000003
#define LINEERR_CALLUNAVAIL                     0x80000005
#define LINEERR_COMPLETIONOVERRUN               0x80000006
#define LINEERR_CONFERENCEFULL                  0x80000007
#define LINEERR_DIALBILLING                     0x80000008
#define LINEERR_DIALDIALTONE                    0x80000009
#define LINEERR_DIALPROMPT                      0x8000000A
#define LINEERR_DIALQUIET                       0x8000000B
#define LINEERR_INCOMPATIBLEAPIVERSION          0x8000000C
#define LINEERR_INCOMPATIBLEEXTVERSION          0x8000000D
#define LINEERR_INIFILECORRUPT                  0x8000000E
#define LINEERR_INUSE                           0x8000000F
#define LINEERR_INVALADDRESS                    0x80000010
#define LINEERR_INVALADDRESSID                  0x80000011
#define LINEERR_INVALADDRESSMODE                0x80000012
#define LINEERR_INVALADDRESSSTATE               0x80000013
#define LINEERR_INVALAPPHANDLE                  0x80000014
#define LINEERR_INVALAPPNAME                    0x80000015
#define LINEERR_INVALBEARERMODE                 0x80000016
#define LINEERR_INVALCALLCOMPLMODE              0x80000017
#define LINEERR_INVALCALLHANDLE                 0x80000018
#define LINEERR_INVALCALLPARAMS                 0x80000019
#define LINEERR_INVALCALLPRIVILEGE              0x8000001A
#define LINEERR_INVALCALLSELECT                 0x8000001B
#define LINEERR_INVALCALLSTATE                  0x8000001C
#define LINEERR_INVALCALLSTATELIST              0x8000001D
#define LINEERR_INVALCARD                       0x8000001E
#define LINEERR_INVALCOMPLETIONID               0x8000001F
#define LINEERR_INVALCONFCALLHANDLE             0x80000020
#define LINEERR_INVALCONSULTCALLHANDLE          0x80000021
#define LINEERR_INVALCOUNTRYCODE                0x80000022
#define LINEERR_INVALDEVICECLASS                0x80000023
#define LINEERR_INVALDEVICEHANDLE               0x80000024
#define LINEERR_INVALDIALPARAMS                 0x80000025
#define LINEERR_INVALDIGITLIST                  0x80000026
#define LINEERR_INVALDIGITMODE                  0x80000027
#define LINEERR_INVALDIGITS                     0x80000028
#define LINEERR_INVALEXTVERSION                 0x80000029
#define LINEERR_INVALGROUPID                    0x8000002A
#define LINEERR_INVALLINEHANDLE                 0x8000002B
#define LINEERR_INVALLINESTATE                  0x8000002C
#define LINEERR_INVALLOCATION                   0x8000002D
#define LINEERR_INVALMEDIALIST                  0x8000002E
#define LINEERR_INVALMEDIAMODE                  0x8000002F
#define LINEERR_INVALMESSAGEID                  0x80000030
#define LINEERR_INVALPARAM                      0x80000032
#define LINEERR_INVALPARKID                     0x80000033
#define LINEERR_INVALPARKMODE                   0x80000034
#define LINEERR_INVALPOINTER                    0x80000035
#define LINEERR_INVALPRIVSELECT                 0x80000036
#define LINEERR_INVALRATE                       0x80000037
#define LINEERR_INVALREQUESTMODE                0x80000038
#define LINEERR_INVALTERMINALID                 0x80000039
#define LINEERR_INVALTERMINALMODE               0x8000003A
#define LINEERR_INVALTIMEOUT                    0x8000003B
#define LINEERR_INVALTONE                       0x8000003C
#define LINEERR_INVALTONELIST                   0x8000003D
#define LINEERR_INVALTONEMODE                   0x8000003E
#define LINEERR_INVALTRANSFERMODE               0x8000003F
#define LINEERR_LINEMAPPERFAILED                0x80000040
#define LINEERR_NOCONFERENCE                    0x80000041
#define LINEERR_NODEVICE                        0x80000042
#define LINEERR_NODRIVER                        0x80000043
#define LINEERR_NOMEM                           0x80000044
#define LINEERR_NOREQUEST                       0x80000045
#define LINEERR_NOTOWNER                        0x80000046
#define LINEERR_NOTREGISTERED                   0x80000047
#define LINEERR_OPERATIONFAILED                 0x80000048
#define LINEERR_OPERATIONUNAVAIL                0x80000049
#define LINEERR_RATEUNAVAIL                     0x8000004A
#define LINEERR_RESOURCEUNAVAIL                 0x8000004B
#define LINEERR_REQUESTOVERRUN                  0x8000004C
#define LINEERR_STRUCTURETOOSMALL               0x8000004D
#define LINEERR_TARGETNOTFOUND                  0x8000004E
#define LINEERR_TARGETSELF                      0x8000004F
#define LINEERR_UNINITIALIZED                   0x80000050
#define LINEERR_USERUSERINFOTOOBIG              0x80000051
#define LINEERR_REINIT                          0x80000052
#define LINEERR_ADDRESSBLOCKED                  0x80000053
#define LINEERR_BILLINGREJECTED                 0x80000054
#define LINEERR_INVALFEATURE                    0x80000055
#define LINEERR_NOMULTIPLEINSTANCE              0x80000056

#define LINEERR_INVALAGENTID                    0x80000057      // TAPI v2.0
#define LINEERR_INVALAGENTGROUP                 0x80000058      // TAPI v2.0
#define LINEERR_INVALPASSWORD                   0x80000059      // TAPI v2.0
#define LINEERR_INVALAGENTSTATE                 0x8000005A      // TAPI v2.0
#define LINEERR_INVALAGENTACTIVITY              0x8000005B      // TAPI v2.0
#define LINEERR_DIALVOICEDETECT                 0x8000005C      // TAPI v2.0


#define LINEFEATURE_DEVSPECIFIC                 0x00000001
#define LINEFEATURE_DEVSPECIFICFEAT             0x00000002
#define LINEFEATURE_FORWARD                     0x00000004
#define LINEFEATURE_MAKECALL                    0x00000008
#define LINEFEATURE_SETMEDIACONTROL             0x00000010
#define LINEFEATURE_SETTERMINAL                 0x00000020

#define LINEFEATURE_SETDEVSTATUS                0x00000040      // TAPI v2.0
#define LINEFEATURE_FORWARDFWD                  0x00000080      // TAPI v2.0
#define LINEFEATURE_FORWARDDND                  0x00000100      // TAPI v2.0


#define LINEFORWARDMODE_UNCOND                  0x00000001
#define LINEFORWARDMODE_UNCONDINTERNAL          0x00000002
#define LINEFORWARDMODE_UNCONDEXTERNAL          0x00000004
#define LINEFORWARDMODE_UNCONDSPECIFIC          0x00000008
#define LINEFORWARDMODE_BUSY                    0x00000010
#define LINEFORWARDMODE_BUSYINTERNAL            0x00000020
#define LINEFORWARDMODE_BUSYEXTERNAL            0x00000040
#define LINEFORWARDMODE_BUSYSPECIFIC            0x00000080
#define LINEFORWARDMODE_NOANSW                  0x00000100
#define LINEFORWARDMODE_NOANSWINTERNAL          0x00000200
#define LINEFORWARDMODE_NOANSWEXTERNAL          0x00000400
#define LINEFORWARDMODE_NOANSWSPECIFIC          0x00000800
#define LINEFORWARDMODE_BUSYNA                  0x00001000
#define LINEFORWARDMODE_BUSYNAINTERNAL          0x00002000
#define LINEFORWARDMODE_BUSYNAEXTERNAL          0x00004000
#define LINEFORWARDMODE_BUSYNASPECIFIC          0x00008000
#define LINEFORWARDMODE_UNKNOWN                 0x00010000      // TAPI v1.4
#define LINEFORWARDMODE_UNAVAIL                 0x00020000      // TAPI v1.4

#define LINEGATHERTERM_BUFFERFULL               0x00000001
#define LINEGATHERTERM_TERMDIGIT                0x00000002
#define LINEGATHERTERM_FIRSTTIMEOUT             0x00000004
#define LINEGATHERTERM_INTERTIMEOUT             0x00000008
#define LINEGATHERTERM_CANCEL                   0x00000010

#define LINEGENERATETERM_DONE                   0x00000001
#define LINEGENERATETERM_CANCEL                 0x00000002

//
// These constants are mutually exclusive - there's no way to specify more
// than one at a time (and it doesn't make sense, either) so they're
// ordinal rather than bits.
//
#define LINEINITIALIZEEXOPTION_USEHIDDENWINDOW      0x00000001  // TAPI v2.0
#define LINEINITIALIZEEXOPTION_USEEVENT             0x00000002  // TAPI v2.0
#define LINEINITIALIZEEXOPTION_USECOMPLETIONPORT    0x00000003  // TAPI v2.0

#define LINELOCATIONOPTION_PULSEDIAL            0x00000001      // TAPI v1.4

#define LINEMAPPER                              0xFFFFFFFF

#define LINEMEDIACONTROL_NONE                   0x00000001
#define LINEMEDIACONTROL_START                  0x00000002
#define LINEMEDIACONTROL_RESET                  0x00000004
#define LINEMEDIACONTROL_PAUSE                  0x00000008
#define LINEMEDIACONTROL_RESUME                 0x00000010
#define LINEMEDIACONTROL_RATEUP                 0x00000020
#define LINEMEDIACONTROL_RATEDOWN               0x00000040
#define LINEMEDIACONTROL_RATENORMAL             0x00000080
#define LINEMEDIACONTROL_VOLUMEUP               0x00000100
#define LINEMEDIACONTROL_VOLUMEDOWN             0x00000200
#define LINEMEDIACONTROL_VOLUMENORMAL           0x00000400

#define LINEMEDIAMODE_UNKNOWN                   0x00000002
#define LINEMEDIAMODE_INTERACTIVEVOICE          0x00000004
#define LINEMEDIAMODE_AUTOMATEDVOICE            0x00000008
#define LINEMEDIAMODE_DATAMODEM                 0x00000010
#define LINEMEDIAMODE_G3FAX                     0x00000020
#define LINEMEDIAMODE_TDD                       0x00000040
#define LINEMEDIAMODE_G4FAX                     0x00000080
#define LINEMEDIAMODE_DIGITALDATA               0x00000100
#define LINEMEDIAMODE_TELETEX                   0x00000200
#define LINEMEDIAMODE_VIDEOTEX                  0x00000400
#define LINEMEDIAMODE_TELEX                     0x00000800
#define LINEMEDIAMODE_MIXED                     0x00001000
#define LINEMEDIAMODE_ADSI                      0x00002000
#define LINEMEDIAMODE_VOICEVIEW                 0x00004000      // TAPI v1.4
#define LAST_LINEMEDIAMODE                      0x00004000

#define LINEOFFERINGMODE_ACTIVE                 0x00000001      // TAPI v1.4
#define LINEOFFERINGMODE_INACTIVE               0x00000002      // TAPI v1.4

#define LINEOPENOPTION_SINGLEADDRESS            0x80000000      // TAPI v2.0
#define LINEOPENOPTION_PROXY                    0x40000000      // TAPI v2.0


#define LINEPARKMODE_DIRECTED                   0x00000001
#define LINEPARKMODE_NONDIRECTED                0x00000002

#define LINEPROXYREQUEST_SETAGENTGROUP          0x00000001      // TAPI v2.0
#define LINEPROXYREQUEST_SETAGENTSTATE          0x00000002      // TAPI v2.0
#define LINEPROXYREQUEST_SETAGENTACTIVITY       0x00000003      // TAPI v2.0
#define LINEPROXYREQUEST_GETAGENTCAPS           0x00000004      // TAPI v2.0
#define LINEPROXYREQUEST_GETAGENTSTATUS         0x00000005      // TAPI v2.0
#define LINEPROXYREQUEST_AGENTSPECIFIC          0x00000006      // TAPI v2.0
#define LINEPROXYREQUEST_GETAGENTACTIVITYLIST   0x00000007      // TAPI v2.0
#define LINEPROXYREQUEST_GETAGENTGROUPLIST      0x00000008      // TAPI v2.0

#define LINEREMOVEFROMCONF_NONE                 0x00000001
#define LINEREMOVEFROMCONF_LAST                 0x00000002
#define LINEREMOVEFROMCONF_ANY                  0x00000003

#define LINEREQUESTMODE_MAKECALL                0x00000001
#define LINEREQUESTMODE_MEDIACALL               0x00000002
#define LINEREQUESTMODE_DROP                    0x00000004
#define LAST_LINEREQUESTMODE                    LINEREQUESTMODE_MEDIACALL

#define LINEROAMMODE_UNKNOWN                    0x00000001
#define LINEROAMMODE_UNAVAIL                    0x00000002
#define LINEROAMMODE_HOME                       0x00000004
#define LINEROAMMODE_ROAMA                      0x00000008
#define LINEROAMMODE_ROAMB                      0x00000010

#define LINESPECIALINFO_NOCIRCUIT               0x00000001
#define LINESPECIALINFO_CUSTIRREG               0x00000002
#define LINESPECIALINFO_REORDER                 0x00000004
#define LINESPECIALINFO_UNKNOWN                 0x00000008
#define LINESPECIALINFO_UNAVAIL                 0x00000010

#define LINETERMDEV_PHONE                       0x00000001
#define LINETERMDEV_HEADSET                     0x00000002
#define LINETERMDEV_SPEAKER                     0x00000004

#define LINETERMMODE_BUTTONS                    0x00000001
#define LINETERMMODE_LAMPS                      0x00000002
#define LINETERMMODE_DISPLAY                    0x00000004
#define LINETERMMODE_RINGER                     0x00000008
#define LINETERMMODE_HOOKSWITCH                 0x00000010
#define LINETERMMODE_MEDIATOLINE                0x00000020
#define LINETERMMODE_MEDIAFROMLINE              0x00000040
#define LINETERMMODE_MEDIABIDIRECT              0x00000080

#define LINETERMSHARING_PRIVATE                 0x00000001
#define LINETERMSHARING_SHAREDEXCL              0x00000002
#define LINETERMSHARING_SHAREDCONF              0x00000004

#define LINETOLLLISTOPTION_ADD                  0x00000001
#define LINETOLLLISTOPTION_REMOVE               0x00000002

#define LINETONEMODE_CUSTOM                     0x00000001
#define LINETONEMODE_RINGBACK                   0x00000002
#define LINETONEMODE_BUSY                       0x00000004
#define LINETONEMODE_BEEP                       0x00000008
#define LINETONEMODE_BILLING                    0x00000010

#define LINETRANSFERMODE_TRANSFER               0x00000001
#define LINETRANSFERMODE_CONFERENCE             0x00000002

#define LINETRANSLATEOPTION_CARDOVERRIDE        0x00000001
#define LINETRANSLATEOPTION_CANCELCALLWAITING   0x00000002      // TAPI v1.4
#define LINETRANSLATEOPTION_FORCELOCAL          0x00000004      // TAPI v1.4
#define LINETRANSLATEOPTION_FORCELD             0x00000008      // TAPI v1.4

#define LINETRANSLATERESULT_CANONICAL           0x00000001
#define LINETRANSLATERESULT_INTERNATIONAL       0x00000002
#define LINETRANSLATERESULT_LONGDISTANCE        0x00000004
#define LINETRANSLATERESULT_LOCAL               0x00000008
#define LINETRANSLATERESULT_INTOLLLIST          0x00000010
#define LINETRANSLATERESULT_NOTINTOLLLIST       0x00000020
#define LINETRANSLATERESULT_DIALBILLING         0x00000040
#define LINETRANSLATERESULT_DIALQUIET           0x00000080
#define LINETRANSLATERESULT_DIALDIALTONE        0x00000100
#define LINETRANSLATERESULT_DIALPROMPT          0x00000200

#define LINETRANSLATERESULT_VOICEDETECT         0x00000400      // TAPI v2.0


#define PHONEBUTTONFUNCTION_UNKNOWN             0x00000000
#define PHONEBUTTONFUNCTION_CONFERENCE          0x00000001
#define PHONEBUTTONFUNCTION_TRANSFER            0x00000002
#define PHONEBUTTONFUNCTION_DROP                0x00000003
#define PHONEBUTTONFUNCTION_HOLD                0x00000004
#define PHONEBUTTONFUNCTION_RECALL              0x00000005
#define PHONEBUTTONFUNCTION_DISCONNECT          0x00000006
#define PHONEBUTTONFUNCTION_CONNECT             0x00000007
#define PHONEBUTTONFUNCTION_MSGWAITON           0x00000008
#define PHONEBUTTONFUNCTION_MSGWAITOFF          0x00000009
#define PHONEBUTTONFUNCTION_SELECTRING          0x0000000A
#define PHONEBUTTONFUNCTION_ABBREVDIAL          0x0000000B
#define PHONEBUTTONFUNCTION_FORWARD             0x0000000C
#define PHONEBUTTONFUNCTION_PICKUP              0x0000000D
#define PHONEBUTTONFUNCTION_RINGAGAIN           0x0000000E
#define PHONEBUTTONFUNCTION_PARK                0x0000000F
#define PHONEBUTTONFUNCTION_REJECT              0x00000010
#define PHONEBUTTONFUNCTION_REDIRECT            0x00000011
#define PHONEBUTTONFUNCTION_MUTE                0x00000012
#define PHONEBUTTONFUNCTION_VOLUMEUP            0x00000013
#define PHONEBUTTONFUNCTION_VOLUMEDOWN          0x00000014
#define PHONEBUTTONFUNCTION_SPEAKERON           0x00000015
#define PHONEBUTTONFUNCTION_SPEAKEROFF          0x00000016
#define PHONEBUTTONFUNCTION_FLASH               0x00000017
#define PHONEBUTTONFUNCTION_DATAON              0x00000018
#define PHONEBUTTONFUNCTION_DATAOFF             0x00000019
#define PHONEBUTTONFUNCTION_DONOTDISTURB        0x0000001A
#define PHONEBUTTONFUNCTION_INTERCOM            0x0000001B
#define PHONEBUTTONFUNCTION_BRIDGEDAPP          0x0000001C
#define PHONEBUTTONFUNCTION_BUSY                0x0000001D
#define PHONEBUTTONFUNCTION_CALLAPP             0x0000001E
#define PHONEBUTTONFUNCTION_DATETIME            0x0000001F
#define PHONEBUTTONFUNCTION_DIRECTORY           0x00000020
#define PHONEBUTTONFUNCTION_COVER               0x00000021
#define PHONEBUTTONFUNCTION_CALLID              0x00000022
#define PHONEBUTTONFUNCTION_LASTNUM             0x00000023
#define PHONEBUTTONFUNCTION_NIGHTSRV            0x00000024
#define PHONEBUTTONFUNCTION_SENDCALLS           0x00000025
#define PHONEBUTTONFUNCTION_MSGINDICATOR        0x00000026
#define PHONEBUTTONFUNCTION_REPDIAL             0x00000027
#define PHONEBUTTONFUNCTION_SETREPDIAL          0x00000028
#define PHONEBUTTONFUNCTION_SYSTEMSPEED         0x00000029
#define PHONEBUTTONFUNCTION_STATIONSPEED        0x0000002A
#define PHONEBUTTONFUNCTION_CAMPON              0x0000002B
#define PHONEBUTTONFUNCTION_SAVEREPEAT          0x0000002C
#define PHONEBUTTONFUNCTION_QUEUECALL           0x0000002D
#define PHONEBUTTONFUNCTION_NONE                0x0000002E

#define PHONEBUTTONMODE_DUMMY                   0x00000001
#define PHONEBUTTONMODE_CALL                    0x00000002
#define PHONEBUTTONMODE_FEATURE                 0x00000004
#define PHONEBUTTONMODE_KEYPAD                  0x00000008
#define PHONEBUTTONMODE_LOCAL                   0x00000010
#define PHONEBUTTONMODE_DISPLAY                 0x00000020

#define PHONEBUTTONSTATE_UP                     0x00000001
#define PHONEBUTTONSTATE_DOWN                   0x00000002
#define PHONEBUTTONSTATE_UNKNOWN                0x00000004      // TAPI v1.4
#define PHONEBUTTONSTATE_UNAVAIL                0x00000008      // TAPI v1.4

#define PHONEERR_ALLOCATED                      0x90000001
#define PHONEERR_BADDEVICEID                    0x90000002
#define PHONEERR_INCOMPATIBLEAPIVERSION         0x90000003
#define PHONEERR_INCOMPATIBLEEXTVERSION         0x90000004
#define PHONEERR_INIFILECORRUPT                 0x90000005
#define PHONEERR_INUSE                          0x90000006
#define PHONEERR_INVALAPPHANDLE                 0x90000007
#define PHONEERR_INVALAPPNAME                   0x90000008
#define PHONEERR_INVALBUTTONLAMPID              0x90000009
#define PHONEERR_INVALBUTTONMODE                0x9000000A
#define PHONEERR_INVALBUTTONSTATE               0x9000000B
#define PHONEERR_INVALDATAID                    0x9000000C
#define PHONEERR_INVALDEVICECLASS               0x9000000D
#define PHONEERR_INVALEXTVERSION                0x9000000E
#define PHONEERR_INVALHOOKSWITCHDEV             0x9000000F
#define PHONEERR_INVALHOOKSWITCHMODE            0x90000010
#define PHONEERR_INVALLAMPMODE                  0x90000011
#define PHONEERR_INVALPARAM                     0x90000012
#define PHONEERR_INVALPHONEHANDLE               0x90000013
#define PHONEERR_INVALPHONESTATE                0x90000014
#define PHONEERR_INVALPOINTER                   0x90000015
#define PHONEERR_INVALPRIVILEGE                 0x90000016
#define PHONEERR_INVALRINGMODE                  0x90000017
#define PHONEERR_NODEVICE                       0x90000018
#define PHONEERR_NODRIVER                       0x90000019
#define PHONEERR_NOMEM                          0x9000001A
#define PHONEERR_NOTOWNER                       0x9000001B
#define PHONEERR_OPERATIONFAILED                0x9000001C
#define PHONEERR_OPERATIONUNAVAIL               0x9000001D
#define PHONEERR_RESOURCEUNAVAIL                0x9000001F
#define PHONEERR_REQUESTOVERRUN                 0x90000020
#define PHONEERR_STRUCTURETOOSMALL              0x90000021
#define PHONEERR_UNINITIALIZED                  0x90000022
#define PHONEERR_REINIT                         0x90000023


#define PHONEFEATURE_GETBUTTONINFO              0x00000001      // TAPI v2.0
#define PHONEFEATURE_GETDATA                    0x00000002      // TAPI v2.0
#define PHONEFEATURE_GETDISPLAY                 0x00000004      // TAPI v2.0
#define PHONEFEATURE_GETGAINHANDSET             0x00000008      // TAPI v2.0
#define PHONEFEATURE_GETGAINSPEAKER             0x00000010      // TAPI v2.0
#define PHONEFEATURE_GETGAINHEADSET             0x00000020      // TAPI v2.0
#define PHONEFEATURE_GETHOOKSWITCHHANDSET       0x00000040      // TAPI v2.0
#define PHONEFEATURE_GETHOOKSWITCHSPEAKER       0x00000080      // TAPI v2.0
#define PHONEFEATURE_GETHOOKSWITCHHEADSET       0x00000100      // TAPI v2.0
#define PHONEFEATURE_GETLAMP                    0x00000200      // TAPI v2.0
#define PHONEFEATURE_GETRING                    0x00000400      // TAPI v2.0
#define PHONEFEATURE_GETVOLUMEHANDSET           0x00000800      // TAPI v2.0
#define PHONEFEATURE_GETVOLUMESPEAKER           0x00001000      // TAPI v2.0
#define PHONEFEATURE_GETVOLUMEHEADSET           0x00002000      // TAPI v2.0
#define PHONEFEATURE_SETBUTTONINFO              0x00004000      // TAPI v2.0
#define PHONEFEATURE_SETDATA                    0x00008000      // TAPI v2.0
#define PHONEFEATURE_SETDISPLAY                 0x00010000      // TAPI v2.0
#define PHONEFEATURE_SETGAINHANDSET             0x00020000      // TAPI v2.0
#define PHONEFEATURE_SETGAINSPEAKER             0x00040000      // TAPI v2.0
#define PHONEFEATURE_SETGAINHEADSET             0x00080000      // TAPI v2.0
#define PHONEFEATURE_SETHOOKSWITCHHANDSET       0x00100000      // TAPI v2.0
#define PHONEFEATURE_SETHOOKSWITCHSPEAKER       0x00200000      // TAPI v2.0
#define PHONEFEATURE_SETHOOKSWITCHHEADSET       0x00400000      // TAPI v2.0
#define PHONEFEATURE_SETLAMP                    0x00800000      // TAPI v2.0
#define PHONEFEATURE_SETRING                    0x01000000      // TAPI v2.0
#define PHONEFEATURE_SETVOLUMEHANDSET           0x02000000      // TAPI v2.0
#define PHONEFEATURE_SETVOLUMESPEAKER           0x04000000      // TAPI v2.0
#define PHONEFEATURE_SETVOLUMEHEADSET           0x08000000      // TAPI v2.0


#define PHONEHOOKSWITCHDEV_HANDSET              0x00000001
#define PHONEHOOKSWITCHDEV_SPEAKER              0x00000002
#define PHONEHOOKSWITCHDEV_HEADSET              0x00000004

#define PHONEHOOKSWITCHMODE_ONHOOK              0x00000001
#define PHONEHOOKSWITCHMODE_MIC                 0x00000002
#define PHONEHOOKSWITCHMODE_SPEAKER             0x00000004
#define PHONEHOOKSWITCHMODE_MICSPEAKER          0x00000008
#define PHONEHOOKSWITCHMODE_UNKNOWN             0x00000010


#define PHONEINITIALIZEEXOPTION_USEHIDDENWINDOW     0x00000001  // TAPI v2.0
#define PHONEINITIALIZEEXOPTION_USEEVENT            0x00000002  // TAPI v2.0
#define PHONEINITIALIZEEXOPTION_USECOMPLETIONPORT   0x00000003  // TAPI v2.0


#define PHONELAMPMODE_DUMMY                     0x00000001
#define PHONELAMPMODE_OFF                       0x00000002
#define PHONELAMPMODE_STEADY                    0x00000004
#define PHONELAMPMODE_WINK                      0x00000008
#define PHONELAMPMODE_FLASH                     0x00000010
#define PHONELAMPMODE_FLUTTER                   0x00000020
#define PHONELAMPMODE_BROKENFLUTTER             0x00000040
#define PHONELAMPMODE_UNKNOWN                   0x00000080

#define PHONEPRIVILEGE_MONITOR                  0x00000001
#define PHONEPRIVILEGE_OWNER                    0x00000002

#define PHONESTATE_OTHER                        0x00000001
#define PHONESTATE_CONNECTED                    0x00000002
#define PHONESTATE_DISCONNECTED                 0x00000004
#define PHONESTATE_OWNER                        0x00000008
#define PHONESTATE_MONITORS                     0x00000010
#define PHONESTATE_DISPLAY                      0x00000020
#define PHONESTATE_LAMP                         0x00000040
#define PHONESTATE_RINGMODE                     0x00000080
#define PHONESTATE_RINGVOLUME                   0x00000100
#define PHONESTATE_HANDSETHOOKSWITCH            0x00000200
#define PHONESTATE_HANDSETVOLUME                0x00000400
#define PHONESTATE_HANDSETGAIN                  0x00000800
#define PHONESTATE_SPEAKERHOOKSWITCH            0x00001000
#define PHONESTATE_SPEAKERVOLUME                0x00002000
#define PHONESTATE_SPEAKERGAIN                  0x00004000
#define PHONESTATE_HEADSETHOOKSWITCH            0x00008000
#define PHONESTATE_HEADSETVOLUME                0x00010000
#define PHONESTATE_HEADSETGAIN                  0x00020000
#define PHONESTATE_SUSPEND                      0x00040000
#define PHONESTATE_RESUME                       0x00080000
#define PHONESTATE_DEVSPECIFIC                  0x00100000
#define PHONESTATE_REINIT                       0x00200000
#define PHONESTATE_CAPSCHANGE                   0x00400000      // TAPI v1.4
#define PHONESTATE_REMOVED                      0x00800000      // TAPI v1.4

#define PHONESTATUSFLAGS_CONNECTED              0x00000001
#define PHONESTATUSFLAGS_SUSPENDED              0x00000002

#define STRINGFORMAT_ASCII                      0x00000001
#define STRINGFORMAT_DBCS                       0x00000002
#define STRINGFORMAT_UNICODE                    0x00000003
#define STRINGFORMAT_BINARY                     0x00000004

#define TAPI_REPLY                              WM_USER + 99

#define TAPIERR_CONNECTED                       0L
#define TAPIERR_DROPPED                         -1L
#define TAPIERR_NOREQUESTRECIPIENT              -2L
#define TAPIERR_REQUESTQUEUEFULL                -3L
#define TAPIERR_INVALDESTADDRESS                -4L
#define TAPIERR_INVALWINDOWHANDLE               -5L
#define TAPIERR_INVALDEVICECLASS                -6L
#define TAPIERR_INVALDEVICEID                   -7L
#define TAPIERR_DEVICECLASSUNAVAIL              -8L
#define TAPIERR_DEVICEIDUNAVAIL                 -9L
#define TAPIERR_DEVICEINUSE                     -10L
#define TAPIERR_DESTBUSY                        -11L
#define TAPIERR_DESTNOANSWER                    -12L
#define TAPIERR_DESTUNAVAIL                     -13L
#define TAPIERR_UNKNOWNWINHANDLE                -14L
#define TAPIERR_UNKNOWNREQUESTID                -15L
#define TAPIERR_REQUESTFAILED                   -16L
#define TAPIERR_REQUESTCANCELLED                -17L
#define TAPIERR_INVALPOINTER                    -18L


#define TAPIMAXDESTADDRESSSIZE                  80L
#define TAPIMAXAPPNAMESIZE                      40L
#define TAPIMAXCALLEDPARTYSIZE                  40L
#define TAPIMAXCOMMENTSIZE                      80L
#define TAPIMAXDEVICECLASSSIZE                  40L
#define TAPIMAXDEVICEIDSIZE                     40L


#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;

#endif /* GUID_DEFINED */

#ifndef __LPGUID_DEFINED__
#define __LPGUID_DEFINED__
typedef GUID *LPGUID;
#endif


typedef struct lineaddresscaps_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwLineDeviceID;
    DWORD       dwAddressSize;
    DWORD       dwAddressOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;
    DWORD       dwAddressSharing;
    DWORD       dwAddressStates;
    DWORD       dwCallInfoStates;
    DWORD       dwCallerIDFlags;
    DWORD       dwCalledIDFlags;
    DWORD       dwConnectedIDFlags;
    DWORD       dwRedirectionIDFlags;
    DWORD       dwRedirectingIDFlags;
    DWORD       dwCallStates;
    DWORD       dwDialToneModes;
    DWORD       dwBusyModes;
    DWORD       dwSpecialInfo;
    DWORD       dwDisconnectModes;
    DWORD       dwMaxNumActiveCalls;
    DWORD       dwMaxNumOnHoldCalls;
    DWORD       dwMaxNumOnHoldPendingCalls;
    DWORD       dwMaxNumConference;
    DWORD       dwMaxNumTransConf;
    DWORD       dwAddrCapFlags;
    DWORD       dwCallFeatures;
    DWORD       dwRemoveFromConfCaps;
    DWORD       dwRemoveFromConfState;
    DWORD       dwTransferModes;
    DWORD       dwParkModes;
    DWORD       dwForwardModes;
    DWORD       dwMaxForwardEntries;
    DWORD       dwMaxSpecificEntries;
    DWORD       dwMinFwdNumRings;
    DWORD       dwMaxFwdNumRings;
    DWORD       dwMaxCallCompletions;
    DWORD       dwCallCompletionConds;
    DWORD       dwCallCompletionModes;
    DWORD       dwNumCompletionMessages;
    DWORD       dwCompletionMsgTextEntrySize;
    DWORD       dwCompletionMsgTextSize;
    DWORD       dwCompletionMsgTextOffset;

    DWORD       dwAddressFeatures;                              // TAPI v1.4

    DWORD       dwPredictiveAutoTransferStates;                 // TAPI v2.0
    DWORD       dwNumCallTreatments;                            // TAPI v2.0
    DWORD       dwCallTreatmentListSize;                        // TAPI v2.0
    DWORD       dwCallTreatmentListOffset;                      // TAPI v2.0
    DWORD       dwDeviceClassesSize;                            // TAPI v2.0
    DWORD       dwDeviceClassesOffset;                          // TAPI v2.0
    DWORD       dwMaxCallDataSize;                              // TAPI v2.0
    DWORD       dwCallFeatures2;                                // TAPI v2.0
    DWORD       dwMaxNoAnswerTimeout;                           // TAPI v2.0
    DWORD       dwConnectedModes;                               // TAPI v2.0
    DWORD       dwOfferingModes;                                // TAPI v2.0
    DWORD       dwAvailableMediaModes;                          // TAPI v2.0

} LINEADDRESSCAPS, FAR *LPLINEADDRESSCAPS;

typedef struct lineaddressstatus_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwNumInUse;
    DWORD       dwNumActiveCalls;
    DWORD       dwNumOnHoldCalls;
    DWORD       dwNumOnHoldPendCalls;
    DWORD       dwAddressFeatures;
    DWORD       dwNumRingsNoAnswer;
    DWORD       dwForwardNumEntries;
    DWORD       dwForwardSize;
    DWORD       dwForwardOffset;
    DWORD       dwTerminalModesSize;
    DWORD       dwTerminalModesOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;

} LINEADDRESSSTATUS, FAR *LPLINEADDRESSSTATUS;

typedef struct linedialparams_tag
{
    DWORD       dwDialPause;
    DWORD       dwDialSpeed;
    DWORD       dwDigitDuration;
    DWORD       dwWaitForDialtone;

} LINEDIALPARAMS, FAR *LPLINEDIALPARAMS;

typedef struct linecallinfo_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    HLINE       hLine;
    DWORD       dwLineDeviceID;
    DWORD       dwAddressID;
    DWORD       dwBearerMode;
    DWORD       dwRate;
    DWORD       dwMediaMode;
    DWORD       dwAppSpecific;
    DWORD       dwCallID;
    DWORD       dwRelatedCallID;
    DWORD       dwCallParamFlags;
    DWORD       dwCallStates;
    DWORD       dwMonitorDigitModes;
    DWORD       dwMonitorMediaModes;
    LINEDIALPARAMS  DialParams;
    DWORD       dwOrigin;
    DWORD       dwReason;
    DWORD       dwCompletionID;
    DWORD       dwNumOwners;
    DWORD       dwNumMonitors;
    DWORD       dwCountryCode;
    DWORD       dwTrunk;
    DWORD       dwCallerIDFlags;
    DWORD       dwCallerIDSize;
    DWORD       dwCallerIDOffset;
    DWORD       dwCallerIDNameSize;
    DWORD       dwCallerIDNameOffset;
    DWORD       dwCalledIDFlags;
    DWORD       dwCalledIDSize;
    DWORD       dwCalledIDOffset;
    DWORD       dwCalledIDNameSize;
    DWORD       dwCalledIDNameOffset;
    DWORD       dwConnectedIDFlags;
    DWORD       dwConnectedIDSize;
    DWORD       dwConnectedIDOffset;
    DWORD       dwConnectedIDNameSize;
    DWORD       dwConnectedIDNameOffset;
    DWORD       dwRedirectionIDFlags;
    DWORD       dwRedirectionIDSize;
    DWORD       dwRedirectionIDOffset;
    DWORD       dwRedirectionIDNameSize;
    DWORD       dwRedirectionIDNameOffset;
    DWORD       dwRedirectingIDFlags;
    DWORD       dwRedirectingIDSize;
    DWORD       dwRedirectingIDOffset;
    DWORD       dwRedirectingIDNameSize;
    DWORD       dwRedirectingIDNameOffset;
    DWORD       dwAppNameSize;
    DWORD       dwAppNameOffset;
    DWORD       dwDisplayableAddressSize;
    DWORD       dwDisplayableAddressOffset;
    DWORD       dwCalledPartySize;
    DWORD       dwCalledPartyOffset;
    DWORD       dwCommentSize;
    DWORD       dwCommentOffset;
    DWORD       dwDisplaySize;
    DWORD       dwDisplayOffset;
    DWORD       dwUserUserInfoSize;
    DWORD       dwUserUserInfoOffset;
    DWORD       dwHighLevelCompSize;
    DWORD       dwHighLevelCompOffset;
    DWORD       dwLowLevelCompSize;
    DWORD       dwLowLevelCompOffset;
    DWORD       dwChargingInfoSize;
    DWORD       dwChargingInfoOffset;
    DWORD       dwTerminalModesSize;
    DWORD       dwTerminalModesOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;
    DWORD       dwCallTreatment;                                // TAPI v2.0
    DWORD       dwCallDataSize;                                 // TAPI v2.0
    DWORD       dwCallDataOffset;                               // TAPI v2.0
    DWORD       dwSendingFlowspecSize;                          // TAPI v2.0
    DWORD       dwSendingFlowspecOffset;                        // TAPI v2.0
    DWORD       dwReceivingFlowspecSize;                        // TAPI v2.0
    DWORD       dwReceivingFlowspecOffset;                      // TAPI v2.0
    DWORD       dwAddressType;                                  // TAPI v3.0
} LINECALLINFO, FAR *LPLINECALLINFO;

typedef struct linecalllist_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwCallsNumEntries;
    DWORD       dwCallsSize;
    DWORD       dwCallsOffset;

} LINECALLLIST, FAR *LPLINECALLLIST;

typedef struct linecallparams_tag               // Defaults:
{
    DWORD       dwTotalSize;                    // ---------
    DWORD       dwBearerMode;                   // voice
    DWORD       dwMinRate;                      // (3.1kHz)
    DWORD       dwMaxRate;                      // (3.1kHz)
    DWORD       dwMediaMode;                    // interactiveVoice
    DWORD       dwCallParamFlags;               // 0
    DWORD       dwAddressMode;                  // addressID
    DWORD       dwAddressID;                    // (any available)
    LINEDIALPARAMS  DialParams;                 // (0, 0, 0, 0)
    DWORD       dwOrigAddressSize;              // 0
    DWORD       dwOrigAddressOffset;
    DWORD       dwDisplayableAddressSize;
    DWORD       dwDisplayableAddressOffset;
    DWORD       dwCalledPartySize;              // 0
    DWORD       dwCalledPartyOffset;
    DWORD       dwCommentSize;                  // 0
    DWORD       dwCommentOffset;
    DWORD       dwUserUserInfoSize;             // 0
    DWORD       dwUserUserInfoOffset;
    DWORD       dwHighLevelCompSize;            // 0
    DWORD       dwHighLevelCompOffset;
    DWORD       dwLowLevelCompSize;             // 0
    DWORD       dwLowLevelCompOffset;
    DWORD       dwDevSpecificSize;              // 0
    DWORD       dwDevSpecificOffset;
    DWORD       dwPredictiveAutoTransferStates;                 // TAPI v2.0
    DWORD       dwTargetAddressSize;                            // TAPI v2.0
    DWORD       dwTargetAddressOffset;                          // TAPI v2.0
    DWORD       dwSendingFlowspecSize;                          // TAPI v2.0
    DWORD       dwSendingFlowspecOffset;                        // TAPI v2.0
    DWORD       dwReceivingFlowspecSize;                        // TAPI v2.0
    DWORD       dwReceivingFlowspecOffset;                      // TAPI v2.0
    DWORD       dwDeviceClassSize;                              // TAPI v2.0
    DWORD       dwDeviceClassOffset;                            // TAPI v2.0
    DWORD       dwDeviceConfigSize;                             // TAPI v2.0
    DWORD       dwDeviceConfigOffset;                           // TAPI v2.0
    DWORD       dwCallDataSize;                                 // TAPI v2.0
    DWORD       dwCallDataOffset;                               // TAPI v2.0
    DWORD       dwNoAnswerTimeout;                              // TAPI v2.0
    DWORD       dwCallingPartyIDSize;                           // TAPI v2.0
    DWORD       dwCallingPartyIDOffset;                         // TAPI v2.0
    DWORD       dwAddressType;                                  // TAPI v3.0
} LINECALLPARAMS, FAR *LPLINECALLPARAMS;

typedef struct linecallstatus_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwCallState;
    DWORD       dwCallStateMode;
    DWORD       dwCallPrivilege;
    DWORD       dwCallFeatures;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;

    DWORD       dwCallFeatures2;                                // TAPI v2.0
    SYSTEMTIME  tStateEntryTime;                                // TAPI v2.0
} LINECALLSTATUS, FAR *LPLINECALLSTATUS;


typedef struct linecalltreatmententry_tag
{
    DWORD       dwCallTreatmentID;                              // TAPI v2.0
    DWORD       dwCallTreatmentNameSize;                        // TAPI v2.0
    DWORD       dwCallTreatmentNameOffset;                      // TAPI v2.0

} LINECALLTREATMENTENTRY, FAR *LPLINECALLTREATMENTENTRY;


typedef struct linecardentry_tag
{
    DWORD       dwPermanentCardID;
    DWORD       dwCardNameSize;
    DWORD       dwCardNameOffset;
    DWORD       dwCardNumberDigits;                             // TAPI v1.4
    DWORD       dwSameAreaRuleSize;                             // TAPI v1.4
    DWORD       dwSameAreaRuleOffset;                           // TAPI v1.4
    DWORD       dwLongDistanceRuleSize;                         // TAPI v1.4
    DWORD       dwLongDistanceRuleOffset;                       // TAPI v1.4
    DWORD       dwInternationalRuleSize;                        // TAPI v1.4
    DWORD       dwInternationalRuleOffset;                      // TAPI v1.4
    DWORD       dwOptions;                                      // TAPI v1.4

} LINECARDENTRY, FAR *LPLINECARDENTRY;

typedef struct linecountryentry_tag
{
    DWORD       dwCountryID;                                    // TAPI v1.4
    DWORD       dwCountryCode;                                  // TAPI v1.4
    DWORD       dwNextCountryID;                                // TAPI v1.4
    DWORD       dwCountryNameSize;                              // TAPI v1.4
    DWORD       dwCountryNameOffset;                            // TAPI v1.4
    DWORD       dwSameAreaRuleSize;                             // TAPI v1.4
    DWORD       dwSameAreaRuleOffset;                           // TAPI v1.4
    DWORD       dwLongDistanceRuleSize;                         // TAPI v1.4
    DWORD       dwLongDistanceRuleOffset;                       // TAPI v1.4
    DWORD       dwInternationalRuleSize;                        // TAPI v1.4
    DWORD       dwInternationalRuleOffset;                      // TAPI v1.4

} LINECOUNTRYENTRY, FAR *LPLINECOUNTRYENTRY;

typedef struct linecountrylist_tag
{
    DWORD       dwTotalSize;                                    // TAPI v1.4
    DWORD       dwNeededSize;                                   // TAPI v1.4
    DWORD       dwUsedSize;                                     // TAPI v1.4
    DWORD       dwNumCountries;                                 // TAPI v1.4
    DWORD       dwCountryListSize;                              // TAPI v1.4
    DWORD       dwCountryListOffset;                            // TAPI v1.4

} LINECOUNTRYLIST, FAR *LPLINECOUNTRYLIST;

typedef struct linedevcaps_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwProviderInfoSize;
    DWORD       dwProviderInfoOffset;
    DWORD       dwSwitchInfoSize;
    DWORD       dwSwitchInfoOffset;
    DWORD       dwPermanentLineID;
    DWORD       dwLineNameSize;
    DWORD       dwLineNameOffset;
    DWORD       dwStringFormat;
    DWORD       dwAddressModes;
    DWORD       dwNumAddresses;
    DWORD       dwBearerModes;
    DWORD       dwMaxRate;
    DWORD       dwMediaModes;
    DWORD       dwGenerateToneModes;
    DWORD       dwGenerateToneMaxNumFreq;
    DWORD       dwGenerateDigitModes;
    DWORD       dwMonitorToneMaxNumFreq;
    DWORD       dwMonitorToneMaxNumEntries;
    DWORD       dwMonitorDigitModes;
    DWORD       dwGatherDigitsMinTimeout;
    DWORD       dwGatherDigitsMaxTimeout;
    DWORD       dwMedCtlDigitMaxListSize;
    DWORD       dwMedCtlMediaMaxListSize;
    DWORD       dwMedCtlToneMaxListSize;
    DWORD       dwMedCtlCallStateMaxListSize;
    DWORD       dwDevCapFlags;
    DWORD       dwMaxNumActiveCalls;
    DWORD       dwAnswerMode;
    DWORD       dwRingModes;
    DWORD       dwLineStates;
    DWORD       dwUUIAcceptSize;
    DWORD       dwUUIAnswerSize;
    DWORD       dwUUIMakeCallSize;
    DWORD       dwUUIDropSize;
    DWORD       dwUUISendUserUserInfoSize;
    DWORD       dwUUICallInfoSize;
    LINEDIALPARAMS  MinDialParams;
    LINEDIALPARAMS  MaxDialParams;
    LINEDIALPARAMS  DefaultDialParams;
    DWORD       dwNumTerminals;
    DWORD       dwTerminalCapsSize;
    DWORD       dwTerminalCapsOffset;
    DWORD       dwTerminalTextEntrySize;
    DWORD       dwTerminalTextSize;
    DWORD       dwTerminalTextOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;

    DWORD       dwLineFeatures;                                 // TAPI v1.4

    DWORD       dwSettableDevStatus;                            // TAPI v2.0
    DWORD       dwDeviceClassesSize;                            // TAPI v2.0
    DWORD       dwDeviceClassesOffset;                          // TAPI v2.0
    GUID        PermanentLineGuid;                              // TAPI v2.2
    DWORD       dwAddressTypes;                                 // TAPI v3.0
    GUID        ProtocolGuid;                                   // TAPI v3.0
    DWORD       dwAvailableTracking;                            // TAPI v3.0

} LINEDEVCAPS, FAR *LPLINEDEVCAPS;
    
typedef struct linedevstatus_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwNumOpens;
    DWORD       dwOpenMediaModes;
    DWORD       dwNumActiveCalls;
    DWORD       dwNumOnHoldCalls;
    DWORD       dwNumOnHoldPendCalls;
    DWORD       dwLineFeatures;
    DWORD       dwNumCallCompletions;
    DWORD       dwRingMode;
    DWORD       dwSignalLevel;
    DWORD       dwBatteryLevel;
    DWORD       dwRoamMode;
    DWORD       dwDevStatusFlags;
    DWORD       dwTerminalModesSize;
    DWORD       dwTerminalModesOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;

    DWORD       dwAvailableMediaModes;                          // TAPI v2.0
    DWORD       dwAppInfoSize;                                  // TAPI v2.0
    DWORD       dwAppInfoOffset;                                // TAPI v2.0
} LINEDEVSTATUS, FAR *LPLINEDEVSTATUS;

typedef struct lineextensionid_tag
{
    DWORD       dwExtensionID0;
    DWORD       dwExtensionID1;
    DWORD       dwExtensionID2;
    DWORD       dwExtensionID3;

} LINEEXTENSIONID, FAR *LPLINEEXTENSIONID;

typedef struct lineforward_tag
{
    DWORD       dwForwardMode;
    DWORD       dwCallerAddressSize;
    DWORD       dwCallerAddressOffset;
    DWORD       dwDestCountryCode;
    DWORD       dwDestAddressSize;
    DWORD       dwDestAddressOffset;

} LINEFORWARD, FAR *LPLINEFORWARD;

typedef struct lineforwardlist_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNumEntries;
    LINEFORWARD ForwardList[1];

} LINEFORWARDLIST, FAR *LPLINEFORWARDLIST;

typedef struct linegeneratetone_tag
{
    DWORD       dwFrequency;
    DWORD       dwCadenceOn;
    DWORD       dwCadenceOff;
    DWORD       dwVolume;

} LINEGENERATETONE, FAR *LPLINEGENERATETONE;

typedef struct lineinitializeexparams_tag
{
    DWORD       dwTotalSize;                                    // TAPI v2.0
    DWORD       dwNeededSize;                                   // TAPI v2.0
    DWORD       dwUsedSize;                                     // TAPI v2.0
    DWORD       dwOptions;                                      // TAPI v2.0

    union
    {
        HANDLE  hEvent;                                         // TAPI v2.0
        HANDLE  hCompletionPort;                                // TAPI v2.0

    } Handles;

    DWORD       dwCompletionKey;                                // TAPI v2.0

} LINEINITIALIZEEXPARAMS, FAR *LPLINEINITIALIZEEXPARAMS;

typedef struct linelocationentry_tag
{
    DWORD       dwPermanentLocationID;
    DWORD       dwLocationNameSize;
    DWORD       dwLocationNameOffset;
    DWORD       dwCountryCode;
    DWORD       dwCityCodeSize;
    DWORD       dwCityCodeOffset;
    DWORD       dwPreferredCardID;

    DWORD       dwLocalAccessCodeSize;                          // TAPI v1.4
    DWORD       dwLocalAccessCodeOffset;                        // TAPI v1.4
    DWORD       dwLongDistanceAccessCodeSize;                   // TAPI v1.4
    DWORD       dwLongDistanceAccessCodeOffset;                 // TAPI v1.4
    DWORD       dwTollPrefixListSize;                           // TAPI v1.4
    DWORD       dwTollPrefixListOffset;                         // TAPI v1.4
    DWORD       dwCountryID;                                    // TAPI v1.4
    DWORD       dwOptions;                                      // TAPI v1.4
    DWORD       dwCancelCallWaitingSize;                        // TAPI v1.4
    DWORD       dwCancelCallWaitingOffset;                      // TAPI v1.4

} LINELOCATIONENTRY, FAR *LPLINELOCATIONENTRY;

typedef struct linemediacontrolcallstate_tag
{
    DWORD       dwCallStates;
    DWORD       dwMediaControl;

} LINEMEDIACONTROLCALLSTATE, FAR *LPLINEMEDIACONTROLCALLSTATE;

typedef struct linemediacontroldigit_tag
{
    DWORD       dwDigit;
    DWORD       dwDigitModes;
    DWORD       dwMediaControl;

} LINEMEDIACONTROLDIGIT, FAR *LPLINEMEDIACONTROLDIGIT;

typedef struct linemediacontrolmedia_tag
{
    DWORD       dwMediaModes;
    DWORD       dwDuration;
    DWORD       dwMediaControl;

} LINEMEDIACONTROLMEDIA, FAR *LPLINEMEDIACONTROLMEDIA;

typedef struct linemediacontroltone_tag
{
    DWORD       dwAppSpecific;
    DWORD       dwDuration;
    DWORD       dwFrequency1;
    DWORD       dwFrequency2;
    DWORD       dwFrequency3;
    DWORD       dwMediaControl;

} LINEMEDIACONTROLTONE, FAR *LPLINEMEDIACONTROLTONE;

typedef struct linemessage_tag
{
    DWORD       hDevice;                                        // TAPI v2.0
    DWORD       dwMessageID;                                    // TAPI v2.0
    DWORD       dwCallbackInstance;                             // TAPI v2.0
    DWORD       dwParam1;                                       // TAPI v2.0
    DWORD       dwParam2;                                       // TAPI v2.0
    DWORD       dwParam3;                                       // TAPI v2.0

} LINEMESSAGE, FAR *LPLINEMESSAGE;


typedef struct linemonitortone_tag
{
    DWORD       dwAppSpecific;
    DWORD       dwDuration;
    DWORD       dwFrequency1;
    DWORD       dwFrequency2;
    DWORD       dwFrequency3;

} LINEMONITORTONE, FAR *LPLINEMONITORTONE;

typedef struct lineproviderentry_tag
{
    DWORD       dwPermanentProviderID;                          // TAPI v1.4
    DWORD       dwProviderFilenameSize;                         // TAPI v1.4
    DWORD       dwProviderFilenameOffset;                       // TAPI v1.4

} LINEPROVIDERENTRY, FAR *LPLINEPROVIDERENTRY;

typedef struct lineproviderlist_tag
{
    DWORD       dwTotalSize;                                    // TAPI v1.4
    DWORD       dwNeededSize;                                   // TAPI v1.4
    DWORD       dwUsedSize;                                     // TAPI v1.4
    DWORD       dwNumProviders;                                 // TAPI v1.4
    DWORD       dwProviderListSize;                             // TAPI v1.4
    DWORD       dwProviderListOffset;                           // TAPI v1.4

} LINEPROVIDERLIST, FAR *LPLINEPROVIDERLIST;


typedef struct linetermcaps_tag
{
    DWORD       dwTermDev;
    DWORD       dwTermModes;
    DWORD       dwTermSharing;

} LINETERMCAPS, FAR *LPLINETERMCAPS;

typedef struct linetranslatecaps_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwNumLocations;
    DWORD       dwLocationListSize;
    DWORD       dwLocationListOffset;
    DWORD       dwCurrentLocationID;
    DWORD       dwNumCards;
    DWORD       dwCardListSize;
    DWORD       dwCardListOffset;
    DWORD       dwCurrentPreferredCardID;

} LINETRANSLATECAPS, FAR *LPLINETRANSLATECAPS;

typedef struct linetranslateoutput_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwDialableStringSize;
    DWORD       dwDialableStringOffset;
    DWORD       dwDisplayableStringSize;
    DWORD       dwDisplayableStringOffset;
    DWORD       dwCurrentCountry;
    DWORD       dwDestCountry;
    DWORD       dwTranslateResults;

} LINETRANSLATEOUTPUT, FAR *LPLINETRANSLATEOUTPUT;

typedef struct phonebuttoninfo_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwButtonMode;
    DWORD       dwButtonFunction;
    DWORD       dwButtonTextSize;
    DWORD       dwButtonTextOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;

    DWORD       dwButtonState;                                  // TAPI v1.4

} PHONEBUTTONINFO, FAR *LPPHONEBUTTONINFO;

typedef struct phonecaps_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwProviderInfoSize;
    DWORD       dwProviderInfoOffset;
    DWORD       dwPhoneInfoSize;
    DWORD       dwPhoneInfoOffset;
    DWORD       dwPermanentPhoneID;
    DWORD       dwPhoneNameSize;
    DWORD       dwPhoneNameOffset;
    DWORD       dwStringFormat;
    DWORD       dwPhoneStates;
    DWORD       dwHookSwitchDevs;
    DWORD       dwHandsetHookSwitchModes;
    DWORD       dwSpeakerHookSwitchModes;
    DWORD       dwHeadsetHookSwitchModes;
    DWORD       dwVolumeFlags;
    DWORD       dwGainFlags;
    DWORD       dwDisplayNumRows;
    DWORD       dwDisplayNumColumns;
    DWORD       dwNumRingModes;
    DWORD       dwNumButtonLamps;
    DWORD       dwButtonModesSize;
    DWORD       dwButtonModesOffset;
    DWORD       dwButtonFunctionsSize;
    DWORD       dwButtonFunctionsOffset;
    DWORD       dwLampModesSize;
    DWORD       dwLampModesOffset;
    DWORD       dwNumSetData;
    DWORD       dwSetDataSize;
    DWORD       dwSetDataOffset;
    DWORD       dwNumGetData;
    DWORD       dwGetDataSize;
    DWORD       dwGetDataOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;
    DWORD       dwDeviceClassesSize;                            // TAPI v2.0
    DWORD       dwDeviceClassesOffset;                          // TAPI v2.0
    DWORD       dwPhoneFeatures;                                // TAPI v2.0
    DWORD       dwSettableHandsetHookSwitchModes;               // TAPI v2.0
    DWORD       dwSettableSpeakerHookSwitchModes;               // TAPI v2.0
    DWORD       dwSettableHeadsetHookSwitchModes;               // TAPI v2.0
    DWORD       dwMonitoredHandsetHookSwitchModes;              // TAPI v2.0
    DWORD       dwMonitoredSpeakerHookSwitchModes;              // TAPI v2.0
    DWORD       dwMonitoredHeadsetHookSwitchModes;              // TAPI v2.0
    GUID        PermanentPhoneGuid;                             // TAPI v2.2
} PHONECAPS, FAR *LPPHONECAPS;

typedef struct phoneextensionid_tag
{
    DWORD       dwExtensionID0;
    DWORD       dwExtensionID1;
    DWORD       dwExtensionID2;
    DWORD       dwExtensionID3;

} PHONEEXTENSIONID, FAR *LPPHONEEXTENSIONID;

typedef struct phoneinitializeexparams_tag
{
    DWORD       dwTotalSize;                                    // TAPI v2.0
    DWORD       dwNeededSize;                                   // TAPI v2.0
    DWORD       dwUsedSize;                                     // TAPI v2.0
    DWORD       dwOptions;                                      // TAPI v2.0

    union
    {
        HANDLE  hEvent;                                         // TAPI v2.0
        HANDLE  hCompletionPort;                                // TAPI v2.0

    } Handles;

    DWORD       dwCompletionKey;                                // TAPI v2.0

} PHONEINITIALIZEEXPARAMS, FAR *LPPHONEINITIALIZEEXPARAMS;

typedef struct phonemessage_tag
{
    DWORD       hDevice;                                        // TAPI v2.0
    DWORD       dwMessageID;                                    // TAPI v2.0
    DWORD       dwCallbackInstance;                             // TAPI v2.0
    DWORD       dwParam1;                                       // TAPI v2.0
    DWORD       dwParam2;                                       // TAPI v2.0
    DWORD       dwParam3;                                       // TAPI v2.0

} PHONEMESSAGE, FAR *LPPHONEMESSAGE;

typedef struct phonestatus_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwStatusFlags;
    DWORD       dwNumOwners;
    DWORD       dwNumMonitors;
    DWORD       dwRingMode;
    DWORD       dwRingVolume;
    DWORD       dwHandsetHookSwitchMode;
    DWORD       dwHandsetVolume;
    DWORD       dwHandsetGain;
    DWORD       dwSpeakerHookSwitchMode;
    DWORD       dwSpeakerVolume;
    DWORD       dwSpeakerGain;
    DWORD       dwHeadsetHookSwitchMode;
    DWORD       dwHeadsetVolume;
    DWORD       dwHeadsetGain;
    DWORD       dwDisplaySize;
    DWORD       dwDisplayOffset;
    DWORD       dwLampModesSize;
    DWORD       dwLampModesOffset;
    DWORD       dwOwnerNameSize;
    DWORD       dwOwnerNameOffset;
    DWORD       dwDevSpecificSize;
    DWORD       dwDevSpecificOffset;

    DWORD       dwPhoneFeatures;                                // TAPI v2.0
} PHONESTATUS, FAR *LPPHONESTATUS;

typedef struct varstring_tag
{
    DWORD       dwTotalSize;
    DWORD       dwNeededSize;
    DWORD       dwUsedSize;
    DWORD       dwStringFormat;
    DWORD       dwStringSize;
    DWORD       dwStringOffset;

} VARSTRING, FAR *LPVARSTRING;


LONG
WINAPI
lineAccept(
    HCALL               hCall,
    LPCSTR              lpsUserUserInfo,
    DWORD               dwSize
    );

LONG
WINAPI
lineAddProvider(
    LPCWSTR             lpszProviderFilename,
    HWND                hwndOwner,
    LPDWORD             lpdwPermanentProviderID
    );


LONG
WINAPI
lineAddToConference(
    HCALL               hConfCall,
    HCALL               hConsultCall
    );

LONG
WINAPI
lineAnswer(
    HCALL               hCall,
    LPCSTR              lpsUserUserInfo,
    DWORD               dwSize
    );

LONG
WINAPI
lineBlindTransfer(
    HCALL               hCall,
    LPCWSTR             lpszDestAddressW,
    DWORD               dwCountryCode
    );

LONG
WINAPI
lineClose(
    HLINE               hLine
    );

LONG
WINAPI
lineCompleteTransfer(
    HCALL               hCall,
    HCALL               hConsultCall,
    LPHCALL             lphConfCall,
    DWORD               dwTransferMode
    );

LONG
WINAPI
lineConfigDialogEdit(
    DWORD               dwDeviceID,
    HWND                hwndOwner,
    LPCWSTR             lpszDeviceClass,
    LPVOID              const lpDeviceConfigIn,
    DWORD               dwSize,
    LPVARSTRING         lpDeviceConfigOut
    );

LONG
WINAPI
lineDeallocateCall(
    HCALL               hCall
    );

LONG
WINAPI
lineDevSpecific(
    HLINE               hLine,
    DWORD               dwAddressID,
    HCALL               hCall,
    LPVOID              lpParams,
    DWORD               dwSize
    );

LONG
WINAPI
lineDial(
    HCALL               hCall,
    LPCWSTR             lpszDestAddress,
    DWORD               dwCountryCode
    );

LONG
WINAPI
lineDrop(
    HCALL               hCall,
    LPCSTR              lpsUserUserInfo,
    DWORD               dwSize
    );

LONG
WINAPI
lineForward(
    HLINE               hLine,
    DWORD               bAllAddresses,
    DWORD               dwAddressID,
    LPLINEFORWARDLIST   const lpForwardList,
    DWORD               dwNumRingsNoAnswer,
    LPHCALL             lphConsultCall,
    LPLINECALLPARAMS    const lpCallParams
    );

LONG
WINAPI
lineGenerateDigits(
    HCALL               hCall,
    DWORD               dwDigitMode,
    LPCWSTR             lpszDigits,
    DWORD               dwDuration
    );

LONG
WINAPI
lineGenerateTone(
    HCALL               hCall,
    DWORD               dwToneMode,
    DWORD               dwDuration,
    DWORD               dwNumTones,
    LPLINEGENERATETONE  const lpTones
    );

LONG
WINAPI
lineGetAddressCaps(
    HLINEAPP            hLineApp,
    DWORD               dwDeviceID,
    DWORD               dwAddressID,
    DWORD               dwAPIVersion,
    DWORD               dwExtVersion,
    LPLINEADDRESSCAPS   lpAddressCaps
    );

LONG
WINAPI
lineGetAddressID(
    HLINE               hLine,
    LPDWORD             lpdwAddressID,
    DWORD               dwAddressMode,
    LPCWSTR             lpsAddress,
    DWORD               dwSize
    );

LONG
WINAPI
lineGetAddressStatus(
    HLINE               hLine,
    DWORD               dwAddressID,
    LPLINEADDRESSSTATUS lpAddressStatus
    );


LONG
WINAPI
lineGetAppPriority(                                             // TAPI v1.4
    LPCWSTR             lpszAppFilename,
    DWORD               dwMediaMode,
    LPLINEEXTENSIONID   lpExtensionID,
    DWORD               dwRequestMode,
    LPVARSTRING         lpExtensionName,
    LPDWORD             lpdwPriority
    );

LONG
WINAPI
lineGetCallInfo(
    HCALL               hCall,
    LPLINECALLINFO      lpCallInfo
    );


LONG
WINAPI
lineGetCallStatus(
    HCALL               hCall,
    LPLINECALLSTATUS    lpCallStatus
    );

LONG
WINAPI
lineGetConfRelatedCalls(
    HCALL               hCall,
    LPLINECALLLIST      lpCallList
    );
    
LONG
WINAPI
lineGetDevCaps(
    HLINEAPP            hLineApp,
    DWORD               dwDeviceID,
    DWORD               dwAPIVersion,
    DWORD               dwExtVersion,
    LPLINEDEVCAPS       lpLineDevCaps
    );

LONG
WINAPI
lineGetDevConfig(
    DWORD               dwDeviceID,
    LPVARSTRING         lpDeviceConfig,
    LPCWSTR             lpszDeviceClass
    );

LONG
WINAPI
lineGetNewCalls(
    HLINE               hLine,
    DWORD               dwAddressID,
    DWORD               dwSelect,
    LPLINECALLLIST      lpCallList
    );

LONG
WINAPI
lineGetIcon(
    DWORD               dwDeviceID,
    LPCWSTR             lpszDeviceClass,
    LPHICON             lphIcon
    );

LONG
WINAPI
lineGetID(
    HLINE               hLine,
    DWORD               dwAddressID,
    HCALL               hCall,
    DWORD               dwSelect,
    LPVARSTRING         lpDeviceID,
    LPCWSTR             lpszDeviceClass
    );

LONG
WINAPI
lineGetLineDevStatus(
    HLINE               hLine,
    LPLINEDEVSTATUS     lpLineDevStatus
    );


LONG
WINAPI
lineGetMessage(
    HLINEAPP        hLineApp,
    LPLINEMESSAGE   lpMessage,
    DWORD           dwTimeout
    );

LONG
WINAPI
lineGetNumRings(
    HLINE               hLine,
    DWORD               dwAddressID,
    LPDWORD             lpdwNumRings
    );

LONG
WINAPI
lineGetProviderList(                                            // TAPI v1.4
    DWORD               dwAPIVersion,
    LPLINEPROVIDERLIST  lpProviderList
    );


LONG
WINAPI
lineGetStatusMessages(
    HLINE               hLine,
    LPDWORD             lpdwLineStates,
    LPDWORD             lpdwAddressStates
    );

LONG
WINAPI
lineGetTranslateCaps(
    HLINEAPP hLineApp,
    DWORD dwAPIVersion,
    LPLINETRANSLATECAPS lpTranslateCaps
    );

LONG
WINAPI
lineHandoff(
    HCALL               hCall,
    LPCWSTR             lpszFileName,
    DWORD               dwMediaMode
    );

LONG
WINAPI
lineHold(
    HCALL               hCall
    );

LONG
WINAPI
lineInitialize(
    LPHLINEAPP          lphLineApp,
    HINSTANCE           hInstance,
    LINECALLBACK        lpfnCallback,
    LPCWSTR             lpszAppName,
    LPDWORD             lpdwNumDevs
    );

LONG
WINAPI
lineInitializeEx(
    LPHLINEAPP                  lphLineApp,
    HINSTANCE                   hInstance,
    LINECALLBACK                lpfnCallback,
    LPCWSTR                     lpszFriendlyAppName,
    LPDWORD                     lpdwNumDevs,
    LPDWORD                     lpdwAPIVersion,
    LPLINEINITIALIZEEXPARAMS    lpLineInitializeExParams
    );

LONG
WINAPI
lineMakeCall(
    HLINE               hLine,
    LPHCALL             lphCall,
    LPCWSTR             lpszDestAddress,
    DWORD               dwCountryCode,
    LPLINECALLPARAMS    const lpCallParams
    );


LONG
WINAPI
lineMonitorDigits(
    HCALL               hCall,
    DWORD               dwDigitModes
    );

LONG
WINAPI
lineMonitorMedia(
    HCALL               hCall,
    DWORD               dwMediaModes
    );

LONG
WINAPI
lineNegotiateAPIVersion(
    HLINEAPP            hLineApp,
    DWORD               dwDeviceID,
    DWORD               dwAPILowVersion,
    DWORD               dwAPIHighVersion,
    LPDWORD             lpdwAPIVersion,
    LPLINEEXTENSIONID   lpExtensionID
    );

LONG
WINAPI
lineNegotiateExtVersion(
    HLINEAPP            hLineApp,
    DWORD               dwDeviceID,
    DWORD               dwAPIVersion,
    DWORD               dwExtLowVersion,
    DWORD               dwExtHighVersion,
    LPDWORD             lpdwExtVersion
    );

LONG
WINAPI
lineOpen(
    HLINEAPP hLineApp, 
    DWORD dwDeviceID,
    LPHLINE lphLine,
    DWORD               dwAPIVersion,
    DWORD               dwExtVersion,
    DWORD               dwCallbackInstance,
    DWORD               dwPrivileges,
    DWORD               dwMediaModes,
    LPLINECALLPARAMS    const lpCallParams
    );

LONG
WINAPI
linePickup(
    HLINE               hLine,
    DWORD               dwAddressID,
    LPHCALL             lphCall,
    LPCWSTR             lpszDestAddress,
    LPCWSTR             lpszGroupID
    );

LONG
WINAPI
linePrepareAddToConference(
    HCALL               hConfCall,
    LPHCALL             lphConsultCall,
    LPLINECALLPARAMS    const lpCallParams
    );


LONG
WINAPI
lineRedirect(
    HCALL               hCall,
    LPCWSTR             lpszDestAddress,
    DWORD               dwCountryCode
    );

LONG
WINAPI
lineReleaseUserUserInfo(                                        // TAPI v1.4
    HCALL               hCall
    );

LONG
WINAPI
lineRemoveFromConference(
    HCALL               hCall
    );

LONG
WINAPI
lineSendUserUserInfo(
    HCALL               hCall,
    LPCSTR              lpsUserUserInfo,
    DWORD               dwSize
    );

LONG
WINAPI
lineSetAppPriority(                                             // TAPI v1.4
    LPCWSTR             lpszAppFilename,
    DWORD               dwMediaMode,
    LPLINEEXTENSIONID   lpExtensionID,
    DWORD               dwRequestMode,
    LPCWSTR             lpszExtensionName,
    DWORD               dwPriority
    );


LONG
WINAPI
lineSetCallParams(
    HCALL               hCall,
    DWORD               dwBearerMode,
    DWORD               dwMinRate,
    DWORD               dwMaxRate,
    LPLINEDIALPARAMS    const lpDialParams
    );

LONG
WINAPI
lineSetCallPrivilege(
    HCALL               hCall,
    DWORD               dwCallPrivilege
    );


LONG
WINAPI
lineSetCurrentLocation(
    HLINEAPP            hLineApp,
    DWORD               dwLocation
    );

LONG
WINAPI
lineSetDevConfig(
    DWORD               dwDeviceID,
    LPVOID              const lpDeviceConfig,
    DWORD               dwSize,
    LPCWSTR             lpszDeviceClass
    );

LONG
WINAPI
lineSetMediaMode(
    HCALL               hCall,
    DWORD               dwMediaModes
    );

LONG
WINAPI
lineSetNumRings(
    HLINE               hLine,
    DWORD               dwAddressID,
    DWORD               dwNumRings
    );

LONG
WINAPI
lineSetStatusMessages(
    HLINE               hLine,
    DWORD               dwLineStates,
    DWORD               dwAddressStates
    );

LONG
WINAPI
lineSetTerminal(
    HLINE               hLine,
    DWORD               dwAddressID,
    HCALL               hCall,
    DWORD               dwSelect,
    DWORD               dwTerminalModes,
    DWORD               dwTerminalID,
    DWORD               bEnable
    );

LONG
WINAPI
lineSetTollList(
    HLINEAPP            hLineApp,
    DWORD               dwDeviceID,
    LPCWSTR             lpszAddressIn,
    DWORD               dwTollListOption
    );


LONG
WINAPI
lineSetupConference(
    HCALL               hCall,
    HLINE               hLine,
    LPHCALL             lphConfCall,
    LPHCALL             lphConsultCall,
    DWORD               dwNumParties,
    LPLINECALLPARAMS    const lpCallParams
    );

LONG
WINAPI
lineSetupTransfer(
    HCALL               hCall,
    LPHCALL             lphConsultCall,
    LPLINECALLPARAMS    const lpCallParams
    );

LONG
WINAPI
lineShutdown(
    HLINEAPP            hLineApp
    );

LONG
WINAPI
lineSwapHold(
    HCALL               hActiveCall,
    HCALL               hHeldCall
    );

LONG
WINAPI
lineTranslateAddress(
    HLINEAPP                hLineApp,
    DWORD                   dwDeviceID,
    DWORD                   dwAPIVersion,
    LPCWSTR                 lpszAddressIn,
    DWORD                   dwCard,
    DWORD                   dwTranslateOptions,
    LPLINETRANSLATEOUTPUT   lpTranslateOutput
    );


LONG
WINAPI
lineTranslateDialog(                                            // TAPI v1.4
    HLINEAPP            hLineApp,
    DWORD               dwDeviceID,
    DWORD               dwAPIVersion,
    HWND                hwndOwner,
    LPCWSTR             lpszAddressIn
    );

LONG
WINAPI
lineUnhold(
    HCALL               hCall
    );



LONG
WINAPI
phoneClose(
    HPHONE              hPhone
    );

LONG
WINAPI
phoneConfigDialog(
    DWORD               dwDeviceID,
    HWND                hwndOwner,
    LPCWSTR             lpszDeviceClass
    );

LONG
WINAPI
phoneDevSpecific(
    HPHONE hPhone,
    LPVOID lpParams,
    DWORD dwSize
    );

LONG
WINAPI
phoneGetDevCaps(
    HPHONEAPP           hPhoneApp,
    DWORD               dwDeviceID,
    DWORD               dwAPIVersion,
    DWORD               dwExtVersion,
    LPPHONECAPS         lpPhoneCaps
    );

LONG
WINAPI
phoneGetGain(
    HPHONE              hPhone,
    DWORD               dwHookSwitchDev,
    LPDWORD             lpdwGain
    );

LONG
WINAPI
phoneGetHookSwitch(
    HPHONE              hPhone,
    LPDWORD             lpdwHookSwitchDevs
    );

LONG
WINAPI
phoneGetIcon(
    DWORD               dwDeviceID,
    LPCWSTR             lpszDeviceClass,
    LPHICON             lphIcon
    );


LONG
WINAPI
phoneGetID(
    HPHONE              hPhone,
    LPVARSTRING         lpDeviceID,
    LPCWSTR             lpszDeviceClass
    );

LONG
WINAPI
phoneGetMessage(
    HPHONEAPP       hPhoneApp,
    LPPHONEMESSAGE  lpMessage,
    DWORD           dwTimeout
    );

LONG
WINAPI
phoneGetRing(
    HPHONE              hPhone,
    LPDWORD             lpdwRingMode,
    LPDWORD             lpdwVolume
    );

LONG
WINAPI
phoneGetStatus(
    HPHONE              hPhone,
    LPPHONESTATUS       lpPhoneStatus
    );

LONG
WINAPI
phoneGetStatusMessages(
    HPHONE              hPhone,
    LPDWORD             lpdwPhoneStates,
    LPDWORD             lpdwButtonModes,
    LPDWORD             lpdwButtonStates
    );

LONG
WINAPI
phoneGetVolume(
    HPHONE              hPhone,
    DWORD               dwHookSwitchDev,
    LPDWORD             lpdwVolume
    );

LONG
WINAPI
phoneInitializeEx(
    LPHPHONEAPP                 lphPhoneApp,
    HINSTANCE                   hInstance,
    PHONECALLBACK               lpfnCallback,
    LPCWSTR                     lpszFriendlyAppName,
    LPDWORD                     lpdwNumDevs,
    LPDWORD                     lpdwAPIVersion,
    LPPHONEINITIALIZEEXPARAMS   lpPhoneInitializeExParams
    );

LONG
WINAPI
phoneNegotiateAPIVersion(
    HPHONEAPP           hPhoneApp,
    DWORD               dwDeviceID,
    DWORD               dwAPILowVersion,
    DWORD               dwAPIHighVersion,
    LPDWORD             lpdwAPIVersion,
    LPPHONEEXTENSIONID  lpExtensionID
    );


LONG
WINAPI
phoneNegotiateExtVersion(
    HPHONEAPP           hPhoneApp,
    DWORD               dwDeviceID,
    DWORD               dwAPIVersion,
    DWORD               dwExtLowVersion,
    DWORD               dwExtHighVersion,
    LPDWORD             lpdwExtVersion
    );

LONG
WINAPI
phoneOpen(
    HPHONEAPP           hPhoneApp,
    DWORD               dwDeviceID,
    LPHPHONE            lphPhone,
    DWORD               dwAPIVersion,
    DWORD               dwExtVersion,
    DWORD               dwCallbackInstance,
    DWORD               dwPrivilege
    );

LONG
WINAPI
phoneSetGain(
    HPHONE              hPhone,
    DWORD               dwHookSwitchDev,
    DWORD               dwGain
    );

LONG
WINAPI
phoneSetHookSwitch(
    HPHONE              hPhone,
    DWORD               dwHookSwitchDevs,
    DWORD               dwHookSwitchMode
    );

LONG
WINAPI
phoneSetRing(
    HPHONE              hPhone,
    DWORD               dwRingMode,
    DWORD               dwVolume
    );

LONG
WINAPI
phoneSetStatusMessages(
    HPHONE              hPhone,
    DWORD               dwPhoneStates,
    DWORD               dwButtonModes,
    DWORD               dwButtonStates
    );

LONG
WINAPI
phoneSetVolume(
    HPHONE              hPhone,
    DWORD               dwHookSwitchDev,
    DWORD               dwVolume
    );

LONG
WINAPI
phoneShutdown(
    HPHONEAPP           hPhoneApp
    );



/*

TAPIERROR_FORMATMESSAGE - macro to convert a TAPI error constant
    into a constant that FormatMessage will accept
    
        TAPIERR : Negative numbers and 0
            Map to : strip off high WORD
            Example: 0xFFFFFFFF (-1) becomes 0x0000FFFF
        LINEERR : Start at 0x80000000
            Map to : strip off 0x80000000 and add 0xE000
            Example: 0x80000004 becomes 0x0000E004
        PHONEERR: Start at 0x90000000
            Map to : strip off 0x90000000 and add 0xF000
            Example: 0x9000000A becomes 0x0000F00A

        pseudocode:
        
        if (__ErrCode__ is a TAPIERR)
            strip off high word

            else if (__ErrCode__ is a PHONEERR)
                strip off 0x90000000
                add 0xE000

                else
                    strip off 0x80000000
                    add 0xF000

*/

#define TAPIERROR_FORMATMESSAGE(__ErrCode__)                                                   \
                                                                                               \
                    ( ( (__ErrCode__) > 0xFFFF0000 ) ?                                         \
                                                                                               \
                            ( (__ErrCode__) & 0x0000FFFF ) :                                   \
                                                                                               \
                            ( ( (__ErrCode__) & 0x10000000 ) ?                                 \
                                                                                               \
                              ( (__ErrCode__) - 0x90000000 + 0xF000 ) :                        \
                                                                                               \
                              ( (__ErrCode__) - 0x80000000 + 0xE000 ) ) )


//
// Define unicode forms of API functions
//
#define lineAddProviderW        lineAddProvider
#define lineBlindTransferW      lineBlindTransfer
#define lineConfigDialogEditW   lineConfigDialogEdit
#define lineDialW               lineDial
#define lineForwardW            lineForward
#define lineGenerateDigitsW     lineGenerateDigits
#define lineGetAddressCapsW     lineGetAddressCaps
#define lineGetAddressIDW       lineGetAddressID
#define lineGetAddressStatusW   lineGetAddressStatus
#define lineGetAppPriorityW     lineGetAppPriority
#define lineGetCallInfoW        lineGetCallInfo
#define lineGetDevCapsW         lineGetDevCaps
#define lineGetDevConfigW       lineGetDevConfig
#define lineGetIconW            lineGetIcon
#define lineGetIDW              lineGetID
#define lineGetProviderListW    lineGetProviderList
#define lineGetTranslateCapsW   lineGetTranslateCaps
#define lineHandoffW            lineHandoff
#define lineInitializeExW       lineInitializeEx
#define lineMakeCallW           lineMakeCall
#define lineOpenW               lineOpen
#define linePickupW             linePickup
#define linePrepareAddToConferenceW linePrepareAddToConference
#define lineRedirectW           lineRedirect
#define lineSetAppPriorityW     lineSetAppPriority
#define lineSetDevConfigW       lineSetDevConfig
#define lineSetTollListW        lineSetTollList
#define lineSetupConferenceW    lineSetupConference
#define lineSetupTransferW      lineSetupTransfer
#define lineTranslateAddressW   lineTranslateAddress
#define lineTranslateDialogW    lineTranslateDialog
#define phoneConfigDialogW      phoneConfigDialog
#define phoneGetDevCapsW        phoneGetDevCaps
#define phoneGetIconW           phoneGetIcon
#define phoneGetIDW             phoneGetID
#define phoneGetStatusW         phoneGetStatus
#define phoneInitializeExW      phoneInitializeEx


#if defined(WINCEOEM) && defined(UNDER_CE)
#include <ptapi.h>
#if defined(WINCEMACRO) || defined(WINCEMACROTAPI)
#include <mtapi.h>
#endif
#endif


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */

#endif // TAPI_H
