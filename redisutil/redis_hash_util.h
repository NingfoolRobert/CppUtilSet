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
	bool HashDel(const char* pKey, const char* filed, size_t filed_len);
	bool HashSet(const char* pKey, const char* filed, size_t filed_len,  const char* data,  size_t  data_len);
	bool HashSetNx(const char* pKey, const char* filed, size_t filed_len, const char* data, size_t data_len);
	const std::string&  HashGet(const char* pKey, const char* filed, size_t filed_len);
	bool HashScan(CRedisResult& ret, const char* key, const char* filed, size_t filed_len);
private:
	bool HashExists(const char* key, const char* filed, size_t filed_len);
private:
	CRedisUtil*			_redis;
	std::string			_buf;
};

#endif 

