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
/*--
Module Name: AUTHHLP
Abstract: NTLM Authentication for telnetd.
--*/

/****************************************************************************
 USAGE

 Call AuthHelpValidateUser(A/W), depending on whether the user name and 
 password are in ANSI or UNICODE strings.  The ACL is always in UNICODE
 in CE because it is typically read from the registry.  The function will
 return TRUE if access is allowed and FALSE if it is not allowed.

 Access may be denied if the user is not a valid user on the domain, or 
 if they are not in the Access Control List (ACL).  
 The ACL is a string, each element separeted by a semicolon, that specifies
 the name of a group or user to either allow or deny service to.  To specify
 a group, put a "@" immediatly before it.  To specify either a user or a group
 is to be denied access, put a "-" before it.  

 
 For instance, ACL = "good_user1; @good_group1; -bad_user1; -@bad_group1; good_user2"
 will allow users named good_user1, good_user2, and anyone who is a member of
 the group good_group1 access (assuming they've succesfully been authenticated
 by NTLM).  It will deny access to anyone named bad_user1 and in bad_group1.

 The checks are made from left to right, and the check will stop being made
 as soon as a match (either positive or negative) is made.

 In the above example, if good_user1 is also a member of bad_group1, they will
 receive access because good_user1 came before bad_group1.  However, if
 good_user2 is a member of bad_group1, they will be denied access because 
 bad_group1 came before good_user2.
 
 A "*" in the ACL list means that all users are granted access, provided they 
 have not been disqualified by any of the arguments to the left of the arg
 list.

 For instance, if ACL = "-bad_user1; *" then all users will be granted
 access, except for bad_user1.
 

 IMPORTANT SECURITY CONSIDERATIONS

 Note that there is some danger in using this API set over a public network
 such as the Internet if the passwords are not encrypted by the calling 
 application.  It is possible for a malicious user to intercept packets sent 
 to  your network application and to learn the password, either on the 
 Domain or on the CE device, depending on which options are used.  Use
 this with care.


 See the telnetd sample for an example of how to use this API.

****************************************************************************/


#ifndef _AUTH_H_
#define _AUTH_H_

#ifndef UNDER_CE
#define SECURITY_WIN32
#endif

#include <windows.h>
#include <sspi.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

BOOL AuthHelpValidateUserA(PSTR pszRemoteUser, PSTR pszPassword, TCHAR *pszACL, DWORD dwFlags);
BOOL AuthHelpValidateUserW(PWSTR wszRemoteUser, PWSTR wszPassword, TCHAR *pszACL, DWORD dwFlags);
BOOL AuthHelpValidateUser(TCHAR *pszRemoteUser, TCHAR *pszPassword, TCHAR * pszACL, DWORD dwFlags);
BOOL IsAccessAllowed(TCHAR *pszRemoteUser, TCHAR *pszRemoteUserGroups, TCHAR *pszACL, BOOL fPeek);

#if defined (UNICODE)
#define IsAccessAllowedEx  IsAccessAllowedExW
#else
#define IsAccessAllowedEx  IsAccessAllowedExA
#endif

BOOL IsAccessAllowedExA(const CHAR *pszRemoteUser, const CHAR *pszRemoteUserGroups, const CHAR *pszACL, DWORD dwFlags);
BOOL IsAccessAllowedExW(const WCHAR *pszRemoteUser, const WCHAR *pszRemoteUserGroups, const WCHAR *pszACL, DWORD dwFlags);

BOOL AuthHelpUnload();
BOOL AuthHelpInitialize(); //NOTE: this will always return TRUE, use AuthIsNTLM() 
			   // to discover if NTLM is being used 

BOOL AuthIsNTLM();

HKEY AuthHelpOpenUserKeyA(PSTR pszApp, PSTR pszRemoteUser);
HKEY AuthHelpOpenUserKeyW(PWSTR pszApp, PWSTR pszRemoteUser);

#if defined (UNICODE)
#define AuthHelpOpenUserKey  AuthHelpOpenUserKeyW
#else
#define AuthHelpOpenUserKey  AuthHelpOpenUserKeyA
#endif


// Set to skip NTLM checking during AuthHelpValidateUser().
#define AUTH_HELP_FLAGS_NO_NTLM             0x01

// In call to IsAccessAllowedEx(), setting this flag will cause function to
// succeed if a group has been set "@", assuming the user name passed in was not
// explicitly denied.  This is used to make sure that a user name was not 
// explicitly denied before beginning a potentially long running NTLM query.
#define AUTH_HELP_FLAGS_PRESCAN             0x02

// Set to ignore group "@" sign, treats it like another character.  This is 
// useful when using authhlp IsAccessAllowedEx() to see if a string is in 
// an allowed-list where the string may legally start with a @.
#define AUTH_HELP_FLAGS_IGNORE_GROUP        0x04


// If a user is not explicitly denied (i.e there is no '-' preceeding their name)
// then IsAccessAllowedEx will return TRUE.  This is the functional equivalent 
// of putting a '*' on the end of a list.
#define AUTH_HELP_FLAGS_CHECK_NOT_DENIED    0x08

// '*' is wildcard allowing everyone access
#define AUTH_HELP_C_ALLOW_ALL        '*'
#define AUTH_HELP_WC_ALLOW_ALL      L'*'

// '-' denies access to entry to the immediate right of it
#define AUTH_HELP_C_DENY             '-'
#define AUTH_HELP_WC_DENY           L'-'

// ';' is delimiter
#define AUTH_HELP_C_DELIMITER        ';'
#define AUTH_HELP_WC_DELIMITER      L';'

// '@' is group character
#define AUTH_HELP_C_GROUP            '@'
#define AUTH_HELP_WC_GROUP          L'@'


#ifdef __cplusplus
}
#endif

#endif // _AUTH_H_
