#define _CRT_SECURE_NO_WARNINGS 1
#include "Server.h"
#include "ScreenInteractor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	ScreenInteractor interactor("D:\\screen.png");
	Server s("27015");
	//variables for command
	int size,x,y;
	char * data;
	s.SetupServer();
	FILE * pFile;
	int command = 0;
	do{

		interactor.TakeScreenshot(); //take intial screenshot
		s.WaitForClient();
		std::string line;
		int ret = 0;
		pFile = fopen("Log.txt", "a+"); //fputs are for logging info

		x = 0;
		y = 0;
		std::string coords;
		size = 0;
		data = NULL;
		do{
			//get intial byte
			command = atoi(s.RecieveData(1));
			switch (command){
				//handle commands
				case MOUSERIGHT:
					coords = s.RecieveData(MAX_PATH);
					x = atoi(coords.substr(0, coords.find(',')).c_str());
					y = atoi(coords.substr(coords.find(',') + 1, coords.length() - coords.find(',') - 1).c_str());
					fputs("x: ",pFile);  fputs(coords.substr(0, coords.find(',')).c_str(),pFile);
					fputs("y: ",pFile); fputs(coords.substr(coords.find(',') + 1, coords.length() - coords.find(',') - 1).c_str(),pFile) ;
					fputs("\n", pFile);
					interactor.RightClick(x, y);
					fputs("click executed", pFile);
					fputs("\n", pFile);
					break;
				case MOUSELEFT:
					coords = s.RecieveData(MAX_PATH);
					x = atoi(coords.substr(0,coords.find(',')).c_str());
					y = atoi(coords.substr(coords.find(',') + 1, coords.length() - coords.find(',') -1).c_str());
					fputs("x: ", pFile);  fputs(coords.substr(0, coords.find(',')).c_str(), pFile);
					fputs("y: ", pFile); fputs(coords.substr(coords.find(',') + 1, coords.length() - coords.find(',') - 1).c_str(), pFile);
					fputs("\n", pFile);
					interactor.LeftClick(x, y);
					fputs("click executed",pFile);
					fputs("\n", pFile);
					break;
				case MOUSELEFTDOUBLE:
					coords = s.RecieveData(MAX_PATH); 
					x = atoi(coords.substr(0, coords.find(',')).c_str());
					y = atoi(coords.substr(coords.find(',') + 1, coords.length() - coords.find(',') - 1).c_str());
					fputs("x: ", pFile);  fputs(coords.substr(0, coords.find(',')).c_str(), pFile);
					fputs("y: ", pFile); fputs(coords.substr(coords.find(',') + 1, coords.length() - coords.find(',') - 1).c_str(), pFile);
					fputs("\n", pFile);
					interactor.LeftClick(x, y);
					interactor.LeftClick(x, y);
					fputs("click executed", pFile);
					fputs("\n", pFile);
					break;
				case KEYPRESS:
					size = atoi(s.RecieveData(MAX_PATH));
					data = s.RecieveData(size);
					fputs("key pressed: ",pFile);
					fputs(data, pFile);
					fputs("\n",pFile);
					interactor.KeyPress(data, size);
					fputs("press executed", pFile);
					fputs("\n", pFile);
					break;
				case SCREENSHOT:
					fputs("Screenshot ask", pFile);
					fputs("\n", pFile);
					interactor.TakeScreenshot();
					data = new char[interactor.getScreenDataLength()];
					s.SendData(_itoa(interactor.getScreenDataLength(),data,10), MAX_PATH);
					s.SendData(interactor.getScreenData(),interactor.getScreenDataLength());
					fputs("Screenshot sent",pFile);
					fputs("\n",pFile);
					break;
				case CHANGEIMGQUAL:
					interactor.setImageQuality(static_cast<ImageQuality>(atoi(s.RecieveData(1))));
					fputs("change img qual done",pFile);
					fputs("\n",pFile);
					break;
				case DISCONNECT:
					s.ShutdownSocket();
					break;
				case SHUTDOWNSERVER:
					s.ShutdownSocket();
					WSACleanup();
					break;
			default:
				s.ShutdownSocket();
				break;
			}
		} while (command != DISCONNECT && command != SHUTDOWNSERVER && command != 0);
		fputs("Disconnect", pFile);
		fputs("\n", pFile);
		fclose(pFile);
	} while (command != SHUTDOWNSERVER);
	pFile = fopen("D:/Log.txt", "a+");
	fputs("Server Shutdown", pFile);
	return 0;
}