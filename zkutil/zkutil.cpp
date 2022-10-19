#include "zkutil.h"
#include "zookeeper/zookeeper.h"
#include "zookeeper/zookeeper_log.h"
#include <unordered_map> 

std::unordered_map<int, ZKErrCode>  rc_map = {
	{ ZOK, kZKSucceed },
	{ ZNONODE, kZKNotExist },
	{ ZNODEEXISTS, kZKExist },
	{ ZOK, kZKSucceed },
	{ ZOK, kZKSucceed },
}
	
CZKUtil::CZKUtil(const char* host, expired_callback_fnc fnc, int loglevel/* = ZOO_LOG_LEVEL_INFO*/):_zh(NULL), _fnc(fnc)
{
	if(host)
	{
		strcpy(_host, host);
	}
	zoo_set_debug_level(logLevel);
	zk_open();
}

CZKUtil::~CZKUtil()
{
	destroy();
}
	
ZKErrCode CZKUtil::create_node(const char* path, void* value, size_t value_len, int flag,  char*  path_buffer, int path_buffer_len)
{
	if(NULL == _zh)
		return kZKError;
	//
	int rc = zoo_create(_zh, path, (char*)value, value_len, &ZOO_OPEN_ACL_UNSAFE, flag, path_buffer, path_buffer_len);
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
	
//async 
bool CZKUtil::create_node(const char* path, void* value, size_t value_len, int flag,  ZKCREATEFNC fnc, void* ctx)
{
	zkWatchCtx  *watch_ctx = new zkWatchCtx(path, ctx, this, false);
	watch_ctx->create_fnc = fnc;
	int rc = zoo_acreate(_zh, path, value, value_len, &ZOO_OPEN_ACL_UNSAFE, flag, fnc, watch_ctx);
	return rc == ZOK;
}
	
bool CZKUtil::remove_node(const char* path, ZKDELFNC fnc, void* ctx)
{
	zkWatchCtx *watch_ctx = new zkWatchCtx(path, ctx, this, false);
	watch_ctx->del_fnc = fnc;
	return  zoo_adelete(_zh, path, -1, fnc, watch_ctx) == ZOK;
}
	
bool CZKUtil::change_node(const char* path, void* value, size_t value_len, int flag, ZKCHANGEFNC fnc, void* ctx)
{
	zkWatchCtx *watch_ctx = new zkWatchCtx(path, ctx, this, false);
	watch_ctx->del_fnc = fnc;
	return  zoo_aset(_zh, path, (char*)value, value_len, -1, fnc, watch_ctx) == ZOK;
}

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
	_zh = zookeeper_init(_host, CZKUtil::zk_watch_global, 30000, 0, this,0);
	if(NULL == _zh)
	{
		printf("init zookeeper fail.");
		return ;
	}	
	// 检测是否可以连接成功	
	
	while(_session_stat != ZOO_CONNECTED_STATE || _session_stat != ZOO_EXPIRED_SESSION_STATE){ ; }
	printf("connect zookeeper stat:%d", _session_stat);
//	if(_session_stat == ZOO_EXPIRED_SESSION_STATE)
//		return false;
//
//	return true;
}
	
//global watch function
static void CZKUtil::zk_watch_global(zhandle_t *zh, int type, int state, const char* path, void* watchCtx)
{
	CZKUtil* pTmp = (CZKUtil*)watchCtx;
	if(type == ZOO_SESSION_EVENT)
	{
		pTmp->_session_stat = state;
		printf("zookeeper session stat:%d\n", state);
		
	}
	//	
//	else if(type == ZOO_DELETED_EVENT)
//	{
//			
//	}
//	else if(type == ZOO_CHILD_EVENT)
//	{
//
//	}
//	else if(type == ZOO_CREATED_EVENT)
//	{
//		
//	}
//	else if(type == ZOO_CHANGED_EVENT)
//	{
//
//	}
//	else if(type == ZOO_NOTWATCHING_EVENT)
//	{
//
//	}
		
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
			watch_ctx->change_fnc(kZKSucceed, watch_ctx->path, watch_ctx->context); 
			break;
		case ZNONODE:
			watch_ctx->change_fnc(kZKNotExist, watch_ctx->path, watch_ctx->context); 
			break;
		default:
			watch_ctx->change_fnc(kZKError, watch_ctx->path, watch_ctx->context); 
			break;
		}
	delete watch_ctx;
}

//	
void CZKUtil::CheckState()
{
	
}
