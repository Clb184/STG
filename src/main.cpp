#include <stdio.h>
#include <stdlib.h>
#include <DirectXMath.h>

#include "XPFW.h"

#define RAD(x) (x * 180.0 / 3.14159)

typedef struct {
	float u0;
	float u1;
	float v0;
	float v1;
} quad_t;

typedef struct {
	FT_Library library;
	font_t* font;
	font_descriptor_t font_desc;
	float past_time;
	GLuint flat;
	GLuint volume;
	GLuint meiling_tex, meiling_vb, meiling_va;
	float var;
	quad_t quad;
} GameData;

void RotateQuad(TLVertex2D* vertex, float angle, float w, float h, float x, float y) {
	float s, c;
	float hw = 0.5 * w, hh = 0.5 * h;
	DirectX::XMScalarSinCos(&s, &c, angle);
	vertex[0].x = c * -hw - s * -hh + x;
	vertex[0].y = s * -hw + c * -hh + y;

	vertex[1].x = c * -hw - s * hh + x;
	vertex[1].y = s * -hw + c * hh + y;

	vertex[2].x = c * hw - s * -hh + x;
	vertex[2].y = s * hw + c * -hh + y;

	vertex[3].x = c * hw - s * hh + x;
	vertex[3].y = s * hw + c * hh + y;
}

void SetQuadUV(TLVertex2D* vertex, quad_t* quad, uint32_t color) {
	vertex[0].u = quad->u0;
	vertex[0].v = quad->v0;
	vertex[0].color = color;
	vertex[1].u = quad->u0;
	vertex[1].v = quad->v1;
	vertex[1].color = color;
	vertex[2].u = quad->u1;
	vertex[2].v = quad->v0;
	vertex[2].color = color;
	vertex[3].u = quad->u1;
	vertex[3].v = quad->v1;
	vertex[3].color = color;
}

void Initialize(GameData* game_data) {
	GLuint vs, ps, pf;
	LoadShaderFromFile("T&L2D.vert", &vs, GL_VERTEX_SHADER);
	LoadShaderFromFile("T&L2D.frag", &ps, GL_FRAGMENT_SHADER);
	CreateShaderProgram(vs, ps, &pf);
	glDeleteShader(vs);
	glDeleteShader(ps);
	game_data->flat = pf;

	LoadTextureFromFile("boss10.png", &game_data->meiling_tex, nullptr);
	CreateTL2DVertexBuffer(4, nullptr, GL_DYNAMIC_DRAW, &game_data->meiling_vb, &game_data->meiling_va);
	game_data->var = 0.0f;

	game_data->quad = { 0.0f, 0.25f, 0.0f, 0.25f };

	InitializeFreeType(&game_data->library);
	game_data->font = new font_t;
	LoadFontFromFile(game_data->library, &game_data->font_desc, "PermanentMarker.ttf");
	CreateFontWithAtlas(game_data->font_desc, game_data->font, 24.0f);
	game_data->past_time = 0.0f;
}

LOOP_FN(GameMain) {
	const float logic_step = 1.0 / 60.0f;
	GameData* game_data = (GameData*)data;
	glUseProgram(game_data->flat);
	glBindTextureUnit(0, game_data->meiling_tex);
	glBindVertexArray(game_data->meiling_va);

	DirectX::XMMATRIX proj = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 640.0f, 480.0f, 0.0f, 1.0f, 1000.0f);
	glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat*)&proj);

	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(game_data->meiling_vb, GL_WRITE_ONLY);
	RotateQuad(vertex, 0, 64.0f, 64.0f, 320.0f, 240.0f + DirectX::XMScalarSin(game_data->var * 2.0f) * 30.0f);
	SetQuadUV(vertex, &game_data->quad, 0xffffffff);
	
	if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_1)) {
		game_data->quad.u0 = 0.0f;
		game_data->quad.u1 = 0.25f;
		game_data->quad.v0 = 0.0f;
		game_data->quad.v1 = 0.25f;
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_2)) {
		game_data->quad.u0 = 0.25f;
		game_data->quad.u1 = 0.50f;
		game_data->quad.v0 = 0.0f;
		game_data->quad.v1 = 0.25f;
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_3)) {
		game_data->quad.u0 = 0.50f;
		game_data->quad.u1 = 0.75f;
		game_data->quad.v0 = 0.0f;
		game_data->quad.v1 = 0.25f;
	}
	else if (GLFW_PRESS == glfwGetKey(window->window, GLFW_KEY_4)) {
		game_data->quad.u0 = 0.75f;
		game_data->quad.u1 = 1.00f;
		game_data->quad.v0 = 0.0f;
		game_data->quad.v1 = 0.25f;
	}

	glUnmapNamedBuffer(game_data->meiling_vb);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	if (game_data->past_time >= logic_step) {
		// Do stuff
		game_data->past_time = 0.0;
	}

	game_data->var += delta_time;
	game_data->past_time += delta_time;
	char buf[64] = "";
	sprintf(buf, "Delta = %.4f", game_data->past_time);
	DrawString(game_data->font, 0, 0, buf, 0xffffffff);
}

int main() {
	window_t window = { 0 };
	GameData data;
	CreateGLWindow("STG game", 800, 600, false, &window);
	Initialize(&data);
	RunMainLoop(&window, &data, GameMain);
	DestroyGLWindow(&window);
}
