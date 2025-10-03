#include "EnemyManager.hpp"

void InitEnemyManager(enemy_manager_t* enm_manager) {
	// ZeroMem all and that stuff
	memset(enm_manager, 0, sizeof(enemy_manager_t));
	InitSingleList(&enm_manager->enemy_list);
	CreateTL2DVertexBuffer(4 * MAX_ENEMY, nullptr, GL_DYNAMIC_DRAW, &enm_manager->vertex_buffer, &enm_manager->vertex_array);
}

void DestroyEnemyManager(enemy_manager_t* enm_manager) {
	// Bye bye
	glDeleteBuffers(1, &enm_manager->vertex_buffer);
	glDeleteVertexArrays(1, &enm_manager->vertex_array);
	DestroySingleList(&enm_manager->enemy_list);
}

enemy_t* AddEnemy(enemy_manager_t* enm_manager, int hp, float x, float y, float dir, GLuint texture) {
	// Check if full
	if (enm_manager->count > MAX_ENEMY) return 0;

	// "Create" a new item
	node_t<enemy_t>* enm = SLPushItem(&enm_manager->enemy_list);
	if (nullptr != enm) {
		InitEnemy(&enm->data, hp, x, y, dir, texture);
		enm_manager->count++;
		return &enm->data;
	}
	return nullptr;
}

void DrawEnemies(enemy_manager_t* enm_manager) {
	// Acquire buffer
	TLVertex2D* vertex = (TLVertex2D*)glMapNamedBuffer(enm_manager->vertex_buffer, GL_WRITE_ONLY);

	// Iterate over and fill a texture id array
	int num_draw = 0;
	for (node_t<enemy_t>* node = enm_manager->enemy_list.head; node; node = node->next) {
		SetupSprite(vertex, node->data.x, node->data.y, &node->data.sprite);

		vertex += 4;

		enm_manager->tex_list[num_draw] = node->data.sprite.texture;
		num_draw++;
	}
	glUnmapNamedBuffer(enm_manager->vertex_buffer);

	// Draw each enemy
	glBindVertexArray(enm_manager->vertex_array);
	GLuint this_tex = -1;
	for (int i = 0; i < num_draw; i++) {
		if (this_tex != enm_manager->tex_list[i]) // Try to avoid binding same texture in a row
			glBindTextureUnit(0, enm_manager->tex_list[i]);

		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
}