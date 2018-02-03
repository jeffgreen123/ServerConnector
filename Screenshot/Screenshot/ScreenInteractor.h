#define _CRT_SECURE_NO_WARNINGS 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#pragma comment (lib,"gdiplus.lib")
using namespace Gdiplus;
enum ImageQuality{
	VeryPoor= 1,
	Poor,
	Okay,
	Good

};
class ScreenInteractor
{
public:
	ScreenInteractor(char * name);
	bool TakeScreenshot();
	void LeftClick(int x, int y);
	void RightClick(int x, int y);
	void KeyPress(char * key, int size);
	void setImageQuality(ImageQuality qual);
	char *getScreenData() const;
	int getScreenDataLength() const;
	~ScreenInteractor();
private:
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	void MouseMove(int x, int y);
	int getKey(char text);
	char * screenData;
	int dataLength;
	char * fileName;
	ImageQuality imgqual;
};

