#include "../redisutil.h"
#include "../redis_sentinel_util.h"
#include "../redis_key_util.h"
#include "../redis_hash_util.h"
#include "hiredis/read.h"

#include <iostream>
#include <functional> 
#include <algorithm>
#include <vector>

using namespace std;
void test_key_get()
{
	CRedisUtil ru;
	if(!ru.Connect("127.0.0.1", 6380,"", 30000))
		return ;
	std::vector<std::string> vecKey;
	CRedisKeyUtil rku(&ru);
	rku.KeyGet(vecKey, "*");
	for(auto i = 0u; i < vecKey.size(); ++i)
	{
		std::string type = rku.KeyType(vecKey[i].c_str());
		printf("%s:%s\n", vecKey[i].c_str(), type.c_str());
	}
	printf("Key: %d", vecKey.size());
}
void test_sentinel_slave()
{
	CRedisUtil ru;
	if(!ru.Connect("10.110.47.89", 26380, "", 30000))
		return ;
	CRedisSentinelUtil rsu(&ru);
   	
	auto & val = rsu.get_slaves("SnapCenter");
	for(auto i =0u; i < val.size();++i)
	{
		printf("Cnt:%d, ip:%s, port:%d\n", i, val[i].first.c_str(), val[i].second);
	}	
}
//
void test_hash_scan()
{
	CRedisUtil ru;
	if(!ru.Connect("127.0.0.1", 6380,"", 30000))
		return ;
	std::vector<std::string> vecKey;
	CRedisHashUtil rhu(&ru);
	
	auto size = rhu.HashSize("RT:0");
	for(auto i = 0; i < size; i+=10)
	{
		CRedisResult ret;
		if(!rhu.HashScan(ret, "RT:0", i))
		{
			printf("redis parse fail.\n");
			return ;
		}
		
		for(auto j = 0; j < ret.elements(); j++)
		{
			auto rep = ret.element(j);
			if(rep.type() != REDIS_REPLY_ARRAY)
				continue;
			for(auto t = 0; t < rep.elements(); t+=2)
			{
				printf("key:%s, val:%d\n", rep.element(t).str(), rep.element(t+1).len());
			}

		}
	}
}

int main()
{
	test_hash_scan();
	test_sentinel_slave();
	test_key_get();
	//
	string ip = "10.110.163.46";
	int value = 96;
	std::string data;
	data.assign((char*)&value, sizeof(value));
	printf("%s %b", ip.c_str(), data.data(), data.length());
	int port = 6379;
	CRedisUtil conn;
	if(conn.Connect(ip.c_str(), 26480))
	{
		printf("Connect Redis Success. ip:port=%s:%d\n", ip.c_str(), port);
	}	
	else 
	{
		printf("Connect Redis fail. ip:port=%s:%d\n", ip.c_str(), port);
		return -1;
	}
	//
		
	CRedisSentinelUtil sentinel(&conn);
	std::string master_ip;
	std::tie(master_ip, port) = sentinel.get_master_addr("hsmaster");
	
	printf("master, ip:port=%s:%d\n", master_ip.c_str(), port);

	const auto& vec = sentinel.get_slaves("hsmaster");
	for(auto i = 0; i < vec.size(); ++i)
	{
		printf("salves, ip:port=%s:%d\n", vec[i].first.c_str(), vec[i].second);
	}
	
	return 0;
}
