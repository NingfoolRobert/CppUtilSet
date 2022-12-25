#include "zkutil.h"
#include "zookeeper/zookeeper.h"
#include "zookeeper/zookeeper.jute.h"
#include "zookeeper/zookeeper_log.h"
#include <cstdio>
#include <ctime>
#include <exception>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unordered_map> 
#include <utility>
#include <string.h>
#include <cstring>
#include <memory>
#include <algorithm>

std::unordered_map<int, ZKErrCode>  rc_map = {
	{ ZOK, kZKSucceed },
	{ ZNONODE, kZKNotExist },
	{ ZNODEEXISTS, kZKExist },
	{ ZOK, kZKSucceed },
	{ ZOK, kZKSucceed },
};
	
CZKUtil::CZKUtil(const char* host, int timeout, const char* logname/*= ""*/, int loglevel/* = ZOO_LOG_LEVEL_INFO*/):_zh(NULL), 
	_session_timeout(timeout),
	_session_stat(0)
{
	if(host)
	{
		strcpy(_host, host);
	}

	if(logname && strlen(logname) > 0) 
	{
		strcpy(_logfile, logname);
		_pFile = fopen(_logfile, "a+");
		if(_pFile)
			zoo_set_log_stream(_pFile);	
	}
	//
	zoo_set_debug_level(logLevel);
	zk_open();
}

CZKUtil::~CZKUtil()
{
	zk_destroy();
	if(_pFile)
	{
		fclose(_pFile);
		_pFile = NULL;
	}
}
	
bool CZKUtil::is_connected()const{
	const int state = zoo_state(_zh);
	return ZOO_CONNECTED_STATE == state;
}
	
std::pair<std::string, int>  CZKUtil::zk_get_connect_addr()
{
	struct sockaddr_in6  addr_in6;
	socklen_t addr_len = sizeof(addr_in6);
	if(NULL == zookeeper_get_connected_host(_zh, (struct sockaddr*)&addr_in6, &addr_len))
	{
		return std::make_pair("", 0);
	}
	const struct sockaddr* addr = reinterpret_cast<const struct sockaddr*>(&addr_in6);
	
	char szIP[16] = { 0 };
	if(AF_INET == addr->sa_family)
	{
		struct sockaddr_in *addr_in = reinterpret_cast<struct sockaddr_in*>(&addr_in6);
		return std::make_pair(inet_ntop(AF_INET, &addr_in->sin_addr, szIP, 16), ntohs(addr_in->sin_port));
	}
	else if(AF_INET6 == addr->sa_family)
	{
		return std::make_pair(inet_ntop(AF_INET6, &addr_in6.sin6_addr,szIP, 16), ntohs(addr_in6.sin6_port));
	}

	return std::make_pair("", 0);
}
	
ZKErrCode CZKUtil::create_node(const char* path, const char* value, size_t value_len, int flag,  char*  path_buffer, int path_buffer_len)
{
	if(NULL == _zh)
		return kZKError;		
	//
	int rc = zoo_create(_zh, path, value, value_len, &ZOO_OPEN_ACL_UNSAFE, flag, path_buffer, path_buffer_len);
	switch(rc)
	{
		case ZOK:
			return kZKSucceed;
		case ZNONODE:
			return kZKNotExist;
		case ZNODEEXISTS:
			return kZKExist;
		default:
			break;
	}			
	return kZKError;
}
	
ZKErrCode CZKUtil::remove_node(const char* path, int version)
{
	if(NULL == _zh)
		return kZKError;
	int rc = zoo_delete(_zh, path, -1);
	switch(rc)
	{
		case ZOK:
			return kZKSucceed;
		case ZNONODE:
			return kZKNotExist;
		default:
			break;
	}	
	return kZKError;
}
	
int  CZKUtil::get_node(const char* path, std::string& out)
{
	char szTmp[256] = { 0 };
	int buffer_len = 255;
	int rc = zoo_get(_zh, path, 1, szTmp, &buffer_len, NULL);

	out = szTmp;
	return rc;

}
//
ZKErrCode CZKUtil::get_children(const char* path, std::vector<std::string>& childrens, bool watch)
{
	childrens.clear();
	struct String_vector strVec= { 0, NULL };
	int rc = zoo_get_children(_zh, path, 1, &strVec);
	
	for(auto i = 0u; i < strVec.count; i++)
		childrens.emplace_back(strVec.data[i]);
	//
	std::sort(childrens.begin(), childrens.end());

	switch(rc)
	{
		case ZOK:
			return kZKSucceed;
			break;
		case ZNONODE:
			return kZKNotExist;
		   break;	
	}	
	return kZKError;
}
	
//async 
//bool CZKUtil::create_node(const char* path, void* value, size_t value_len, int flag,  ZKCREATEFNC fnc, void* ctx)
//{
//	zkWatchCtx  *watch_ctx = new zkWatchCtx(path, ctx, this, false);
//	watch_ctx->create_fnc = fnc;
//	int rc = zoo_acreate(_zh, path, value, value_len, &ZOO_OPEN_ACL_UNSAFE, flag, fnc, watch_ctx);
//	return rc == ZOK;
//}
//	
//bool CZKUtil::remove_node(const char* path, ZKDELFNC fnc, void* ctx)
//{
//	zkWatchCtx *watch_ctx = new zkWatchCtx(path, ctx, this, false);
//	watch_ctx->del_fnc = fnc;
//	return  zoo_adelete(_zh, path, -1, fnc, watch_ctx) == ZOK;
//}
//	
//bool CZKUtil::change_node(const char* path, void* value, size_t value_len, int flag, ZKCHANGEFNC fnc, void* ctx)
//{
//	zkWatchCtx *watch_ctx = new zkWatchCtx(path, ctx, this, false);
//	watch_ctx->del_fnc = fnc;
//	return  zoo_aset(_zh, path, (char*)value, value_len, -1, fnc, watch_ctx) == ZOK;
//}

