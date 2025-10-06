#ifndef STG_DEFINED
#define STG_DEFINED

#include "GameMain.hpp"
#include "OpenGL/Window.h"
#include "OpenGL/Font.h"

enum GAME_STATE {
	GAME_MENU, // Has access to load gamemain and exit the game
	GAME_MAIN, // Is the game itself
	GAME_REPLAY, // Is the replay of a game
};

struct STGData {
	FT_Library library;
	font_t* font_normal;
	font_t* font_small;
	font_t* font_big;
	font_descriptor_t font_desc;
	float past_time;
	float all_time;
	GLuint flat;
	GLuint volume;
	GLuint facea, faceb;

	// Procedure
	game_main_t game_main;
	GAME_STATE proc_id = GAME_MAIN;

	float lowest_fps;
	float highest_fps;

	float tick_spd;
};

extern struct ShaderData shaders;

void Initialize(STGData* game_data);
void Move(window_t* window, void* data);
void Draw(window_t* window, void* data);
void Destroy(STGData* game_data);

#endif