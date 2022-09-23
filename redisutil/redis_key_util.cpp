#include "redis_key_util.h"
#include "hiredis/read.h"
#include "redisutil.h"
#include <cstdio>
	
CRedisKeyUtil::CRedisKeyUtil(CRedisUtil* pRedis):_redis(pRedis)
{

}

CRedisKeyUtil::~CRedisKeyUtil()
{
	
}



bool CRedisKeyUtil::KeyDel(const char* pKey)
{
	if(_redis == NULL)
		return false;
	CRedisResult ret;
	_redis->Command(ret, "DEL %s", pKey);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER || ret.integer() != 1)
		return false;

	return true;
}

bool CRedisKeyUtil::KeyExists(const char* pKey)
{
	if(_redis == NULL)
		return false;
	CRedisResult ret;
	_redis->Command(ret, "EXISTS %s", pKey);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER || ret.integer() != 1)
		return false;
	return true;
}
 
bool CRedisKeyUtil::KeyExpire(const char* pKey,  int timeout) // ms 
{
	if(NULL == _redis || NULL == pKey)
		return false;
	
	CRedisResult ret;
	_redis->Command(ret, "PEXPIRE %s %d", pKey, timeout);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER || ret.integer() != 1)
		return false;
	return true;
}

bool CRedisKeyUtil::KeyPersist(const char* pKey)
{
	if(NULL == _redis || NULL == pKey)
		return false;
	CRedisResult ret;
	_redis->Command(ret, "PERSIST %s", pKey);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER || ret.integer() != 1)
		return false;
	return true;
}

bool CRedisKeyUtil::KeyMove(const char* pKey, int db)//move key to db from current
{
	if(NULL == _redis || NULL == pKey)
		return false;
	//
	CRedisResult ret;
	_redis->Command(ret, "MOVE %s %d", pKey, db);
	if(!ret.check()  || ret.type() != REDIS_REPLY_INTEGER || ret.integer() != 1) 
		return false;

	return true;
}
