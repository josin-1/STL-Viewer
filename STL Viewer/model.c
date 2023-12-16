#pragma warning(disable:4996)

#include "model.h"

Vertex3 model_calcCenter(const Model* const model) {
	/*
		Calculate the center point of a 3D object.
		Sum up the X, Y and Z parts of each node, then divide it through the amount of elements that were summed up
	*/

	Vertex3 center = { 0 };
	float sumX = 0, sumY = 0, sumZ = 0;

	for (size_t i = 0; i < model->num_polygons; ++i) {
		for (size_t j = 0; j < 3; ++j) {
			sumX += model->polygons[i].vertices[j].x;
			sumY += model->polygons[i].vertices[j].y;
			sumZ += model->polygons[i].vertices[j].z;
		}
	}
	center.x = sumX / (float)(model->num_polygons * 3u);
	center.y = sumY / (float)(model->num_polygons * 3u);
	center.z = sumZ / (float)(model->num_polygons * 3u);

	return center;
}

void model_shift(const Model* model, const Vertex3 shiftVertex) {
	/*
		Shift the 3D object in respect to a Vertex.
		X, Y and Z components subtracted with the shift Vertex components
	*/

	for (size_t i = 0; i < model->num_polygons; ++i) {
		for (size_t j = 0; j < 3; ++j) {
			model->polygons[i].vertices[j].x -= shiftVertex.x;
			model->polygons[i].vertices[j].y -= shiftVertex.y;
			model->polygons[i].vertices[j].z -= shiftVertex.z;
		}
	}
}

