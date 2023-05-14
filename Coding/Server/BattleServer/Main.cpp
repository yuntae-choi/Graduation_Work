//==================================================================================================================================
//이 서버는 매칭된 플레이어들의 인게임 전투를 관리 합니다. 
// FPS 게임이 실제 플레이되는 서버
//==================================================================================================================================

#include "pch.h"
#include "CorePch.h"
#include "CLIENT.h"
#include "Overlap.h"
#include "LockQueue.h"
//#include "GameDataBase.h"
#include "BattleServer.h"

int main(int argc, char* argv[])
{

	wcout.imbue(locale("korean"));
	short server_port = -1;
	if (argc > 1)
		server_port = atoi(argv[1]);
	//Init_DB();
	server.Init(MAX_TH);
	if (server_port != -1)
	    server.BindListen(server_port);
	else
		server.BindListen(SERVER_PORT);
	server.Connect();

	server.Run();
	server.End();
	//DB_Rel();

}
