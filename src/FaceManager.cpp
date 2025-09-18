#include "FaceManager.hpp"
#include <string.h>

void InitFaceManager(face_manager_t* face_manager) {
	memset(face_manager, 0, sizeof(face_manager_t));
	InitSingleList(&face_manager->face_list, MAX_FACES);
	CreateTL2DVertexBuffer(4 * MAX_FACES, nullptr, GL_DYNAMIC_DRAW, &face_manager->vertex_buffer, &face_manager->vertex_array);
}

void DestroyFaceManager(face_manager_t* face_manager) {
	glDeleteBuffers(1, &face_manager->vertex_buffer);
	glDeleteVertexArrays(1, &face_manager->vertex_array);
	DestroySingleList(&face_manager->face_list);
}

face_t* AddFace(face_manager_t* face_manager, float x, float y, float angle, GLuint texture) {
	if (face_manager->count > MAX_FACES) return 0;
	const int idx = face_manager->count;
	InitFace(face_manager->faces + idx, x, y, angle, texture);
	SLPushItem(&face_manager->face_list, face_manager->faces + idx);
	face_manager->draw_list[idx] = face_manager->faces + idx;
	face_manager->count++;
	return face_manager->faces + idx;
}


void DrawFaces(face_manager_t* face_manager) {
	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(face_manager->vertex_buffer, GL_WRITE_ONLY);
	node_t* node = face_manager->face_list.head;
	int cnt = face_manager->count;
	for (int i = 0; i < cnt; i++) {
		face_t* face = face_manager->draw_list[i];
		SetupSprite(vertex, face->x, face->y, &face->sprite);
		vertex += 4;
	}
	glUnmapNamedBuffer(face_manager->vertex_buffer);

	SLReset(&face_manager->face_list);
	glBindVertexArray(face_manager->vertex_array);
	for (int i = 0; i < cnt; i++) {
		glBindTextureUnit(0, face_manager->draw_list[i]->sprite.texture);
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
}