Model* model_load(const char* const filename) {
	/*
		Load a Binary STL File (ASCII not supported yet) and save it into a Model Structure.
		
		A Binary STL FILE is structured like this:
			Header (ASCII Chars) - 80 Bytes
			number of Polygons (Triangles) - uint32

			for each triangle:
				Normal Vertex :
					x -> float (REAL32) - 32 bit;
					y -> float (REAL32) - 32 bit;
					z -> float (REAL32) - 32 bit;
				Vertex 1: 
					x -> float (REAL32) - 32 bit;
					y -> float (REAL32) - 32 bit;
					z -> float (REAL32) - 32 bit;
				Vertex 2:
					x -> float (REAL32) - 32 bit;
					y -> float (REAL32) - 32 bit;
					z -> float (REAL32) - 32 bit;
				Vertex 3:
					x -> float (REAL32) - 32 bit;
					y -> float (REAL32) - 32 bit;
					z -> float (REAL32) - 32 bit;
				Attribute - uint16

		At the end it also shifts the Model in respect to its center
	*/

	FILE* fp = NULL;
	Model* model = NULL;

	size_t ret;
	char header[80];
	uint16_t attr;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		SDL_Log("ERROR: Opening File: %s", SDL_GetError());
		return NULL;
	}
	model = malloc(sizeof(Model));
	if (model == NULL) {
		SDL_Log("ERROR: malloc(): %s", SDL_GetError());
		return NULL;
	}

	model->sideLength = MODEL_SIDE_LENGTH;

	ret = fread(header, 80, 1, fp);
	if (ret != 1) {
		SDL_Log("ERROR: Loading File: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	ret = fread(&model->num_polygons, 4, 1, fp);
	if (ret != 1) {
		SDL_Log("ERROR: Loading File: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	model->polygons = calloc(sizeof(Polygon), model->num_polygons);
	if (model->polygons == NULL) {
		SDL_Log("ERROR: malloc(): %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < model->num_polygons; ++i) {

		model->polygons[i].enabled = 1;

		ret = fread(&(model->polygons[i].normal), sizeof(Vertex3), 1, fp);
		if (ret != 1) {
			SDL_Log("ERROR: Loading File: %s", SDL_GetError());
			return NULL;
		}

		ret = fread(&(model->polygons[i].vertices), sizeof(Vertex3) * 3, 1, fp);
		if (ret != 1) {
			SDL_Log("ERROR: Loading File: %s", SDL_GetError());
			return NULL;
		}

		ret = fread(&attr, sizeof(attr), 1, fp);
		if (ret != 1) {
			SDL_Log("ERROR: Loading File: %s", SDL_GetError());
			return NULL;
		}
	}

	fclose(fp);

	model_shift(model, model_calcCenter(model));

	return model;
}

Quaternion model_quat_rotationQuaternion(const float angleX, const float angleY, const float angleZ) {
	/*
		Calculate the rotation Quaternion.
		
		First it calculates the Axis Angle represantion from the input angles,
		then it calculates the Quaternion with these and returns it
	*/
	

	// Euler Angles to Axis Angle represantion
	// copied and adapted from https://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToEuler/index.htm

	float axisAngleX = 0, axisAngleY = 0, axisAngleZ = 0, axisAngleTheta = 0;

	// Assuming the angles are in radians.
	float c1 = (float)cos((double)angleY / 2);
	float s1 = (float)sin((double)angleY / 2);
	float c2 = (float)cos((double)angleZ / 2);
	float s2 = (float)sin((double)angleZ / 2);
	float c3 = (float)cos((double)angleX / 2);
	float s3 = (float)sin((double)angleX / 2);
	float c1c2 = c1 * c2;
	float s1s2 = s1 * s2;
	float w = c1c2 * c3 - s1s2 * s3;
	axisAngleX = c1c2 * s3 + s1s2 * c3;
	axisAngleY = s1 * c2 * c3 + c1 * s2 * s3;
	axisAngleZ = c1 * s2 * c3 - s1 * c2 * s3;
	axisAngleTheta = 2.0f * (float)acos((double)w);
	float norm = axisAngleX * axisAngleX + axisAngleY * axisAngleY + axisAngleZ * axisAngleZ;
	if (norm < 0.00001f) { // when all euler angles are zero angle =0 so
		// we can set axis to anything to avoid divide by zero
		axisAngleX = 1;
		axisAngleY = 0;
		axisAngleY = axisAngleZ = 0;
	}
	else {
		norm = (float)sqrt((double)norm);
		axisAngleX /= norm;
		axisAngleY /= norm;
		axisAngleZ /= norm;
	}

	Quaternion rotationQuat = { 0 };

	float sinThetaHalf = (float)sin((double)axisAngleTheta / 2.0f);
	float cosThetaHalf = (float)cos((double)axisAngleTheta / 2.0f);

	rotationQuat.real = cosThetaHalf;
	rotationQuat.i = axisAngleX * sinThetaHalf;
	rotationQuat.j = axisAngleY * sinThetaHalf;
	rotationQuat.k = axisAngleZ * sinThetaHalf;

	return rotationQuat;
}

void model_quat_rotation(float* x, float* y, float* z, const Quaternion rotationQuat) {
	/*
		Rotate the 3D object with the rotation Quaternion.
		
		It is based on 2 Quaternion Multiplications:
			inverse_rotationQuaternion * vertexQuaternion * rotationQuaternion
		A vertex is converted to a Quaternion as follows:
			real = 0
			x = i
			y = j
			z = k
		The inverse Quaternion is:
			real = real
			i = -i
			j = -j
			k = -k
		Because the real component of the Vertex Quaternion equals 0, a few multiplications dont have to be calculated
	*/

	Quaternion product = { 0 };
	product.real = -(-rotationQuat.i) * *x - (-rotationQuat.j) * *y - (-rotationQuat.k) * *z;
	product.i = rotationQuat.real * *x - (-rotationQuat.j) * *z + (-rotationQuat.k) * *y;
	product.j = rotationQuat.real * *y + (-rotationQuat.i) * *z - (-rotationQuat.k) * *x;
	product.k = rotationQuat.real * *z - (-rotationQuat.i) * *y + (-rotationQuat.j) * *x;

	*x = product.real * rotationQuat.i + product.i * rotationQuat.real - product.j * rotationQuat.k + product.k * rotationQuat.j;
	*y = product.real * rotationQuat.j + product.i * rotationQuat.k + product.j * rotationQuat.real - product.k * rotationQuat.i;
	*z = product.real * rotationQuat.k - product.i * rotationQuat.j + product.j * rotationQuat.i + product.k * rotationQuat.real;

}

void model_rotate(const Model* model, const float angleX, const float angleY, const float angleZ) {
	/*
		Loop through the whole Model and rotate each Vertex.
	*/

	Quaternion rotationQuat = model_quat_rotationQuaternion(angleX, angleY, angleZ);

	for (size_t i = 0; i < model->num_polygons; ++i) {
		model_quat_rotation(&model->polygons[i].normal.x, &model->polygons[i].normal.y, &model->polygons[i].normal.z, rotationQuat);
		for (size_t j = 0; j < 3; ++j) {
			model_quat_rotation(&model->polygons[i].vertices[j].x, &model->polygons[i].vertices[j].y, &model->polygons[i].vertices[j].z, rotationQuat);
		}
	}
}

int model_draw(SDL_Window* mainWindow, SDL_Renderer* renderer, const Model* model) {
	/*
		Draw the 3D-Model using SDL_RenderGeometry.

		First the Shader and Sorting Algo is put on the Model.
		Then the activated Polygons (dont need to print Polygons, that look away) are put into new SDL_Vertex array.
		A little calculation is done to shift the Vertices in respect to the Window Size and the Side Length of the Model (Zoom)
	*/

	model_shader(model);
	model_sort(model);

	SDL_Vertex* renderVertices = NULL;
	size_t k = 0, num_vertices = 0;
	int32_t status = 0, windowWidth = 0, windowHeight = 0;
	
	for (size_t i = 0; i < model->num_polygons; ++i) {
		if (model->polygons[i].enabled) ++num_vertices;
	}
	num_vertices *= 3;

	SDL_GetWindowSize(mainWindow, &windowWidth, &windowHeight);

	status += SDL_SetRenderDrawColor(renderer, BACKGROUND_R, BACKGROUND_G, BACKGROUND_B, BACKGROUND_A);
	status += SDL_RenderClear(renderer);

	renderVertices = calloc(sizeof(SDL_Vertex), num_vertices);
	if (renderVertices == NULL) {
		SDL_Log("ERROR: calloc(): %s", SDL_GetError());
		return -1;
	}

	k = 0;
	for (size_t i = 0; i < model->num_polygons; ++i) {
		if (model->polygons[i].enabled) {
			for (size_t j = 0; j < 3; ++j) {
				renderVertices[k].color = model->polygons[i].color;

				renderVertices[k].position.x = (float)(windowWidth >> 1) + (model->polygons[i].vertices[j].x * ((int)model->sideLength >> 1));
				renderVertices[k].position.y = (float)(windowHeight >> 1) - (model->polygons[i].vertices[j].y * ((int)model->sideLength >> 1));
				++k;
			}
		}
	}
	status += SDL_RenderGeometry(renderer, NULL, renderVertices, (int)num_vertices, NULL, 0);
	free(renderVertices);
	return status;
}

void model_shader(const Model* model) {
	/*
		Calculate the Color for each Polygon in a shaded way.
		
		Polygon looks towards us -> white
		Polygon looks sideways -> black
		Polygon looks away -> deactivated (is used to avoid artifacts and speed up sorting algo)
	*/

	SDL_Color color = { 0 };

	for (size_t i = 0; i < model->num_polygons; ++i) {

		if (model->polygons[i].normal.z > -1.0f && model->polygons[i].normal.z < -0.0f) { // polygon looks away
			model->polygons[i].enabled = 0u;
		}

		else if (model->polygons[i].normal.z > 0.5f) { // polygon looks at front
			model->polygons[i].enabled = 1u;

			color.r = (Uint8)((510 - 510 * SOLID_R_MOD) * model->polygons[i].normal.z + (510 * SOLID_R_MOD - 255));
			color.g = (Uint8)((510 - 510 * SOLID_G_MOD) * model->polygons[i].normal.z + (510 * SOLID_G_MOD - 255));
			color.b = (Uint8)((510 - 510 * SOLID_B_MOD) * model->polygons[i].normal.z + (510 * SOLID_B_MOD - 255));
			color.a = (Uint8)(255 * SOLID_A_MOD);
			model->polygons[i].color = color;
		}
		else { // polygon is looking sideways
			model->polygons[i].enabled = 1u;

			color.r = (Uint8)(510 * SOLID_R_MOD * model->polygons[i].normal.z);
			color.g = (Uint8)(510 * SOLID_G_MOD * model->polygons[i].normal.z);
			color.b = (Uint8)(510 * SOLID_B_MOD * model->polygons[i].normal.z);
			color.a = (Uint8)(255 * SOLID_A_MOD);
			model->polygons[i].color = color;
		}
	}
}

int model_sort(const Model* model) {
	/*
		Sort all Polygons of the 3D-Model, from Front to Back.
		
		First a new Structure Array is filled with the median of the z-Value and the index of the Polygon.
		Then it is sorted using qsort (with the z median value as comparator).
		Afterwards a new temp Polygon Array is made to fill it with the sorted index.
		At last the temp Array is copied into the original Array.
	*/

	SortStruct* sortArr = NULL;
	Polygon* sortedPolygons = NULL;
	size_t original_index = 0, j = 0, num_activePolygons = 0;

	for (size_t i = 0; i < model->num_polygons; ++i) {
		if (model->polygons[i].enabled == 1u) ++num_activePolygons;
	}

	// Initialize an array of indices and z values
	sortArr = calloc(num_activePolygons, sizeof(SortStruct));
	if (sortArr == NULL) {
		SDL_Log("ERROR: calloc(): %s", SDL_GetError());
		return -1;
	}

	for (size_t i = 0; i < model->num_polygons; ++i) {
		if (model->polygons[i].enabled) {
			sortArr[j].indices = i;
			sortArr[j].z_median = (model->polygons[i].vertices[0].z + model->polygons[i].vertices[1].z + model->polygons[i].vertices[2].z) / 3.0f;
			++j;
		}
	}

	// Sort the indices based on the 'z' values
	qsort(sortArr, num_activePolygons, sizeof(SortStruct), model_sort_compare);

	// Create temporary arrays to hold the sorted values
	sortedPolygons = calloc(model->num_polygons, sizeof(Polygon));
	if (sortedPolygons == NULL) {
		SDL_Log("ERROR: calloc(): %s", SDL_GetError());
		return -1;
	}

	// Copy values to the temporary arrays based on sorted indices
	j = 0;
	for (size_t i = 0; i < model->num_polygons; ++i) {
		if (model->polygons[i].enabled) {
			original_index = sortArr[j].indices;
			sortedPolygons[j] = model->polygons[original_index];
			++j;
		}
	}
	for (size_t i = 0; j < model->num_polygons; ++i) {
		if (!model->polygons[i].enabled) {
			sortedPolygons[j] = model->polygons[i];
			++j;
		}
	}
	
	// Copy the sorted values back to the original arrays
	for (size_t i = 0; i < model->num_polygons; i++) {
		model->polygons[i] = sortedPolygons[i];
	}

	// Free temporary arrays
	free(sortArr);
	free(sortedPolygons);

	return 0;
}

int model_sort_compare(const SortStruct* const a, const SortStruct* const b) {
	// Custom comparison function for sorting based on the 'z' values
	return (a->z_median - b->z_median < 0) ? -1 : 1;
}