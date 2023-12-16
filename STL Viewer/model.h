#ifndef modelH
#define modelH

#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "consts.h"

typedef struct {
	float x, y, z;
} Vertex3;

typedef struct {
	Vertex3 vertices[3];
	Vertex3 normal;
	SDL_Color color;
	uint8_t enabled;
} Polygon;

typedef struct {
	Polygon* polygons;
	uint32_t num_polygons;
	float sideLength;
} Model;

typedef struct {
	float real, i, j, k;
} Quaternion;

typedef struct {
	size_t indices;
	float z_median;
} SortStruct;

Vertex3 model_calcCenter(const Model* const);
void model_shift(const Model*, const Vertex3);
Model* model_load(const char* const);
Quaternion model_quat_rotationQuaternion(const float, const float, const float);
void model_quat_rotation(float*, float*, float*, const Quaternion);
void model_rotate(const Model*, const float, const float, const float);
int model_draw(SDL_Window*, SDL_Renderer*, const Model*);
void model_shader(const Model*);
int  model_sort(const Model*);
int model_sort_compare(const SortStruct* const, const SortStruct* const);

#endif // !modelH 