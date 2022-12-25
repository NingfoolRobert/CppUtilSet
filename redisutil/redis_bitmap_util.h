/**
 * @file	redis_bidmap_util.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-28
 */
#ifndef _REDIS_BIT_MAP_UTIL_H_
#define _REDIS_BIT_MAP_UTIL_H_ 

#include "redisutil.h"

class CRedisBitMapUtil
{
public:
	explicit CRedisBitMapUtil(CRedisUtil* conn);
	~CRedisBitMapUtil();
public:
	bool  BitSet(const char* key, size_t offset);
	bool  BitReset(const char* key, size_t offset);
	bool  BitGet(const char* key, size_t offset);
	long long BitCount(const char* key);
private:
	CRedisUtil*		_conn;
};


#endif 

