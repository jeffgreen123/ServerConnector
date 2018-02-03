#include "ScreenInteractor.h"

ScreenInteractor::ScreenInteractor(char * name)
{
	fileName = name;
	screenData = NULL;
	imgqual = Good;
}
bool ScreenInteractor::TakeScreenshot()
{
	//setup
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//setup desktop bitmaps;
	HDC scrdc, memdc;
	HBITMAP membit;
	scrdc = GetDC(0);

	// setup dest height and src height
	unsigned int Height, Width;
	unsigned int nHeight = 768;
	unsigned int nWidth = 1280;

	Height = GetSystemMetrics(SM_CYSCREEN);
	Width = GetSystemMetrics(SM_CXSCREEN);

	//setup bitmap
	memdc = CreateCompatibleDC(scrdc);
	membit = CreateCompatibleBitmap(scrdc, nWidth, nHeight);
	SelectObject(memdc, membit);

	//setup image compression quality(will change file size dramatically)
	switch (imgqual){
		case Good:
		case Poor:
			SetStretchBltMode(memdc, HALFTONE);
			break;
		case Okay:
		case VeryPoor:
			SetStretchBltMode(memdc, BLACKONWHITE);
			break;
		default:
			break;
	}

	//compress
	StretchBlt(memdc, 0, 0, nWidth, nHeight, scrdc, 0, 0, Width, Height, SRCCOPY);

	///make bitmap
	HBITMAP hBitmap;
	hBitmap = static_cast <HBITMAP>(SelectObject(memdc, membit));
	Gdiplus::Bitmap bitmap(hBitmap, NULL);

	BitmapData bmd;
	//make bitmap greyscale for lower quality
	if (imgqual == Poor || imgqual == VeryPoor){
		bitmap.LockBits(new Rect(0, 0, nWidth, nHeight), ImageLockModeRead, bitmap.GetPixelFormat(), &bmd);
		int PixelSize = 4;
		for (int y = 0; y < bmd.Height; y++){
			byte* row = (byte *)bmd.Scan0 + (y*bmd.Stride);
			for (int x = 0; x < bmd.Width; x++){
				int lum = (row[x*PixelSize] * 0.30) + (row[x*PixelSize + 1] * 0.59) + (row[x*PixelSize + 2] * 0.11);
				row[x*PixelSize] = lum;
				row[x*PixelSize + 1] = lum;
				row[x*PixelSize + 2] = lum;

			}

		}
		bitmap.UnlockBits(&bmd);
	}
	// draw the image onto the bitmap
	Gdiplus::Bitmap bm(nWidth, nHeight, PixelFormat8bppIndexed);
	Graphics g(&bm);
	g.DrawImage(&bitmap, 0, 0);

	//set encode type and save (must be done form compression to work)
	CLSID   encoderClsid;
	GetEncoderClsid(L"image/png", &encoderClsid);

	//bitmap.Save(L"D:\\screen.png", &encoderClsid, NULL);
	bm.Save(L"D:\\screen.png", &encoderClsid, NULL);
	DeleteObject(hBitmap);

	// open and put data into char a* fro sending
	FILE * file;
	file = fopen(fileName, "rb");
	if (!file)
	{
		return false;
	}

	fseek(file, 0, SEEK_END);
	dataLength = ftell(file);
	fseek(file, 0, SEEK_SET);
	screenData = new char[dataLength];
	fread(screenData, dataLength, 1, file);
	fclose(file);

	return true;

}
void ScreenInteractor::LeftClick(int x, int y)
{

	MouseMove(x, y);
	INPUT    Input = { 0 };

	// left down 
	Sleep(100);
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(100);

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));

}
void ScreenInteractor::RightClick(int x,int y)
{
	MouseMove(x, y);
	INPUT    Input = { 0 };

	// right down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(100);

	// right up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

void ScreenInteractor::KeyPress(char * text, int size)
{
	INPUT ip;
	INPUT shift; //handle cases where shift is held down

	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; 
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	shift.type = INPUT_KEYBOARD;
	shift.ki.wScan = 0;
	shift.ki.time = 0;
	shift.ki.dwExtraInfo = 0;
	shift.ki.wVk = VK_SHIFT; // virtual-key code for the "shift" key

	for (int i = 0; i < size; i++){
		if (text[i]){
			//handle shift key presses
			switch (text[i]){
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
				case 'H':
				case 'I':
				case 'J':
				case 'K':
				case 'L':
				case 'M':
				case 'N':
				case 'O':
				case 'P':
				case 'Q':
				case 'R':
				case 'S':
				case 'T':
				case 'U':
				case 'V':
				case 'W':
				case 'X':
				case 'Y':
				case 'Z':
				case '~':
				case '!':
				case '@':
				case '#':
				case '$':
				case '%':
				case '^':
				case '&':
				case '*':
				case '(':
				case ')':
				case '_':
				case '+':
				case '{':
				case '}':
				case '"':
				case ':':
				case '?':
				case '<':
				case '>':
					shift.ki.dwFlags = 0; // 0 for key press
					// hold down shift key
					SendInput(1, &shift, sizeof(INPUT));
					break;
				default:
					break;
			}

			ip.ki.wVk = getKey(text[i]); 

			//handle special keys
			if (text[i] == '\r'){
				ip.ki.wVk = 13;
			}
			if (text[i] == 'Φ'){
				ip.ki.wVk = 8;
			}

			ip.ki.dwFlags = 0; // 0 for key press
			SendInput(1, &ip, sizeof(INPUT));

			ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
			SendInput(1, &ip, sizeof(INPUT));
			
		}
		else{
			// once no more text exists -- break
			break;
		}
	}
}

void ScreenInteractor::setImageQuality(ImageQuality qual){
	imgqual = qual;
}

char*ScreenInteractor::getScreenData() const{

	FILE * pfile = fopen("D:/screen2.png", "wb");
	fwrite((const char*)screenData, 1, dataLength, pfile);
	fclose(pfile);

	return screenData;
}

int ScreenInteractor::getScreenDataLength() const{
	return dataLength;
}

int ScreenInteractor::GetEncoderClsid(const WCHAR* format, CLSID* pClsid){

	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

void ScreenInteractor::MouseMove(int x, int y)
{
	double fScreenWidth = GetSystemMetrics(SM_CXSCREEN) - 1;
	double fScreenHeight = GetSystemMetrics(SM_CYSCREEN) - 1;
	double fx = x*(65535.0f / fScreenWidth);
	double fy = y*(65535.0f / fScreenHeight);

	INPUT  Input = { 0 };

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input.mi.dx = fx;
	Input.mi.dy = fy;
	SendInput(1, &Input, sizeof(INPUT));
}

//maps key values to integer values
int ScreenInteractor::getKey(char text){

	std::string line;
	int ret = 0;
	std::ifstream myfile("D:/VirtualKeys.txt");

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (line.length() > 0 && line.at(0) == text){
			ret = atoi(line.substr(0 + 2, line.length() - 2).c_str());
				break;
			}
		}
		myfile.close();
	}

  return ret;
}
ScreenInteractor::~ScreenInteractor(){} 
