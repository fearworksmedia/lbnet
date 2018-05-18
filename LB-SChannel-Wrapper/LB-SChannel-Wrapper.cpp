// LB-SChannel-Wrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#define SECURITY_WIN32
#include <security.h>
#include <schnlsp.h>

#define DLL_API EXTERN_C __declspec(dllexport)
#pragma comment(lib, "Secur32.lib")

#define cbMaxMessage 12000

DLL_API SECURITY_STATUS AcquireSChannelUnvalidatedCredHandle(PCredHandle * ppCredHandle)
{
	*ppCredHandle = new CredHandle();
	SCHANNEL_CRED sc = SCHANNEL_CRED();
	sc.dwVersion = SCHANNEL_CRED_VERSION;
	sc.dwFlags = SCH_CRED_MANUAL_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_NO_SERVERNAME_CHECK;

	return AcquireCredentialsHandle(NULL, const_cast<LPSTR>(UNISP_NAME), SECPKG_CRED_OUTBOUND, NULL,
		&sc, NULL, NULL, *ppCredHandle, NULL);
} 

DLL_API int GetMaxMessageSize()
{
	return cbMaxMessage;
}

DLL_API SECURITY_STATUS InitSecurityContext(PCredHandle phCredential, PCtxtHandle phContext, SEC_CHAR *pszTargetName,
	PVOID pvInputBuffer, ULONG cbInputBuffer, PCtxtHandle * pphNewContext, PVOID pvOutputBuffer, ULONG cbOutputBuffer)
{
	SecBufferDesc OutputBufDesc;
	SecBuffer OutputBuf;

	OutputBufDesc.ulVersion = SECBUFFER_VERSION;
	OutputBufDesc.cBuffers = 1;
	OutputBufDesc.pBuffers = &OutputBuf;

	OutputBuf.BufferType = SECBUFFER_TOKEN;
	OutputBuf.cbBuffer = cbOutputBuffer;
	OutputBuf.pvBuffer = pvOutputBuffer;

	ULONG context;

	if (phContext == NULL)  //First call to InitializeSecurityContext()
	{
		*pphNewContext = new CtxtHandle();
		return InitializeSecurityContext(phCredential, phContext, pszTargetName, ISC_REQ_CONFIDENTIALITY, 0,
			0, NULL, 0, *pphNewContext, &OutputBufDesc, &context, NULL);
	}
	else
	{
		SecBufferDesc InputBufDesc;
		SecBuffer InputBuf[2];

		InputBuf[0].BufferType = SECBUFFER_TOKEN;
		InputBuf[0].cbBuffer = cbInputBuffer;
		InputBuf[0].pvBuffer = pvInputBuffer;

		InputBuf[1] = SecBuffer();

		InputBufDesc.cBuffers = 2;
		InputBufDesc.pBuffers = InputBuf;
		InputBufDesc.ulVersion = SECBUFFER_VERSION;

		return InitializeSecurityContext(phCredential, phContext, pszTargetName, ISC_REQ_CONFIDENTIALITY, 0,
			0, &InputBufDesc, 0, *pphNewContext, &OutputBufDesc, &context, NULL);
	}
}

DLL_API SECURITY_STATUS FreeCredHandle(PCredHandle pCredHandle)
{
	return FreeCredentialsHandle(pCredHandle);
	delete pCredHandle;
}

DLL_API SECURITY_STATUS DeleteSecContext(PCtxtHandle phContext)
{
	return DeleteSecurityContext(phContext);
}