void CZKUtil::zk_destroy()
{
	if(_zh)
	{
		zookeeper_close(_zh);
		_zh = NULL;
	}
	
}
	
void CZKUtil::zk_open()
{
	std::unique_lock<std::mutex> _(_lck);
	if(_zh)
	{
		zookeeper_close(_zh);
		_zh = NULL;
	}
	//
	_zh = zookeeper_init(_host, CZKUtil::zk_watch_global, _session_timeout, 0, this,0);
	if(NULL == _zh)
	{
		printf("init zookeeper fail.");
		return ;
	}	
	// 检测是否可以连接成功	
	//while(_session_stat != ZOO_CONNECTED_STATE || _session_stat != ZOO_EXPIRED_SESSION_STATE){ ; }
	_cond.wait(_, [this]()->bool { return _session_stat == ZOO_CONNECTED_STATE || _session_stat == ZOO_EXPIRED_SESSION_STATE;  });
	printf("\nconnect zookeeper stat:%d\n", _session_stat);
//	if(_session_stat == ZOO_EXPIRED_SESSION_STATE)
//		return false;
//
//	return true;
}
	
//global watch function
void CZKUtil::zk_watch_global(zhandle_t *zh, int type, int state, const char* path, void* watchCtx)
{
	CZKUtil* pTmp = (CZKUtil*)watchCtx;
	if(type == ZOO_SESSION_EVENT)
	{
		pTmp->_session_stat = state;
		if(ZOO_EXPIRED_SESSION_STATE == state)
		{
			pTmp->zk_session_expired(path);
		
		}
		printf("\nzookeeper session stat:%d\n", state);
		
		pTmp->_cond.notify_one();	
	}
	else 
	{
		printf("\nzookeeper event:%d, path:%s\n", type, path);		
	}
	//	
		
}
	
//创建zk 回调	
void CZKUtil::zk_create_completion(int rc, const char* value, const void* data)
{
	const zkWatchCtx *watch_ctx = reinterpret_cast<const zkWatchCtx*>(data);
	switch(rc){
		case ZOK:
			watch_ctx->create_fnc(kZKSucceed, watch_ctx->path, "", watch_ctx->context);	
			break;
		case ZNONODE:
			watch_ctx->create_fnc(kZKNotExist, watch_ctx->path, "", watch_ctx->context);
			break;
		case kZKExist:
			watch_ctx->create_fnc(kZKExist, watch_ctx->path, "", watch_ctx->context);
			break;
		default:
			watch_ctx->create_fnc(kZKError, watch_ctx->path, "", watch_ctx->context);
			break;
	}
	
	delete watch_ctx;
}
	
//delete zk callback
void CZKUtil::zk_remove_completion(int rc, const void* data)
{
	const zkWatchCtx *watch_ctx = reinterpret_cast<const zkWatchCtx*>(data);
	switch(rc){
		case ZOK:
			watch_ctx->del_fnc(kZKSucceed, watch_ctx->path, watch_ctx->context); 
			break;
		case ZNONODE:
			watch_ctx->del_fnc(kZKNotExist, watch_ctx->path, watch_ctx->context); 
			break;
		case ZNOTEMPTY:
			watch_ctx->del_fnc(kZKNotEmpty, watch_ctx->path, watch_ctx->context); 
			break;
		default:
			watch_ctx->del_fnc(kZKError, watch_ctx->path, watch_ctx->context); 
			break;
	}
	delete watch_ctx;
}
	
void CZKUtil::zk_change_completion(int rc, const struct Stat *stat, const void* data)
{
	const zkWatchCtx *watch_ctx = reinterpret_cast<const zkWatchCtx*>(data);
	switch(rc){
		case ZOK:
			watch_ctx->change_fnc(kZKSucceed, watch_ctx->path, stat,watch_ctx->context); 
			break;
		case ZNONODE:
			watch_ctx->change_fnc(kZKNotExist, watch_ctx->path, stat, watch_ctx->context); 
			break;
		default:
			watch_ctx->change_fnc(kZKError, watch_ctx->path, stat, watch_ctx->context); 
			break;
		}
	delete watch_ctx;
}

//	
void CZKUtil::CheckState()
{
	
}
	
void CZKUtil::zk_session_expired(const char* path)
{
	//	
	printf("session disconnect....\n");
	zk_reconnect();
}
	
void CZKUtil::zk_reconnect()
{
	zk_destroy();
	zk_open();
}
	
bool CZKUtil::watch_node(const char* path, watcher_fn fnc)
{
	int rc = zoo_wexists(_zh, path,  fnc, this,  NULL);
	switch(rc)
	{
		case ZOK:
			return true;
		default:
			break;
	}
	return false;
}
	
void CZKUtil::zk_watch_node(zhandle_t* zh, int type, int state, const char* path, void* watchCtx)
{
	CZKUtil* pTmp = reinterpret_cast<CZKUtil*>(watchCtx);		
	printf("watch path:%s, %d %d\n", path,type,  state);	
}
