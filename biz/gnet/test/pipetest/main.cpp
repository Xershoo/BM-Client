#include "pipe_client.h"
#include "pipe_server.h"
#include "protocols/rpcgen_voice.h"
#include "gnet/pipe/spipe/iothread.h"


void test_client();
void test_server();

PipClient client;
PipServer server;


int main()
{
	LOG << sizeof(long long);

	LOG << "start";
	rpcgen_voice::RegisterProtocols();
	gnet::IOThread::GetInst()->Start();

	//test_client();
	test_server();
	getchar();

	gnet::IOThread::GetInst()->Stop();
	rpcgen_voice::UnregisterProtocols();
	return 0;
}


void test_client()
{
	volatile bool bStop = false;
	client.Connect(L"123", 1000, bStop);
}


void test_server()
{
	server.Connect(L"123");

	STARTUPINFOW si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi;
	BOOL bRet = CreateProcessW(L"client.exe", NULL, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

	int kkk = 9;
}