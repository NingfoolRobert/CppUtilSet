#include "redisutil.h"
#include "hiredis/hiredis.h"
#include <cstdarg>
#include <cstddef>
#include <functional>
#include <memory>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

CRedisUtil::CRedisUtil():_port(6379),_timeout(1000),_cxt(NULL)
{
	memset(_ip, 0, sizeof(_ip));
	memset(_pwd, 0, sizeof(_pwd));
}
CRedisUtil::~CRedisUtil()
{
	if(NULL != _cxt)
	{
		redisFree(_cxt);
		_cxt = NULL;
	}
}

bool CRedisUtil::Connect(const char*  ip,  short port /*= 6379*/, const char* pwd, int timeout/* = 1000*/)
{
	if(nullptr == ip || nullptr == pwd)
		return false;
	strcpy(_ip, ip);
	_port = port;
	strcpy(_pwd, pwd);
	//
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000 ) * 1000;
	_cxt = redisConnectWithTimeout(_ip, _port, tv);
	if(NULL == _cxt || _cxt->err)
	{
		printf("Connect Redis fail. ip:port=%s:%d, err:%d(%s)", _ip, _port, _cxt?_cxt->err:0, _cxt? _cxt->errstr:"");	
		return false;
	}
	//	
	if(!Auth())
		return false;
	return true;	
}
//
bool CRedisUtil::Ping()
{
	CRedisResult r;
	Command(r, "PING");
	if(r.check() && strcmp(r.str(), "PONG") == 0)
	{
		return true;
	}
	//
	printf("ping fail. ip:port=%s:%d", _ip, _port);
	return false;
}
//
void  CRedisUtil::Command(CRedisResult &result,  const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Command(result, fmt, args);	
	va_end(args);
}
	
bool  CRedisUtil::Auth()
{
	if(0 == strlen(_pwd))
		return true;
	//
	CRedisResult r;
	Command(r, "AUTH %s", _pwd);
	if(!r.check() || strcmp(r.str(), "OK") != 0)
	{
		printf("Auth fail. ip:port=%s:%d,  pwd:%s", _ip, _port, _pwd);
		return false;
	}	
	return true;
}
//
void  CRedisUtil::Command(CRedisResult &result,  const char* fmt, va_list args)
{
	auto r = (redisReply*)redisvCommand(_cxt, fmt, args);
	result.init(r);
}
	
