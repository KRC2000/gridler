#include <cglm/cglm.h>

#include "raylib.h"
#include "gridler.h"

int main() {
	uint screenW = 800;
	uint screenH = 800;
	InitWindow(screenW, screenH, "Voxeld Ray-cast");

	size_t size = 10;

	vec2 rayStart = {size * 8 + 8, size * 4 + 3};
	vec2 rayEnd = {size * 1 + 9, size * 6 + 5};

	size_t intersectedCount = 0;
	float** intersected =
		GetLineIntersectedVoxels2D(size, rayStart, rayEnd, &intersectedCount);

	while (!WindowShouldClose()) {
		float tileSize = glm_min(screenW, screenH) / size;
		float ratio = tileSize / size;

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			for (size_t i = 0; i < intersectedCount; ++i)
				free(intersected[i]);
			free(intersected);
			intersectedCount = 0;
			glm_vec2_copy((vec2){GetMousePosition().x / ratio,
								 GetMousePosition().y / ratio},
						  rayStart);
			intersected = GetLineIntersectedVoxels2D(size, rayStart, rayEnd,
													 &intersectedCount);
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			for (size_t i = 0; i < intersectedCount; ++i)
				free(intersected[i]);
			free(intersected);
			intersectedCount = 0;
			glm_vec2_copy((vec2){GetMousePosition().x / ratio,
								 GetMousePosition().y / ratio},
						  rayEnd);
			intersected = GetLineIntersectedVoxels2D(size, rayStart, rayEnd,
													 &intersectedCount);
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (size_t x = 0; x < size; ++x) {
			for (size_t y = 0; y < size; ++y) {
				DrawLineEx((Vector2){rayStart[0] * ratio, rayStart[1] * ratio},
						   (Vector2){rayEnd[0] * ratio, rayEnd[1] * ratio}, 5,
						   RED);
				DrawCircle(rayStart[0] * ratio, rayStart[1] * ratio, 8, GREEN);
				DrawCircle(rayEnd[0] * ratio, rayEnd[1] * ratio, 8, YELLOW);
			}
		}

		for (size_t i = 0; i < intersectedCount; ++i) {
			DrawRectangleLinesEx(
				(Rectangle){intersected[i][0] * tileSize,
							intersected[i][1] * tileSize, tileSize, tileSize},
				1, GRAY);
		}

		EndDrawing();
	}

	return 0;
}
