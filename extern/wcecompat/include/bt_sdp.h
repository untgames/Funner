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
//------------------------------------------------------------------------------
// 
//      Bluetooth Test Client
// 
// 
// Module Name:
// 
//      bt_sdp.h
// 
// Abstract:
// 
//      This file contains constants and structures for Simple Discovery Protocol.
// 
// 
//------------------------------------------------------------------------------

#if ! defined (__bt_sdp_H__)
#define __bt_sdp_H__		1

#define SDP_ERROR_INVALID_SDP_VERSION           0x0001
#define SDP_ERROR_INVALID_RECORD_HANDLE         0x0002
#define SDP_ERROR_INVALID_REQUEST_SYNTAX        0x0003
#define SDP_ERROR_INVALID_PDU_SIZE              0x0004
#define SDP_ERROR_INVALID_CONTINUATION_STATE    0x0005
#define SDP_ERROR_INSUFFICIENT_RESOURCES        0x0006

#define SDP_ATTRIB_RECORD_HANDLE            0x0000
#define SDP_ATTRIB_CLASS_ID_LIST            0x0001
#define SDP_ATTRIB_RECORD_STATE             0x0002
#define SDP_ATTRIB_SERVICE_ID               0x0003
#define SDP_ATTRIB_PROTOCOL_DESCRIPTOR_LIST 0x0004
#define SDP_ATTRIB_BROWSE_GROUP_LIST        0x0005
#define SDP_ATTRIB_LANG_BASE_ATTRIB_ID_LIST 0x0006
#define SDP_ATTRIB_INFO_TIME_TO_LIVE        0x0007
#define SDP_ATTRIB_AVAILABILITY             0x0008
#define SDP_ATTRIB_PROFILE_DESCRIPTOR_LIST  0x0009
#define SDP_ATTRIB_DOCUMENTATION_URL        0x000A
#define SDP_ATTRIB_CLIENT_EXECUTABLE_URL    0x000B
#define SDP_ATTRIB_ICON_URL                 0x000C

//
// Attribute IDs in the range of 0x000D - 0x01FF are reserved for future use
//
#define SDP_ATTRIB_PROFILE_SPECIFIC                     0x0200

#define STRING_NAME_OFFSET                              0x0000
#define STRING_DESCRIPTION_OFFSET                       0x0001
#define STRING_PROVIDER_NAME_OFFSET                     0x0002

#define SDP_ATTRIB_SDP_VERSION_NUMBER_LIST              0x0200 
#define SDP_ATTRIB_SDP_DATABASE_STATE                   0x0201

#define SDP_ATTRIB_BROWSE_GROUP_ID                      0x0200 

#define SDP_ATTRIB_CORDLESS_EXTERNAL_NETWORK            0x0301

#define SDP_ATTRIB_FAX_CLASS_1_SUPPORT                  0x0302
#define SDP_ATTRIB_FAX_CLASS_2_0_SUPPORT                0x0303
#define SDP_ATTRIB_FAX_CLASS_2_SUPPORT                  0x0304
#define SDP_ATTRIB_FAX_AUDIO_FEEDBACK_SUPPORT           0x0305

#define SDP_ATTRIB_HEADSET_REMOTE_AUDIO_VOLUME_CONTROL  0x0302

#define SDP_ATTRIB_LAN_LPSUBNET                         0x0200

#define SDP_ATTRIB_OBJECT_PUSH_SUPPORTED_FORMATS_LIST   0x0303

#define SDP_ATTRIB_SYNCH_SUPPORTED_DATA_STORES_LIST     0x0301

//  this is in the assigned numbers doc, but it does not show up in any profile
#define SDP_ATTRIB_SERVICE_VERSION                      0x0300

#define SDP_ATTRIB_PAN_SECURITY_DESCRIPTION             0x030A
#define SDP_ATTRIB_PAN_NET_ACCESS_TYPE                  0x030B
#define SDP_ATTRIB_PAN_MAX_NET_ACCESS_RATE              0x030C
#define SDP_ATTRIB_PAN_IPV4_SUBNET                      0x030D
#define SDP_ATTRIB_PAN_IPV6_SUBNET                      0x030E


// Bluetooth base UUID for service discovery
//
//DEFINE_GUID(Bluetooth_Base_UUID,   0x00000000, 0x0000, 0x1000, 0x70, 0x07, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(Bluetooth_Base_UUID,   0x00000000, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);

