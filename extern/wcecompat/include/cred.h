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
#ifndef _CRED_H

#define _CRED_H

#ifdef __cplusplus
extern "C" {
#endif

// Pre defined Credential Types
// 32 bit unsigned number
// Reserved range 0x00010000 - 0xffffffff

// Primitive
// LSB bit should be zero [even numbers]

#define CRED_TYPE_NTLM 						0x00010002	// 65538
#define CRED_TYPE_KERBEROS					0x00010004	// 65540
#define CRED_TYPE_PLAINTEXT_PASSWORD		0x00010006	// 65542
#define CRED_TYPE_CERTIFICATE	 			0x00010008	// 65544
#define CRED_TYPE_GENERIC		 			0x0001000a	// 65546

// Virtual
// LSB bit should be one [odd numbers]

#define CRED_TYPE_DOMAIN_PASSWORD		0x00010001	// 65537

// Credential Flags

// Taken values [0x1 - 0x1000]

// Read/Write
#define CRED_FLAG_PERSIST					0x00000001
#define CRED_FLAG_DEFAULT					0x00000002
#define CRED_FLAG_SENSITIVE				0x00000008
#define CRED_FLAG_TRUSTED					0x00000010

// Read
#define CRED_FLAG_IMPLICIT_DEFAULT		0x00000020

// CredWrite Flags
#define CRED_FLAG_FAIL_IF_EXISTING		0x00000400

// CredRead Flags
#define CRED_FLAG_NO_DEFAULT				0x00000800
#define CRED_FLAG_NO_IMPLICIT_DEFAULT	0x00001000

// CredUpdate Flags
#define CRED_FLAG_UPDATE_USER				0x00000080
#define CRED_FLAG_UPDATE_BLOB 			0x00000100
#define CRED_FLAG_UPDATE_FLAGS			0x00000200
#define CRED_FLAG_UPDATE_ALL	(CRED_FLAG_UPDATE_USER|CRED_FLAG_UPDATE_BLOB|CRED_FLAG_UPDATE_FLAGS)

// CredWrite and CredUpdate
#define CRED_FLAG_NO_BLOB_HANDLING		0x00000040

// Magic constants

#define CRED_VER_1 							1

// Length includes terminating null
#define CRED_MAX_TARGET_LEN			255

// UNLEN=256 + 1 (terminating null)
#define CRED_MAX_USERNAME_LEN 			257

// DNS_MAX_NAME_LENGTH=255 + 1(terminating null)
#define CRED_MAX_DOMAINNAME_LEN 			256

// domain/user (includes terminating null)
#define CRED_MAX_USER_LEN		CRED_MAX_USERNAME_LEN + \
								CRED_MAX_DOMAINNAME_LEN

#define CRED_MAX_DOMAINUSERNAME_LEN 	CRED_MAX_USER_LEN

// Assuming max password length of PWLEN (lmcons.h) = 256
// Our length includes NULL character hence adding space for 1 more character
// Also our lengths are byte size, hence doubling by 2 for WCHAR
// 514 = (256+1)*2
#define CRED_MAX_BLOB_SIZE					514 

// Error codes
// 16000-17999

#define ERROR_CREDMAN_ALL_FAILED 			16001L
#define ERROR_CREDMAN_SOME_FAILED		16002L

// Structures

typedef struct _CRED {
	DWORD 		dwVersion;
	DWORD 		dwType;
	PWCHAR 		wszUser;
	DWORD 		dwUserLen;
	PWCHAR 		wszTarget;
	DWORD 		dwTargetLen;
	PBYTE 		pBlob;
	DWORD 		dwBlobSize;
	DWORD 		dwFlags;
} CRED, *PCRED, **PPCRED;

#define CRED_MAX_CRED_SIZE (sizeof(CRED) + \
								(CRED_MAX_USER_LEN+CRED_MAX_TARGET_LEN)*sizeof(WCHAR) + \
								CRED_MAX_BLOB_SIZE)
								
// Functions

DWORD
CredWrite(
	IN	PCRED		pCred,
	IN	DWORD		dwFlags
);

DWORD
CredRead(
	IN 	PWCHAR		wszTarget,
	IN 	DWORD 		dwTargetLen,
	IN 	DWORD		dwType,
	IN 	DWORD		dwFlags,
	OUT PPCRED		ppCred
);

DWORD
CredDelete(
	IN	PWCHAR		wszTarget,
	IN 	DWORD 		dwTargetLen,	
	IN	DWORD		dwType,
	IN 	DWORD		dwFlags
);

DWORD
CredUpdate(
	IN 	PWCHAR		wszTarget,
	IN 	DWORD 		dwTargetLen,	
	IN 	DWORD 		dwType,
	IN	PCRED		pCred,
	IN	DWORD		dwFlags
);	

DWORD
CredFree(
	IN	PBYTE 		pvBuffer
);

#ifdef __cplusplus
}
#endif

#endif

