/**
 * @file	redis_sentinel_util.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-23
 */
#ifndef _REDIS_SENTINEL_UTIL_H_
#define _REDIS_SENTINEL_UTIL_H_ 

#include "redisutil.h"
#include <vector>

class CRedisSentinelUtil
{
public:
	explicit CRedisSentinelUtil(CRedisUtil* pRedis);
	~CRedisSentinelUtil();
public:
	std::pair<std::string, int>	get_master_addr(const char* master_name);			
	const std::vector<std::pair<std::string, int> >&  get_slaves(const char* name);
private:
	CRedisUtil*		_redis;
private:
	std::vector<std::pair<std::string, int> >		_slaves;		//从机信息
	std::pair<std::string, int>				_master;
};

#endif 