DEFINE_GUID(SDP_PROTOCOL_UUID,    0x00000001, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(UDP_PROTOCOL_UUID,    0x00000002, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(RFCOMM_PROTOCOL_UUID, 0x00000003, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(TCP_PROTOCOL_UUID,    0x00000004, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(TCSBIN_PROTOCOL_UUID, 0x00000005, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(TCSAT_PROTOCOL_UUID,  0x00000006, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(OBEX_PROTOCOL_UUID,   0x00000008, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(IP_PROTOCOL_UUID,     0x00000009, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(FTP_PROTOCOL_UUID,    0x0000000A, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(HTTP_PROTOCOL_UUID,   0x0000000C, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(WSP_PROTOCOL_UUID,    0x0000000E, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(BNEP_PROTOCOL_UUID,   0x0000000F, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(L2CAP_PROTOCOL_UUID,  0x00000100, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);

DEFINE_GUID(ServiceDiscoveryServerServiceClassID_UUID, 0x00001000, 0x0000, 0x1000, 0x80, 0x08, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(BrowseGroupDescriptorServiceClassID_UUID,  0x00001001, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(PublicBrowseGroupServiceClass_UUID,        0x00001002, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(SerialPortServiceClass_UUID,               0x00001101, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(LANAccessUsingPPPServiceClass_UUID,        0x00001102, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(DialupNetworkingServiceClass_UUID,         0x00001103, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(IrMCSyncServiceClass_UUID,                 0x00001104, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(OBEXObjectPushServiceClass_UUID,           0x00001105, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(OBEXFileTransferServiceClass_UUID,         0x00001106, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(IrMCSyncCommandServiceClass_UUID,          0x00001107, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(HeadsetServiceClass_UUID,                  0x00001108, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(CordlessTelephonyServiceClass_UUID,        0x00001109, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(IntercomServiceClass_UUID,                 0x00001110, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(FaxServiceClass_UUID,                      0x00001111, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(HeadsetAudioGatewayServiceClass_UUID,      0x00001112, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(PANUServiceClass_UUID,                     0x00001115, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(NAPServiceClass_UUID,                      0x00001116, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(GNServiceClass_UUID,                       0x00001117, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(HandsfreeServiceClass_UUID,                0x0000111E, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(HandsfreeAudioGatewayServiceClass_UUID,    0x0000111F, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(PnPInformationServiceClass_UUID,           0x00001200, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(GenericNetworkingServiceClass_UUID,        0x00001201, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(GenericFileTransferServiceClass_UUID,      0x00001202, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(GenericAudioServiceClass_UUID,             0x00001203, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);
DEFINE_GUID(GenericTelephonyServiceClass_UUID,         0x00001204, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB);

#define SDP_PROTOCOL_UUID16     0x0001
#define UDP_PROTOCOL_UUID16     0x0002
#define RFCOMM_PROTOCOL_UUID16  0x0003
#define TCP_PROTOCOL_UUID16     0x0004
#define TCSBIN_PROTOCOL_UUID16  0x0005
#define TCSAT_PROTOCOL_UUID16   0x0006
#define OBEX_PROTOCOL_UUID16    0x0008
#define IP_PROTOCOL_UUID16      0x0009
#define FTP_PROTOCOL_UUID16     0x000A
#define HTTP_PROTOCOL_UUID16    0x000C
#define WSP_PROTOCOL_UUID16     0x000E
#define BNEP_PROTOCOL_UUID16    0x000F
#define L2CAP_PROTOCOL_UUID16   0x0100

#define ServiceDiscoveryServerServiceClassID_UUID16     (0x1000)
#define BrowseGroupDescriptorServiceClassID_UUID16      (0x1001)
#define PublicBrowseGroupServiceClassID_UUID16          (0x1002)
#define SerialPortServiceClassID_UUID16                 (0x1101)
#define LANAccessUsingPPPServiceClassID_UUID16          (0x1102)
#define DialupNetworkingServiceClassID_UUID16           (0x1103)
#define IrMCSyncServiceClassID_UUID16                   (0x1104)
#define OBEXObjectPushServiceClassID_UUID16             (0x1105)
#define OBEXFileTransferServiceClassID_UUID16           (0x1106)
#define IrMcSyncCommandServiceClassID_UUID16            (0x1107)
#define HeadsetServiceClassID_UUID16                    (0x1108)
#define CordlessServiceClassID_UUID16                   (0x1109)
#define IntercomServiceClassID_UUID16                   (0x1110)
#define FaxServiceClassID_UUID16                        (0x1111)
#define HeadsetAudioGatewayServiceClassID_UUID16        (0x1112)
#define PANUServiceClassID_UUID16                       (0x1115)
#define NAPServiceClassID_UUID16                        (0x1116)
#define GNServiceClassID_UUID16                         (0x1117)
#define HandsfreeServiceClassID_UUID16                  (0x111E)
#define HandsfreeAudioGatewayServiceClassID_UUID16      (0x111F)
#define PnPInformationServiceClassID_UUID16             (0x1200)
#define GenericNetworkingServiceClassID_UUID16          (0x1201)
#define GenericFileTransferServiceClassID_UUID16        (0x1202)
#define GenericAudioServiceClassID_UUID16               (0x1203)
#define GenericTelephonyServiceClassID_UUID16           (0x1204)

#define PSM_SDP                 0x0001
#define PSM_RFCOMM              0x0003
#define PSM_TCS_BIN             0x0005
#define PSM_TCS_BIN_CORDLESS    0x0007


// HID specific SDP attrib ids.
#define SDP_ATTRIB_HID_DEVICE_RELEASE_NUMBER    0x0200
#define SDP_ATTRIB_HID_PARSER_VERSION           0x0201
#define SDP_ATTRIB_HID_DEVICE_SUBCLASS          0x0202
#define SDP_ATTRIB_HID_COUNTRY_CODE             0x0203
#define SDP_ATTRIB_HID_VIRTUAL_CABLE            0x0204
#define SDP_ATTRIB_HID_RECONNECT_INITIATE       0x0205
#define SDP_ATTRIB_HID_DESCRIPTOR_LIST          0x0206
#define SDP_ATTRIB_HID_LANGID_BASE_LIST         0x0207
#define SDP_ATTRIB_HID_SDP_DISABLE              0x0208
#define SDP_ATTRIB_HID_BATTERY_POWER            0x0209
#define SDP_ATTRIB_HID_REMOTE_WAKE              0x020A
#define SDP_ATTRIB_HID_PROFILE_VERSION          0x020B
#define SDP_ATTRIB_HID_SUPERVISION_TIMEOUT      0x020C
#define SDP_ATTRIB_HID_NORMALLY_CONNECTABLE     0x020D
#define SDP_ATTRIB_HID_BOOT_DEVICE              0x020E

#endif		/* __bt_sdp_H__ */
