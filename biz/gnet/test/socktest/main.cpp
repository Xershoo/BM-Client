#include "sock_client.h"
#include "sock_server.h"
#include "gnet/socket/net.h"
#include "../pipetest/protocols/rpcgen_voice.h"


void test_client();
void test_server();


ServerManager svManager;
ClientManager cliManager;



int main()
{
	rpcgen_voice::RegisterProtocols();
	gnet::Engine::GetInst()->StartUp();

	//test_client();
	test_server();
	getchar();

	gnet::Engine::GetInst()->CleanUp();
	rpcgen_voice::UnregisterProtocols();
	return 0;
}


void test_client()
{
	cliManager.Connect("localhost", "5000", NULL);
	getchar();
	cliManager.Close(1, "close info");
}


void test_server()
{
	svManager.Listen("localhost", "5000", NULL);

	STARTUPINFOW si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi;
	CreateProcessW(L"client.exe", NULL, NULL, NULL,
		FALSE, NORMAL_PRIORITY_CLASS|CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
}