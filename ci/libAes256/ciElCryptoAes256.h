/** \class Aes256Crypto
 *  Copyright ⓒ 2016, SQIsoft ELiga. All rights reserved.
 *
 *  \brief 블록암호모드 CBC 모드
 *  
 *  \author SQIsoft, detect8
 *  \version 1.0
 *  \purpose
 *  \date 2016/01/11 11:01:00
 */

#ifndef _ciElCryptoAes256_h_
#define _ciElCryptoAes256_h_

#include "ci/libBase/ciListType.h"
#include "ci/libAes256/ciAES256.h"


class  ciElCryptoAes256
{

public:
	//static ciElCryptoAes256* getInstance();

    ciElCryptoAes256(void);
    virtual ~ciElCryptoAes256(void);

    std::string  Encrypt(std::string plain_text, bool useGUID);
    std::string  Decrypt(std::string cipher_text);

	bool    Init(std::string key32="JYT20140315197402171994082501234", std::string iv16="JYT2014031519740", ciAES256::Chaining_Mode mode=ciAES256::CBC);

	bool CreateGUID(ciString& outval);

protected:

	std::string  key32_;
    std::string  iv16_;
    ciAES256::Chaining_Mode block_cipher_mode_;

    ciAES256* aes256_;


	//static ciElCryptoAes256*	_instance;
	//static ciMutex				_instLock;

};

class ciAes256Util
{
public:
    static std::string  Encrypt(std::string plain_text, bool useGUID);
    static std::string  Decrypt(std::string cipher_text);

	static bool ciAes256Util::EncryptFile(LPCTSTR inFilePath, LPCTSTR outFilePath, bool removeInFile);
	static bool ciAes256Util::DecryptFile(LPCTSTR inFilePath, LPCTSTR outFilePath, bool removeInFile);
};

#endif // _ciElCryptoAes256_h_