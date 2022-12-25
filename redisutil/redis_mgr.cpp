#include "redis_mgr.h"
#include "redisutil.h"
#include "redis_sentinel_util.h"
#include <mutex>
#include <utility>
#include <string.h>
	
CRedisMgr::CRedisMgr():_max_size(20),_discon_cnt(0),_master(0)
{
}
	
CRedisMgr::~CRedisMgr()
{
	Clear();
}

	
bool CRedisMgr::Init(const char* ip, int port, const char* pwd, int size/* = 10*/)
{
	strcpy(_pwd,  pwd);
	_addrs.push_back(std::make_pair(ip, port));
	
	return true;
}

CRedisUtil*	 CRedisMgr::GetConn()
{
	CRedisUtil*  pConn = NULL;
	std::unique_lock<std::mutex> _(_lck);
	if(!_wait_conns.empty())
	{
		auto pConn = _wait_conns.front();
		_wait_conns.pop();
		return pConn;	
	}
	//
	if(_cur_size >= _max_size || _addrs.size() == 0)
		return NULL;
	//
	pConn = new CRedisUtil;
	if(pConn == NULL)
	{
		printf("memory error.");
		return NULL;
	}
	//
	if(!pConn->Connect(_addrs[0].first.c_str(), _addrs[0].second, _pwd))
	{
		delete pConn;
		return NULL;
	}
	
	return pConn;
}

void CRedisMgr::Reclaim(CRedisUtil* conn)
{
	std::unique_lock<std::mutex> _(_lck);
	if(NULL == conn)
		return ;
	//
	if(!conn->Ping())
	{	
		delete  conn;	
		return ;
	}
	//
	if(_wait_conns.size() >= _max_size)
	{
		delete conn;
		return ;
	}
	//
	_wait_conns.push(conn);
}

void CRedisMgr::Clear()
{
	std::unique_lock<std::mutex> _(_lck);
	while(!_wait_conns.empty())	
	{
		auto pConn = _wait_conns.front();
		_wait_conns.pop();
		if(NULL != pConn)
		{
			delete pConn;
		}
	}
	//
	_addrs.clear();
}

//
bool CRedisMgr::InitSentinel(const char* ip, int port, const char* pwd, const char* name, bool bmaster/* = true*/,  int size /*= 10*/)
{
	_master = bmaster ;
	_port = port;
	strcpy(_ip, ip);
	strcpy(_name, name);
	strcpy(_pwd, pwd);	
	//
	CRedisUtil conn;
	CRedisResult ret;
	if(!conn.Connect(_ip, _port, _pwd))
	{
		printf("connect sentinel fail. ip:port=%s:%d", _ip, _port);
		return false;
	}	
	//
	CRedisSentinelUtil snt(&conn);
	if(_master)
	{
		std::string ip;
		int port;
		std::tie(ip, port) = snt.get_master_addr(_name);
		if(ip.length() == 0)
		{
			printf("not find the master:%s", _name);
			return false;
		}
		//	
		_addrs.emplace_back(std::make_pair(ip, port));
		return _addrs.size() > 0;
	}
	//slave
	const auto& vecTmp = snt.get_slaves(_name);
	for(auto i = 0u; i < vecTmp.size(); ++i)
	{
		_addrs.push_back(vecTmp[i]);
	}	
	//		
	return _addrs.size() > 0;
}
	
