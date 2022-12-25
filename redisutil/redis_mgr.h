/**
 * @file	redis_mgr.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-23
 */
#ifndef _REDIS_MGR_H_
#define _REDIS_MGR_H_ 

#include "redis_sentinel_util.h"
#include "redisutil.h"
#include <mutex>
#include <queue>

class CRedisMgr 
{
public:
	CRedisMgr();
	~CRedisMgr();
public:
	bool			Init(const char* ip, int port, const char* pwd, int size = 10);	
	bool			InitSentinel(const char* ip, int port, const char* pwd, const char* name, bool bmaster = true,  int size= 10);
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
	char			_ip[16];	//sentinel 
	int				_port;		//sentinel
	char			_pwd[64];

	char			_name[32];
	int				_master;
	int				_discon_cnt;

	std::vector<std::pair<std::string, int>>  _addrs;//ip 	
};


#endif 

