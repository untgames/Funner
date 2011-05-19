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
//
//  Messages for Windows CE Event logging
//
//
// Event Log messages
//
// ReportEventFormat() uses this message ID with an sprintf format identifier to allow
// applications to pass in sprintf style strings to eventlog                           
//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SERVICES                0x2
#define FACILITY_EVENT_LOG               0x1


//
// Define the severity codes
//


//
// MessageId: EVENT_LOG_FREEFORM_MSG
//
// MessageText:
//
//  %1%0
//
#define EVENT_LOG_FREEFORM_MSG           0x00010001L

//
// MessageId: EVENT_LOG_UNKNOWN_MSG
//
// MessageText:
//
//  The description for Event ID ( %1 ) in Source ( %2 ) cannot be found. The local computer may not have the necessary registry information.  The following information is part of the event: %3 %0
//
#define EVENT_LOG_UNKNOWN_MSG            0xC0010002L

//
// MessageId: EVENT_LOG_OSVER_MSG
//
// MessageText:
//
//  Microsoft (R) Windows CE (R) %1 %2.%0
//
#define EVENT_LOG_OSVER_MSG              0x00010003L

//
// MessageId: EVENT_LOG_STARTUP_MSG
//
// MessageText:
//
//  The Event log service was started.%0
//
#define EVENT_LOG_STARTUP_MSG            0x00010004L

//
// MessageId: EVENT_LOG_STOP_MSG
//
// MessageText:
//
//  The Event log service was stopped.%0
//
#define EVENT_LOG_STOP_MSG               0x00010005L

//
// Generic Services messages
//
//
// MessageId: SERVICES_LOAD_SUCCEED_MSG
//
// MessageText:
//
//  The %1 service has loaded successfully.%0
//
#define SERVICES_LOAD_SUCCEED_MSG        0x00020001L

//
// MessageId: SERVICES_LOAD_FAILED_MSG
//
// MessageText:
//
//  The %1 service failed to load due to the following error:
//  %2%0
//
#define SERVICES_LOAD_FAILED_MSG         0xC0020002L

//
// MessageId: SERVICES_SHUTDOWN_SUCCEED_MSG
//
// MessageText:
//
//  The %1 service has shutdown successfully.%0
//
#define SERVICES_SHUTDOWN_SUCCEED_MSG    0x00020003L

//
// MessageId: SERVICES_SHUTDOWN_FAILED_MSG
//
// MessageText:
//
//  The %1 service failed to shutdown due to the following error: 
//  %2%0
//
#define SERVICES_SHUTDOWN_FAILED_MSG     0xC0020004L

//
// MessageId: SERVICES_RESUME_SUCCEED_MSG
//
// MessageText:
//
//  The %1 service has successfully resumed.%0
//
#define SERVICES_RESUME_SUCCEED_MSG      0x00020005L

//
// MessageId: SERVICES_RESUME_FAILED_MSG
//
// MessageText:
//
//  The %1 service failed to resume due to the following error:
//  %2%0
//
#define SERVICES_RESUME_FAILED_MSG       0xC0020006L

//
// MessageId: SERVICES_PAUSE_SUCCEED_MSG
//
// MessageText:
//
//  The %1 service has successfully paused.%0
//
#define SERVICES_PAUSE_SUCCEED_MSG       0x00020007L

//
// MessageId: SERVICES_PAUSE_FAILED_MSG
//
// MessageText:
//
//  The %1 service failed to pause due to the following error:
//  %2%0
//
#define SERVICES_PAUSE_FAILED_MSG        0xC0020008L

//
// MessageId: SERVICES_USER_LOGON_MSG
//
// MessageText:
//
//  User '%1' has logged onto the service.%0
//
#define SERVICES_USER_LOGON_MSG          0x00020009L

//
// MessageId: SERVICES_USER_LOGOFF_MSG
//
// MessageText:
//
//  User '%1' has logged off the service.%0
//
#define SERVICES_USER_LOGOFF_MSG         0x0002000AL

//
// MessageId: SERVICES_USER_LOGON_FAIL_MSG
//
// MessageText:
//
//  User '%1' failed to logon to the service due to the following error:
//  %2.%0
//
#define SERVICES_USER_LOGON_FAIL_MSG     0xC002000BL

//
// MessageId: SERVICES_USER_LOGON_IP_MSG
//
// MessageText:
//
//  User '%1' on machine '%2' has logged onto the service.%0
//
#define SERVICES_USER_LOGON_IP_MSG       0x0002000CL

//
// MessageId: SERVICES_USER_LOGON_IP_FAIL_MSG
//
// MessageText:
//
//  User '%1' on machine '%2' failed to logon to the service due to the following error:
//  %3.%0
//
#define SERVICES_USER_LOGON_IP_FAIL_MSG  0xC002000DL

