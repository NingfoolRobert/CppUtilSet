/**
 * @file	redisutil.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-09-22
 */
#ifndef _REDIS_UTIL_H_
#define _REDIS_UTIL_H_ 

#include "hiredis/hiredis.h"
#include <cstdarg>
#include <cstdint>
#include <string>

class CRedisResult{
public:
	class CRedisReply{
	public:
		CRedisReply(redisReply* r){ _rep = r; }
		CRedisReply(const CRedisReply& r) { _rep = r._rep; }
		CRedisReply() { _rep = NULL; }
		~CRedisReply() { }
		inline  bool		check()const { if(_rep == NULL) return false; return true; }
		inline  int32_t		type() const { return _rep->type; }	
		inline  long long	integer() const { return _rep->integer; }
		inline  size_t		len()const { return _rep->len; }
		inline  char*		str()const {return _rep->str;}
		inline	size_t		elements() const {return _rep->elements;}
		inline  CRedisReply element(uint32_t index)const { return CRedisReply(_rep->element[index]); }
	private:
		friend class CRedisResult;
		redisReply*			_rep;
	};
public:
	CRedisResult(){}
	~CRedisResult() { if(_r._rep) freeReplyObject(_r._rep); }
	void				init(redisReply* r) { if(_r._rep)freeReplyObject(_r._rep);  _r._rep = r;} 	
	bool				check() { return _r.check(); }
	inline  int32_t		type() const { return _r.type(); }	
	inline  long long	integer() const { return _r.integer(); }
	inline  size_t		len()const { return _r.len(); }
	inline  char*		str()const {return _r.str();}
	inline	size_t		elements() const {return _r.elements();}
	inline  CRedisReply element(uint32_t index)const { return _r.element(index); }
private:
	CRedisReply			_r;
};
//////////////////

class CRedisUtil 
{
public:
	CRedisUtil();
	~CRedisUtil();
public:
	bool  Connect(const char*  ip,  short port = 6379, const char* pwd = "", int timeout = 10000);
	void  Command(CRedisResult &result,  const char* fmt, ...);	
	bool  Ping();
public:
private:
	bool  Auth();
	void  Command(CRedisResult &result,  const char* fmt, va_list args);
private:
	CRedisUtil& operator=(CRedisUtil&){ return *this; }
	CRedisUtil(CRedisResult&){}
private:
	char				_ip[16];
	int					_port;	
	char				_pwd[64];
	int					_timeout; 
	
private:
	redisContext		*_cxt;
};



#endif 

