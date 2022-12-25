/**
 * @file	redis_key_util.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-22
 */
#ifndef  _REDIS_KEY_UTIL_H_
#define  _REDIS_KEY_UTIL_H_ 

#include "redisutil.h"
#include <vector>


class  CRedisKeyUtil
{
public:	
	explicit CRedisKeyUtil(CRedisUtil* pRedis);
	~CRedisKeyUtil();
public:
	bool			KeyDel(const char* pKey);
	bool 			KeyExists(const char* pKey);
	bool 			KeyExpire(const char* pKey,  int timeout); // ms 
	bool 			KeyPersist(const char* pKey);
	bool 			KeyMove(const char* pKey, int db);//move key to db from current 
	bool 			KeyGet(std::vector<std::string>& vecKey, const char* pattern);
	std::string		KeyType(const char* key); //
private:
	CRedisUtil*			_redis;
};


#endif 

