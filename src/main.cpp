#include <stdio.h>
#include <stdlib.h>
#include "STG.hpp"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

int main() {
	window_t window = { 0 };
	STGData* data = new STGData;
	CreateGLWindow("STG game", 1280, 960, false, &window);
	Initialize(data);
	RunMainLoop(&window, data, Move, Draw);

	Destroy(data);
#ifdef WIN32
	char buf[512];
	sprintf(buf, "Highest FPS: %.2f\n Lowest FPS: %.2f",data->highest_fps, data->lowest_fps);
	//MessageBox(NULL, buf, " ", MB_OK);
#endif
	delete data;
}
