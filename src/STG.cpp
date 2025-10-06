#include "STG.hpp"
#include "XPFW.h"
#include "DirectXMath.h"

struct ShaderData {
	GLuint program_2d;
	GLuint program_2d_no_tex;
	GLuint program_3d;
	GLuint program_3d_no_tex;
} shaders;

void Initialize(STGData* game_data) {
	// Default 2D shader
	GLuint vs, ps, pf;
	LoadShaderFromFile("T&L2D.vert", &vs, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &ps, GL_FRAGMENT_SHADER);
	CreateShaderProgram(vs, ps, &pf);
	glDeleteShader(vs);
	glDeleteShader(ps);
	shaders.program_2d = pf;

	// Initialize font render
	InitializeFreeType(&game_data->library);
	game_data->font_small = new font_t;
	game_data->font_normal = new font_t;
	game_data->font_big = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font_small, 16.0f);
	CreateFontWithAtlas(game_data->font_desc, game_data->font_normal, 24.0f);
	CreateFontWithAtlas(game_data->font_desc, game_data->font_big, 32.0f);

	InitializeGameMain(&game_data->game_main);

	game_data->proc_id = GAME_MAIN;

	// Other data
	game_data->past_time = 0.0f;
	game_data->all_time = 0.0f;

	game_data->lowest_fps = 1000.0f;
	game_data->highest_fps = -1000.0f;

	game_data->tick_spd = 1.0f;
}

void Move(window_t* window, void* data) {
	float delta_time = window->delta_time;
	STGData* game_data = (STGData*)data;

	switch (game_data->proc_id) {
	case GAME_MENU:
		MoveGameMain(&game_data->game_main, window->window);
		break;
	case GAME_MAIN:
		MoveGameMain(&game_data->game_main, window->window);
		break;
	case GAME_REPLAY:
		MoveGameMain(&game_data->game_main, window->window);
		break;
	}

	game_data->all_time += delta_time;
}

void Draw(window_t* window, void* data) {
	STGData* game_data = (STGData*)data;
	char buf[64] = "";
	const char* types[] = {"GameMenu", "GameMain", "GameReplay"};

	// Using default shader for 2D
	glUseProgram(shaders.program_2d);

	switch (game_data->proc_id) {
	case GAME_MENU:
		DrawGameMain(&game_data->game_main);
		break;
	case GAME_MAIN:
		DrawGameMain(&game_data->game_main);
		break;
	case GAME_REPLAY:
		DrawGameMain(&game_data->game_main);
		break;
	}

	// Text
	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 640.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);

	float fps = GetWindowFPS(window);
	if (fps > game_data->lowest_fps) game_data->lowest_fps = fps;
	else if (fps < game_data->lowest_fps && fps != INFINITY) game_data->highest_fps = fps;
	sprintf(buf, "%.2f fps", fps);
	DrawString(game_data->font_small, 0, 0, buf, 0xffffffff);
	sprintf(buf, "Time passed: %.2f s", game_data->all_time);
	DrawString(game_data->font_small, 0, 16, buf, 0xffffffff);
	sprintf(buf, "Proc: %s", types[game_data->proc_id % 3]);
	DrawString(game_data->font_small, 0, 464, buf, 0xffffffff);
}

void Destroy(STGData* game_data) {

}