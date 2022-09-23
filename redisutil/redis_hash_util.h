/**
 * @file	redis_hash_util.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-22
 */
#ifndef _REDIS_HASH_UTIL_H_
#define _REDIS_HASH_UTIL_H_ 

#include "redisutil.h"

class CRedisHashUtil
{
public:
	explicit CRedisHashUtil(CRedisUtil* pRedis);
	~CRedisHashUtil();
public:
	bool HashDel(const char* pKey, const char* pFiled);
};

#endif 

