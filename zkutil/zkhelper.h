/**
 * @file	zkhelper.h
 * @brief 
 * @author	ningbf
 * @version 1.0.0
 * @date	2022-10-20
 */
#ifndef _ZK_HELPER_H_
#define _ZK_HELPER_H_ 
#include "zkutil.h"


template<class T>
class CZkHelper 
{
public:
	CZkHelper(T* type){ _type = type }
	~CZkHelper(){}
public:
		
private:
	T*				_type;
	CZKUtil*		_zk;

	char			_path[256];
	char			_value[256];
};

#endif 
