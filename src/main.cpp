#include <stdio.h>
#include <stdlib.h>
#include <DirectXMath.h>

#include "XPFW.h"
#include "Sprite.hpp"
#include "Common.hpp"
#include "GameMain.hpp"

struct GameData{
	FT_Library library;
	font_t* font;
	font_descriptor_t font_desc;
	float past_time;
	float all_time;
	GLuint flat;
	GLuint volume;
	GLuint facea, faceb;

	// Procedure
	game_main_t game_main;
	void* current_proc = nullptr;
	int proc_id = 0;

	float lowest_fps;
	float highest_fps;

	float tick_spd;
};

void Initialize(GameData* game_data) {
	// Default 2D shader
	GLuint vs, ps, pf;
	LoadShaderFromFile("T&L2D.vert", &vs, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &ps, GL_FRAGMENT_SHADER);
	CreateShaderProgram(vs, ps, &pf);
	glDeleteShader(vs);
	glDeleteShader(ps);
	game_data->flat = pf;

	// Initialize font render
	InitializeFreeType(&game_data->library);
	game_data->font = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font, 24.0f);

	InitializeGameMain(&game_data->game_main);

	// Other data
	game_data->past_time = 0.0f;
	game_data->all_time = 0.0f;

	game_data->lowest_fps = 1000.0f;
	game_data->highest_fps = -1000.0f;

	game_data->tick_spd = 1.0f;
}

void Move(window_t* window,  void* data) {
	float delta_time = window->delta_time;
	GameData* game_data = (GameData*)data;

	MoveGameMain(&game_data->game_main, window->window);

	game_data->all_time += delta_time;
}

void Draw(window_t* window, void* data) {
	GameData* game_data = (GameData*)data;
	char buf[64] = "";

	glUseProgram(game_data->flat);

	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 640.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);

	DrawGameMain(&game_data->game_main);

	// Text
	float fps = GetWindowFPS(window);
	if (fps > game_data->lowest_fps) game_data->lowest_fps = fps;
	else if (fps < game_data->lowest_fps && fps != INFINITY) game_data->highest_fps = fps;
	sprintf(buf, "%.2f fps", fps);
	DrawString(game_data->font, 0, 0, buf, 0xffffffff);
	sprintf(buf, "Time passed: %.2f s", game_data->all_time);
	DrawString(game_data->font, 0, 24, buf, 0xffffffff);
	/*
	sprintf(buf, "X = %.3f", game_data->meiling.x);
	DrawString(game_data->font, 0, 48, buf, 0xffffffff);
	sprintf(buf, "Y = %.3f", game_data->meiling.y);
	DrawString(game_data->font, 0, 72, buf, 0xffffffff);

	sprintf(buf, "Dir = %.3f", atan2f(game_data->meiling.move.sdir, game_data->meiling.move.cdir));
	DrawString(game_data->font, 0, 288, buf, 0xffffffff);
	sprintf(buf, "DirS / FinalX = %.3f", game_data->meiling.move.sdir);
	DrawString(game_data->font, 0, 312, buf, 0xffffffff);
	sprintf(buf, "DirC / FinalY = %.3f",game_data->meiling.move.cdir);
	DrawString(game_data->font, 0, 336, buf, 0xffffffff);
	sprintf(buf, "Speed = %.3f", game_data->meiling.move.speed);
	DrawString(game_data->font, 0, 360, buf, 0xffffffff);
	*/
}

void Destroy(GameData* game_data) {

}

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

int main() {
	window_t window = { 0 };
	GameData* data = new GameData;
	CreateGLWindow("STG game", 640, 480, false, &window);
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
