#include "StdAfx.h"
#include "ci/libBase/ciArgParser.h"

ciArgParser* ciArgParser::_instance = NULL;
ciArgMutex	 ciArgParser::_lock;


void
ciArgMutex::lock( long millis ) {
#ifdef _COP_MSC_
    DWORD waitResult = ::WaitForSingleObject(_handle, millis);
#else // UNIX PTHREAD
    pthread_mutex_lock(&_handle);
#endif
}

void
ciArgMutex::unlock() {
#ifdef _COP_MSC_
    ::ReleaseMutex(_handle);
#else // UNIX PTHREAD
    pthread_mutex_unlock(&_handle);
#endif
}

ciArgParser*
ciArgParser::getInstance()
{
    if(!_instance) {
        _lock.lock();
        if(!_instance ) {
            _instance = new ciArgParser();
            _lock.unlock();
            return _instance;
        }
    	_lock.unlock();
    }
    return _instance;
}


ciBoolean
ciArgParser::initialize(ciInt pArgc, char** pArgv)
{
    if(!_instance) {
        _lock.lock();
        if(!_instance ) {
            _instance = new ciArgParser();
        	_lock.unlock();
			return _instance->_init(pArgc,pArgv);
        }
		_lock.unlock();
    }
	return ciTrue;
}

ciBoolean
ciArgParser::_init(ciInt pArgc, char** pArgv)
{
	_argsMap.clear();
	_argsList.clear();

	ciString aTempStr;
	ciString aOption;
	ciStringList* aValues = NULL;

	for(ciInt i=1; i<pArgc; i++) {
		if(pArgv[i]) {
			aTempStr = pArgv[i];

			_argsList.push_back(aTempStr);

			if(aTempStr.find_first_of("+-") == 0) {
				if(i>1) {
					if(!aOption.empty()) {
						_argsMap.insert(ciArgsMap::value_type(aOption, aValues));
					}
					aValues = NULL;
				}
				aOption = aTempStr;
				aValues = new ciStringList();
				continue;
			}

			if(aValues != NULL) {
				aValues->push_back(aTempStr);
			} 

		}	
	}

	if(!aOption.empty()) {
		_argsMap.insert(ciArgsMap::value_type(aOption, aValues));
	}
			
	_argc = pArgc;
	_argv = pArgv;

	// blocked by jhchoi 2004.12.02
	//printIt();

	_processName = pArgv[0];

	if(!getArgValue("+id", _serverId)) {
		cerr << "WARNING : +id is not set !!!" << endl;
	}
	if(!getArgValue("+kind", _serverKind)){
		cerr << "WARNING : +kind is not set !!!" << endl;
	}
	if(!getArgValue("+domain", _serverDomain)){
		cerr << "WARNING : +domain is not set !!!" << endl;
	}
	_serverName = _serverKind + "=" + _serverId;
	return ciTrue;
}

void
ciArgParser::terminate()
{
    if(_instance) {
		_lock.lock();
		if(_instance) {
			delete _instance;
			_instance = 0;
		}	
		_lock.unlock();
	}
}

void
ciArgParser::_clear()
{
    ciArgsMap::iterator itr;
	for(itr=_argsMap.begin();itr!=_argsMap.end();itr++) {
		ciStringList* aList = (*itr).second;
		aList->clear();
		delete aList;
	}
	_argsMap.clear();
    _argsList.clear();
}


ciBoolean
ciArgParser::isSet(const char* pOption)
{
	return _getArgValues(pOption, NULL); 
}

ciBoolean
ciArgParser::getArgValue(const char* pOption, ciString& pValue)
{
	ciStringList aRetValues;

	ciBoolean aRet = _getArgValues(pOption, &aRetValues); 
	if(aRet == ciFalse) {
		return ciFalse;
	}

	if(aRetValues.empty()) {
		pValue = "";
	} else {
		ciStringList::iterator itr = aRetValues.begin();
		pValue = *itr;
	}
	return ciTrue;
}

ciBoolean
ciArgParser::getArgValues(const char* pOption, ciStringList& pValues)
{
	return _getArgValues(pOption, &pValues); 
}

void
ciArgParser::getAllArgValues(ciStringList& pValues)
{
	pValues = _argsList;
}

ciBoolean
ciArgParser::removeArg(const char* pOption)
{
    ciString aOption(pOption);

    ciInt aStart = 0;
    while(1){
        for(ciInt i = 1;i<(_argc);i++) {
            if(aStart == 0 && (aOption == (_argv)[i])) {
                (_argc)--;
                aStart = i;
            }
            if(aStart){
                (_argv)[i] = (_argv)[i+1];
            }
        }
        if(aStart){
            for(ciInt k=aStart; k<(_argc); k++){
                if((_argv)[k][0] != '-' && (_argv)[k][0] != '+'){
                    (_argc)--;
                    for(ciInt j=k;j<(_argc);j++){
                        (_argv)[j] = (_argv)[j+1];
                    }
                    k--;
                }else{
                    break;
                }
            }
        }
        if(aStart == 0){
            break;
        }else{
            aStart = 0;
        }
    }
	return ciTrue;
}

ciBoolean
ciArgParser::_getArgValues(const char* pOption, ciStringList* pValues)
{
	ciString aOption(pOption);
	ciArgsMap::iterator itr = _argsMap.find(aOption);
	if(itr == _argsMap.end()) {
		return ciFalse;
	}
	if(pValues != NULL) {
		ciStringList* aValues = itr->second;
		*pValues = *aValues;
	}
	
	return ciTrue;
}

void
ciArgParser::printIt()
{
	ciString aOption;
	ciStringList* aValues;

	ciArgsMap::iterator itr;

	printf("ArgParser's Information -------------------------------------\n");
	printf("Arguments Map -----------------------------------------------\n");
	int i=0;
	ciDoList(_argsMap, itr) {
		aOption = itr->first;
		aValues = itr->second;

		printf("%d-[%s]: ", i, aOption.c_str());

		if(aValues == NULL) {
			printf("Value is Not Existed\n");
		} else {
			ciStringList::iterator itr1;
			ciDoListPtr(aValues, itr1) {
				if(itr1 != aValues->begin()) {
					printf(",");
				}
				printf("%s", itr1->c_str());
			}
			printf("\n");
		}
		i++;
	}
	printf("Arguments List -----------------------------------------------\n");

	ciStringList::iterator itr2;
	i=0;
	
	ciDoList(_argsList, itr2) {
		printf("%d: %s\n", i, itr2->c_str());
		i++;
	}

	printf("--------------------------------------------------------------\n");
}


