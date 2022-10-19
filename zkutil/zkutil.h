/**
 * @file	zookeeper.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-10-17
 */
#ifndef _ZOOKEEPER_UTIL_H_
#define _ZOOKEEPER_UTIL_H_ 

#include "zookeeper/zookeeper.h"
#include "zookeeper/zookeeper.jute.h"
#include "zookeeper/zookeeper_log.h"

enum ZKErrCode
{
	kZKSucceed,				//操作成功, watch 继续生效
	kZKNotExist,			//节点不存在, 对于exist 操作watch 继续生效,其他操作均失效
	kZKError,				//请求失败, watch 失效 
	kZKDeleted,				//节点删除, watch 失效
	kZKExist,				//节点已存在, create fail 
	kZKNotEmpty				//节点有子节点, delete fail 
};

typedef std::function<void(void*)> 	expired_callback_fnc;
typedef  void(*ZKCREATEFNC)(ZKErrCode errCode, const char* path, const char* value, void* ctx);
typedef  void(*ZKEXISTFNC)(ZKErrCode errCode, const char* path, const struct Stat *stat, void* ctx);
typedef	 void(*ZKCHANGEFNC)(ZKErrCode errCode, const char* path, const struct Stat* stat, void* ctx); 
typedef  void(*ZKDELFNC)(ZKErrCode errCode, const char* path, void* ctx);

class CZKUtil;
struct zkWatchCtx
{
	zkWatchCtx(const char* _path, void* _context, CZKUtil* _zkClient, bool _watch)
	{
		zkClient = _zkClient;
		strcpy(path, _path);
		context = _context;
		watch = _watch;
	}
	CZKUtil		*zkClient;
	char		path[1024];
	void		*context;
	bool		watch;
	union {
		ZKCREATEFNC			create_fnc;
		ZKDELFNC			del_fnc;
		ZKEXISTFNC			exist_fnc;
		ZKCHANGEFNC			change_fnc;
	};
};


//async api
class CZKUtil
{
public:
	CZKUtil(const char* host, expired_callback_fnc fnc, int loglevel = ZOO_LOG_LEVEL_INFO);
	~CZKUtil();
public:
	//
	ZKErrCode create_node(const char* path, void* value, size_t value_len, int flag,  char*  out_path_buffer = NULL, int out_path_buffer_len = 0);
	
	ZKErrCode remove_node(const char* path, int version);
	
	int  get_node(const char* path, std::string& out);

	int  get_children(const char* path, std::vector<std::string>& childrens, bool watch);
	
	//async 
	bool create_node(const char* path, void* value, size_t value_len, int flag,  ZKCREATEFNC fnc, void* ctx);

	bool remove_node(const char* path, ZKDELFNC fnc, void* ctx);

	bool change_node(const char* path, void* value, size_t value_len, int flag, ZKCHANGEFNC fnc, void* ctx);

	void CheckState();
public:
	//global watch function
	static void zk_watch_global(zhandle_t *zh, int type, int state, const char* path, void* watchCtx);
	//delete callback
	static void zk_completion(int rc, const void *data);
	//set/exist/ change callback  	
	static void zk_stat_completion(int rc, const struct Stat *stat, const void* data);
	//create callback
	static void zk_string_completion(int rc, const char* value, const void* data);	
	//创建zk 回调	
	static void zk_create_completion(int rc, const char* value, const void* data);
	//delete zk callback
	static void zk_remove_completion(int rc, const void* data);
	//
	static void zk_change_completion(int rc, const struct Stat *stat, const void* data);
	//childrens callback	
	static void zk_strings_completion(int rc, const struct String_vector *strings, const void* data);
	//
	static void zk_strings_completion(int rc, const struct String_vector *strings, const Stat *stat, const void* data);
	//
	static void zk_acl_completion(int rc, struct ACL_vector *acl,  struct Stat *stat, const void* data);
private:
	void zk_open();
	void zk_destroy();
private:
	zhandle_t					*_zh;	
	char						_host[256];
	int							_session_stat;
	
	expired_callback_fnc		_fnc;
};


#endif 

