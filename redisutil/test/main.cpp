#include "../redisutil.h"
#include "../redis_sentinel_util.h"

#include <iostream>
#include <functional> 
#include <algorithm>

using namespace std;

int main()
{
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
