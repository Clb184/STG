#include "FaceManager.hpp"
#include <string.h>

void InitFaceManager(face_manager_t* face_manager) {
	// ZeroMem all and that stuff
	memset(face_manager, 0, sizeof(face_manager_t));
	InitSingleList(&face_manager->face_list);
	CreateTL2DVertexBuffer(4 * MAX_FACES, nullptr, GL_DYNAMIC_DRAW, &face_manager->vertex_buffer, &face_manager->vertex_array);
}

void DestroyFaceManager(face_manager_t* face_manager) {
	// Bye bye
	glDeleteBuffers(1, &face_manager->vertex_buffer);
	glDeleteVertexArrays(1, &face_manager->vertex_array);
	DestroySingleList(&face_manager->face_list);
}

face_t* AddFace(face_manager_t* face_manager, float x, float y, float angle, GLuint texture) {
	// Check if full
	if (face_manager->count > MAX_FACES) return 0;

	// "Create" a new item
	node_t<face_t>* face = SLPushItem(&face_manager->face_list);
	if (nullptr != face) {
		InitFace(&face->data, x, y, angle, texture);
		face_manager->count++;
		return &face->data;
	}
	return nullptr;
}


void DrawFaces(face_manager_t* face_manager) {
	// Acquire buffer
	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(face_manager->vertex_buffer, GL_WRITE_ONLY);

	// Iterate over and fill a texture id array
	int num_draw = 0;
	for (node_t<face_t>* node = face_manager->face_list.head; node; node = node->next) {
		SetupSprite(vertex, node->data.x, node->data.y, &node->data.sprite);

		vertex += 4;

		face_manager->tex_list[num_draw] = node->data.sprite.texture;
		num_draw++;
	}
	glUnmapNamedBuffer(face_manager->vertex_buffer);

	// Draw each face
	glBindVertexArray(face_manager->vertex_array);
	GLuint this_tex = -1; 
	for (int i = 0; i < num_draw; i++)  {
		if(this_tex != face_manager->tex_list[i]) // Try to avoid binding same texture in a row
			glBindTextureUnit(0, face_manager->tex_list[i]);
		
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
}