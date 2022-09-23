/**
 * @file	redis_mgr.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-23
 */
#ifndef _REDIS_MGR_H_
#define _REDIS_MGR_H_ 

#include "redisutil.h"
#include <mutex>

class CRedisMgr 
{
public:
	CRedisMgr();
	~CRedisMgr();
public:
	bool			Init(const char* ip, int port, const char* pwd, int size = 10);	
public:
	CRedisUtil*		GetConn();
	void			Reclaim(CRedisUtil *conn);
	void			Clear();
private:
	std::mutex						_lck;
	std::queue<CRedisUtil*>			_wait_conns;
private:
	int				_max_size;
	int				_cur_size;
private:
	char			_ip[16];
	int				_port;
	char			_pwd[64];
};


#endif 

