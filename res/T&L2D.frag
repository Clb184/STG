#version 450 core

// There isn't anything interesting in here
out vec4 COLOR;

in vec4 color;
in vec2 tex_coords;

uniform sampler2D g_Sampler;

void main() {
	COLOR = texture(g_Sampler, tex_coords) * color;
}