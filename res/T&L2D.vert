#version 450 core

// The most basic shader I'll do for 2D stuff
layout(location = 0) in vec2 POSITION;
layout(location = 1) in vec2 TEXCOORDS;
layout(location = 2) in vec4 COLOR;

layout(location = 0) uniform mat4 g_CameraMatrix; 

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};
out vec4 color;
out vec2 tex_coords;

const float div = 1.0f / 255.0f;

// Since it converts the colors already, I just need to transform vectors, something as quick as...
// well, I dunno lol
void main() {
	color = COLOR;
	tex_coords = TEXCOORDS;
	gl_Position = g_CameraMatrix * vec4(POSITION, 0.0f, 1.0f);
}