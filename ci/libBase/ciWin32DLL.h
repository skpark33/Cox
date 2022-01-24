#ifndef __COMMON_ciWin32DLL_h__
#define __COMMON_ciWin32DLL_h__

#ifdef WIN32

	#include <winsock2.h>
	#include <windows.h>
	#include <atlbase.h>

	#pragma warning (disable :4786)
	#pragma warning (disable :4284)
	#pragma warning (disable :4250)
	#pragma warning (disable :4503)
	// added
	#pragma warning (disable :4251)
	#pragma warning (disable :4290) 
	//#pragma warning (disable :4103)
	//#pragma warning (disable :4355)
	//#pragma warning (disable :4290)
	//#pragma warning (disable :4231)

	#ifdef COP_CI_EXPORTS
		#define COP_CI_API __declspec(dllexport)
	#else
		#define COP_CI_API 
	#endif

	#ifdef COP_HI_EXPORTS
		#define COP_HI_API __declspec(dllexport)
	#else
		#define COP_HI_API __declspec(dllimport)
	#endif

	#ifdef COP_CAFIDL_EXPORTS
		#define COP_CAFIDL_API __declspec(dllexport)
	#else
		#define COP_CAFIDL_API __declspec(dllimport)
	#endif

	#ifdef COP_CAFIDL_TMF814V1_EXPORTS
		#define COP_CAFIDL_TMF814V1_API __declspec(dllexport)
	#else
		#define COP_CAFIDL_TMF814V1_API __declspec(dllimport)
	#endif

	#ifdef COP_CCI_EXPORTS
		#define COP_CCI_API __declspec(dllexport)
	#else
		#define COP_CCI_API __declspec(dllimport)
	#endif

	#ifdef COP_ACI_EXPORTS
		#define COP_ACI_API __declspec(dllexport)
	#else
		#define COP_ACI_API __declspec(dllimport)
	#endif

	#ifdef COP_MIR_EXPORTS
		#define COP_MIR_API __declspec(dllexport)
	#else
		#define COP_MIR_API __declspec(dllimport)
	#endif

	#ifdef COP_CASIDL_EXPORTS
		#define COP_CASIDL_API __declspec(dllexport)
	#else
		#define COP_CASIDL_API __declspec(dllimport)
	#endif

#else
	#define COP_CI_API
	#define COP_HI_API
	#define COP_CI_HTTP_API
	#define COP_CAFIDL_API
	#define COP_CAFIDL_TMF814V1_API
	#define COP_CCI_API
	#define COP_ACI_API
	#define COP_MIR_API
	#define COP_CASIDL_API
#endif

#endif//__COMMON_ciWin32DLL_h__

