#include "redis_bitmap_util.h"

	
CRedisBitMapUtil::CRedisBitMapUtil(CRedisUtil* conn):_conn(conn)
{
}

CRedisBitMapUtil::~CRedisBitMapUtil()
{
}

	
bool  CRedisBitMapUtil::BitSet(const char* key, size_t offset)
{
	CRedisResult ret;
	_conn->Command(ret, "SETBIT %s %d 1", key, offset);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return false;
	return true;
}
//
bool  CRedisBitMapUtil::BitReset(const char* key, size_t offset)
{
	CRedisResult ret;
	_conn->Command(ret, "SETBIT %s %d 0", key, offset);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return false;
	return true;
}
//
bool  CRedisBitMapUtil::BitGet(const char* key, size_t offset)
{
	CRedisResult ret;
	_conn->Command(ret, "GETBIT %s %d", key, offset);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return false;
	
	return ret.integer();
}
	
long long CRedisBitMapUtil::BitCount(const char* key)
{
	CRedisResult ret;
	_conn->Command(ret, "BITCOUNT %s", key);
	if(!ret.check() || ret.type() != REDIS_REPLY_INTEGER)
		return 0;
	return ret.integer();
}
