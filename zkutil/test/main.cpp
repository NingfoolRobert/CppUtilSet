
#include <stdio.h> 
#include <stdlib.h> 


#include "../zkutil.h"
#include "zookeeper/zookeeper.h"

const char host[] = {"127.0.0.1:2181"};
const int timeout = 30000;

void test_watch_node(zhandle_t *zh, int type, int stat, const char* path, void* ctx)
{
	printf("watch node: %d, path:%s", stat, path);
}

int main()
{
	CZKUtil zk(host, timeout, ZOO_LOG_LEVEL_DEBUG);
	char szout[128] = { 0 };
//	if(zk.create_node("/testone", "alive", 6, ZOO_PERSISTENT, szout,127) == kZKSucceed)
//	{
//		printf("set node success.\n");
//	}
	
	szout[0] = 0;
	if(zk.create_node("/master-snapservice/snapservice", "alive", 6, ZOO_EPHEMERAL | ZOO_SEQUENCE, szout,127) == kZKSucceed)
	{
		printf("create sub node success.");
	}

	//
	std::string out;
	zk.get_node(szout, out);
	//	
	if(!zk.watch_node(szout, test_watch_node))
	{
		printf("watch fail.");
	}

	std::vector<std::string> vecTmp;
	zk.get_children("/master-snapservice", vecTmp, false);
	for(auto i = 0; i < vecTmp.size(); ++i)
		printf("child:%d, %s\n", i, vecTmp[i].c_str());
	
	printf("\n");
	//
	getchar();
	return 0;
}
