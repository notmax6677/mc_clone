#include <CGLM/cglm.h>
#include <CGLM/vec2.h>

#include "headers/chunk.h"
#include "headers/camera.h"

#include <stdbool.h>
#include <stdio.h>

// thank you https://www.geeksforgeeks.org/bubble-sort-algorithm/

void swap(float* xp, float* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
// An optimized version of Bubble Sort
void bubbleSort(vec2 arr[], int n)
{
    int i, j;
    bool swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = false;
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[0][j], &arr[0][j + 1]);
                swapped = true;
            }
        }

        // If no two elements were swapped by inner loop,
        // then break
        if (swapped == false)
            break;
    }
}


// ---


vec2 sortLastChunkPos = GLM_VEC2_ZERO_INIT;

int compareByPosition(const void* a, const void* b) {
	vec2 posA;
	glm_vec2_copy(((struct Chunk*)a)->pos, posA);
	vec2 posB;
	glm_vec2_copy(((struct Chunk*)a)->pos, posB);

	//return ((struct Chunk*)a)->pos[0] - ((struct Chunk*)b)->pos[0];

	return 
			(sqrt( (sortLastChunkPos[0] - posB[0])*(sortLastChunkPos[0] - posB[0])
				+ ( (sortLastChunkPos[1] - posB[1])*(sortLastChunkPos[1] - posB[1]))))

			- (sqrt( (sortLastChunkPos[0] - posA[0])*(sortLastChunkPos[0] - posA[0])
				+ ( (sortLastChunkPos[1] - posA[1])*(sortLastChunkPos[1] - posA[1]))));
}



// sorts chunks from back to front based on distance from camera
void sort_chunks(struct Chunk* chunks, int world_size) {
	
	// fetch camera position
	vec3* camPos = get_camera_pos();

	// snap player position to chunk grid and assign data to vec2 object
	vec2 playerChunkPos;
	glm_vec2_copy(
			(vec2) { floor((*camPos)[0] / 16), floor((*camPos)[2] / 16) }, 
			playerChunkPos);

	// if moved to another chunk
	if(playerChunkPos[0] != sortLastChunkPos[0] || playerChunkPos[1] != sortLastChunkPos[1]) {

		printf("%f, %f\n%f, %f\n", playerChunkPos[0], playerChunkPos[1], sortLastChunkPos[0], sortLastChunkPos[1]);


		qsort(chunks, world_size*world_size, sizeof(struct Chunk), compareByPosition);

	}
	



	// at end of function, copy over player chunk position to lastChunkPos for next frame
	glm_vec2_copy(playerChunkPos, sortLastChunkPos);

}
