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

#ifndef _FW6_INCLUDED_
#define _FW6_INCLUDED_

#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef DWORD_PTR HRULE;

typedef enum _FW_RULE_MASKS
{
    FWM_PRIVATE_HOST =       0x0001,
    FWM_PUBLIC_HOST =        0x0002,
    FWM_PORT =               0x0004,
    FWM_TYPE =               0x0008,
    FWM_CODE =               0x0010,
    FWM_PROTOCOL =           0x0020,
    FWM_ACTION =             0x0040,
    FWM_TIME_OF_DAY =        0x0080,
    FWM_DAY_OF_WEEK =        0x0100,
    FWM_DAY =                0x0200,
    FWM_MONTH =              0x0400,
    FWM_PUBLIC_HOST_MASK =   0x0800,
    FWM_PUBLIC_HOST_PREFIX = 0x1000,

}FW_RULE_MASKS;


#define FWM_DATE_TIME   (FWM_TIME_OF_DAY | FWM_DAY_OF_WEEK | FWM_DAY | FWM_MONTH)
#define FWM_ALL_PROTO   (FWM_DATE_TIME | FWM_PRIVATE_HOST | FWM_PUBLIC_HOST | FWM_PUBLIC_HOST_MASK | FWM_PUBLIC_HOST_PREFIX | FWM_PROTOCOL | FWM_ACTION) 
#define FWM_TCP_UDP     (FWM_ALL_PROTO | FWM_PORT)
#define FWM_ICMP        (FWM_ALL_PROTO | FWM_TYPE | FWM_CODE)

typedef enum _FW_ACTIONS
{
    FWA_BLOCK          = 0x01,
    FWA_ALLOW          = 0x02,
    FWA_ALLOW_RESPONSE = 0x03,
    
}FW_ACTIONS;

typedef enum _FW_DIRECTIONS
{
    FWD_INBOUND =   0x08,
    FWD_OUTBOUND =  0x10,
    
}FW_DIRECTIONS;

typedef enum _FW_RULE_FLAGS
{
    FWF_BLOCK =     FWA_BLOCK,
    FWF_ALLOW =     FWA_ALLOW,
    FWF_LOG =       0x04,
    FWF_INBOUND =   FWD_INBOUND,
    FWF_OUTBOUND =  FWD_OUTBOUND,
    FWF_DISABLED =  0x20

}FW_RULE_FLAGS;


typedef struct _FW_IP_ADDRESS
{
    USHORT       Family;
    union
    {
        IN6_ADDR AddressIPv6;
        IN_ADDR  AddressIPv4;
    };
    
}FW_IP_ADDRESS, *PFW_IP_ADDRESS;


typedef enum _FW_DAYS
{
    FWD_SUNDAY =    0x01,
    FWD_MONDAY =    0x02,
    FWD_TUESDAY =   0x04,
    FWD_WEDNESDAY = 0x08,
    FWD_THURSDAY =  0x10,
    FWD_FRIDAY =    0x20,
    FWD_SATURDAY =  0x40,
    FWD_WEEKDAY =   0x3E,
    FWD_WEEKEND =   0x41,
}FW_DAYS;


typedef struct _FIREWALL_RULE
{
    DWORD               dwSize;
    HRULE               hRule;
    DWORD               dwFlags;
    DWORD               dwMask;
    LPWSTR              wszDescription;
    FW_IP_ADDRESS       PrivateHost;
    FW_IP_ADDRESS       PublicHost;
    union
    {
        UINT            PublicHostPrefixLength;
        IN_ADDR         PublicHostMask;
    };
    INT                 Protocol;
    FW_ACTIONS          Action;
    WORD                wHourStart, wHourEnd;
    WORD                wDayOfWeek;
    WORD                wDay;
    WORD                wMonth;
    
    union
    {
        // used for TCP and UDP protocols
        struct
        {
            USHORT      PortMin;
            USHORT      PortMax;
        };
        
        // used for ICMP protocol
        struct
        {
            UCHAR       Type;
            UCHAR       Code;
        };
    };
    
}FW_RULE, *PFW_RULE;


typedef struct _FIREWALL_LOG_ENTRY
{
    FW_IP_ADDRESS       PrivateHost;
    FW_IP_ADDRESS       PublicHost;
    INT                 Protocol;
    FW_ACTIONS          Action;
    FW_DIRECTIONS       Direction;
    
    union
    {
        struct 
        {
            ushort tcp_src;     // Source port.
            ushort tcp_dest;    // Destination port.
            int    tcp_seq;     // Sequence number.
            int    tcp_ack;     // Ack number.
            ushort tcp_flags;   // Flags and data offset.
            ushort tcp_window;  // Window offered.
            ushort tcp_xsum;    // Checksum.
            ushort tcp_urgent;  // Urgent pointer.
        }TCPHeader;
        
        struct
        {
            ushort Source;      // Source port.
            ushort Dest;        // Destination port.
            ushort Length;      // Length.
            ushort Checksum;    // Checksum.
        }UDPHeader;
        
        struct {
            uchar ich_type;     // Type of ICMP packet.
            uchar ich_code;     // Subcode of type.
            ushort ich_xsum;    // Checksum of packet.
            ulong ich_param;    // Type-specific parameter field.
        }ICMPHeader;
        
        struct
        {
            uchar Type;         // Type of message (high bit zero for error messages).
            uchar Code;         // Type-specific differentiater.
            ushort Checksum;    // Calculated over ICMPv6 message and IPv6 psuedo-header.
        }ICMPv6Header;
    };
    
}FW_LOG_ENTRY, *PFW_LOG_ENTRY;

//
// Firewall APIs
//
BOOL  IsFirewallEnabled(USHORT Family);
DWORD FirewallEnable(USHORT Family, BOOL bEnable, BOOL bPersist);
DWORD FirewallCreateRule(FW_RULE* pRule, BOOL bPersist);
DWORD FirewallDeleteRule(HRULE hRule, BOOL bPersist);
DWORD FirewallEnableRule(HRULE hRule, BOOL bEnable, BOOL bPersist);
DWORD FirewallGetRules(PFW_RULE pRules, DWORD *pcbSize, DWORD* pdwCount);
VOID  FirewallRefresh();
DWORD FirewallSetInterface(LPCWSTR pszInterfaceName, BOOL bFirewalled, BOOL bPersist);
BOOL  IsInterfaceFirewalled(LPCWSTR pszInterfaceName);

//
// Logging callbacks called by firewall
// 
void FirewallLogInit();
void FirewallLogPacket(const FW_LOG_ENTRY* pLogEntry);
void FirewallLogUninit();

#ifdef __cplusplus
}
#endif

#endif // _FW6_INCLUDED_
