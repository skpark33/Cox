
#ifndef _ciArgParser_h_
#define _ciArgParser_h_

#include "ci/libBase/ciBaseType.h"
#include "ci/libBase/ciListType.h"
#ifndef _COP_MSC_
#include <pthread.h>
#include <semaphore.h>
#endif

#include <map>

class COP_CI_API ciArgMutex
{
public:
    ciArgMutex() {};
    ~ciArgMutex() {};

    void lock(long millis = COP_INFINITE);
    void unlock();

protected:
#ifdef _COP_MSC_
    HANDLE _handle;
#else // UNIX PTHREAD
    pthread_mutex_t _handle;
#endif
};


class COP_CI_API ciArgParser {
public:
	typedef map<ciString, ciStringList*> ciArgsMap;

	static ciBoolean 	initialize(ciInt pArgc, char** pArgv);
	static void 		terminate();

	static ciArgParser* getInstance();

	virtual ~ciArgParser(){ _clear(); }

	ciBoolean 	isSet(const char* pOption);
	ciBoolean 	getArgValue(const char* pOption, ciString& pValue);
	ciBoolean 	getArgValues(const char* pOption, ciStringList& pValues);
	void 		getAllArgValues(ciStringList& pValues);

	ciBoolean 	removeArg(const char* pArg);
	void 		printIt();

	const char* getServerId()		{ return _serverId.c_str(); }
	const char* getServerKind()		{ return _serverKind.c_str(); }
	const char* getServerDomain()	{ return _serverDomain.c_str(); }
	const char* getServerName()		{ return _serverName.c_str(); }
	const char* getProcessName()	{ return _processName.c_str(); }

	char** 		getArgv() { return _argv; }
	ciInt 		getArgc() { return _argc; }

	void		lock() {}
	void		unlock() {}
					

protected:

	ciArgParser():_serverId(""), _serverKind(""), _serverDomain(""), _serverName(""), _processName("") {}

	static ciArgParser* _instance;
	static ciArgMutex	_lock;


	ciBoolean 			_init(ciInt pArgc, char** pArgv);
	void				_clear();
	ciBoolean 			_getArgValues(const char* pOption, 
									ciStringList* pValues = NULL);

	ciArgsMap			_argsMap;
	ciStringList		_argsList;

	char** 				_argv;
	ciInt 				_argc;

	// basic arguments

	ciString 			_serverId;
	ciString			_serverKind;
	ciString    		_serverDomain;
	ciString    		_serverName;
	ciString    		_processName;
};
	
#endif //_ciArgParser_h_
