#include "redis_mgr.h"
#include "redisutil.h"
#include <mutex>
	
CRedisMgr::CRedisMgr():_max_size(20)
{
}
	
CRedisMgr::~CRedisMgr()
{
	Clear();
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
	if(_cur_size >= _max_size)
		return NULL;
	//
	pConn = new CRedisUtil;
	if(pConn == NULL)
	{
		printf("memory error.");
		return NULL;
	}
	
	if(!pConn->Connect(_ip, _port, _pwd))
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
}
