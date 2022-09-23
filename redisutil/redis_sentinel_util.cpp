#include "redis_sentinel_util.h"
#include "hiredis/read.h"
#include "redisutil.h"
#include <string.h>

CRedisSentinelUtil::CRedisSentinelUtil(CRedisUtil* pRedis):_redis(pRedis)
{

}

CRedisSentinelUtil::~CRedisSentinelUtil()
{
	
}
	
std::pair<std::string, int>	CRedisSentinelUtil::get_master_addr(const char* master_name)
{
	using namespace std;
	string ip;
	int port;
	if(NULL == _redis)
		return std::make_pair(ip, port);

	CRedisResult ret;
	_redis->Command(ret, "SENTINEL get-master-addr-by-name %s", master_name);
	if(!ret.check())
		return std::make_pair(ip, port);
	//	
	if(ret.elements() < 2)
		return std::make_pair(ip, port);

	auto ip_ret = ret.element(0);
	if(ip_ret.type() == REDIS_REPLY_STRING)
		ip = ip_ret.str();
	auto port_ret = ret.element(1);
	if(port_ret.type() == REDIS_REPLY_STRING)
		port = atoi(port_ret.str());
		
	return std::make_pair(ip, port);
}
	
const std::vector<std::pair<std::string, int> >&  CRedisSentinelUtil::get_slaves(const char* name)
{
	using namespace std;
	_slaves.clear();
	if(NULL == _redis || NULL == name)
		return _slaves;
	//	
	CRedisResult ret;
	_redis->Command(ret, "SENTINEL slaves %s", name);
	if(!ret.check())
		return _slaves;

	//
	string ip;
	int  port;
	int nCnt = ret.elements();
	for(auto i = 0; i < nCnt; i++)
	{
		auto addr = ret.element(i);
		size_t type = addr.type();
		if(addr.type() != REDIS_REPLY_ARRAY)
			continue;
		
		int addr_cnt = addr.elements();
		for(auto i = 0; i < addr_cnt; i = i + 2)
		{
			auto tmp = addr.element(i);
			if(strcmp(tmp.str(), "ip") == 0)
			{
				auto slave_ip = addr.element(1 + i);
				if(slave_ip.type() == REDIS_REPLY_STRING)
				ip = slave_ip.str();
			}
			else if(strcmp(tmp.str(), "port") == 0)
			{
				auto slave_port = addr.element(i + 1);
				if(slave_port.type() == REDIS_REPLY_STRING)
					port = atoi(slave_port.str());
			}
		}
		//	
		_slaves.push_back(std::make_pair(ip,port));
	}
	//	
	return _slaves;	
}
