#include "redis_hash_util.h"
#include "hiredis/read.h"
#include "redisutil.h"
#include <cstdio>
#include <memory>

	
	
CRedisHashUtil::CRedisHashUtil(CRedisUtil* pRedis):_redis(pRedis)
{
}

CRedisHashUtil::~CRedisHashUtil()
{
}

bool CRedisHashUtil::HashDel(const char* pKey, const char* filed, size_t filed_len)
{
	CRedisResult ret;
	_redis->Command(ret, "HDEL %s %b", pKey, filed, filed_len);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return false;
	return true;
}
//
bool CRedisHashUtil::HashSet(const char* pKey, const char* filed, size_t filed_len,  const char* data,  size_t  data_len)
{
	CRedisResult ret;
	_redis->Command(ret, "HSET %s %b %b", pKey, filed, filed_len, data, data_len);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return false;
	if(ret.integer() == 1 || ret.integer() == 0)
		return true;
	return false;
}
//
bool CRedisHashUtil::HashSetNx(const char* pKey, const char* filed, size_t filed_len, const char* data, size_t data_len)
{
	CRedisResult ret;
	_redis->Command(ret, "HSETNX %s %b %b", pKey, filed, filed_len, data, data_len);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER || ret.integer() != 1)
		return false;
	return true;
}
//
bool CRedisHashUtil::HashScan(CRedisResult& ret, const char* key, size_t  cursor)
{
	if(nullptr == _redis)
		return false;
	_redis->Command(ret, "HSCAN %s  %d", key, cursor);	
	if(!ret.check() || 0 == ret.elements())
		return false;
	return true;
}

bool CRedisHashUtil::HashScanFuzzy(CRedisResult& ret, const char* key, size_t cursor, const char* match_str)
{
	_redis->Command(ret, "HSCAN %s %d match %s", key, cursor, match_str);
	if(!ret.check())
		return false;
	return true;
}
	
size_t CRedisHashUtil::HashSize(const char* key)
{
	CRedisResult ret;
	_redis->Command(ret, "HLEN %s", key);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return 0;
	return ret.integer();
}
//	
bool CRedisHashUtil::HashExists(const char* key, const char* filed, size_t filed_len)
{
	CRedisResult ret;
	_redis->Command(ret, "HEXISTS %s %b", key, filed, filed_len);	
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER) 
		return false;
	if(ret.integer() == 1)
		return true;
	return false;
}
//	
const std::string&  CRedisHashUtil::HashGet(const char* pKey, const char* filed, size_t filed_len)
{
	_buf.clear();
	CRedisResult ret;
	_redis->Command(ret, "HGET %s %b", pKey, filed, filed_len);
	if(!ret.check() )// || ret.type() == REDIS_REPLY_NIL || ret.type() != REDIS_REPLY_STRING)
	{
		printf("HGET fail");
		return _buf;
	}
	//
	if(ret.str() != NULL && ret.len())
		_buf.assign(ret.str(), ret.len());
	//
	return _buf;
}

