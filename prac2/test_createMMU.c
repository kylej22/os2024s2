#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
        int pageNo;
        int modified;
} page;

typedef struct {
		int pageNo;
		int modified;
		int lastUsed;
} frame;

frame *frames;
int *pageTable;
int     createMMU( int);
const   int pageoffset = 12;            /* Page size is fixed to 4 KB */
int     numFrames ;

/* Creates the page table structure to record memory allocation */
int createMMU(int numFrames) {
    // Allocate memory and assign it to the global frames pointer
    frames = (frame *)malloc(numFrames * sizeof(frame));  // Assign directly to the global frames
    if (frames == NULL) {
        return -1; // Return error code if memory allocation fails
    }
    
    // Initialize each frame
    for (int i = 0; i < numFrames; i++) {
        frames[i].pageNo = -1;
        frames[i].modified = 0;
        frames[i].lastUsed = 0;
    }

    return 0;
}



int main() {
    int result;
    
    // Test 1: Create MMU with 5 frames
    numFrames = 5;  // Set global variable
    result = createMMU(numFrames);
    assert(result == 0);  // Ensure createMMU succeeded
    assert(frames != NULL);  // Ensure memory was allocated

    // Check if frames are initialized correctly
    for (int i = 0; i < numFrames; i++) {
        assert(frames[i].pageNo == -1);
        assert(frames[i].modified == 0);
        assert(frames[i].lastUsed == 0);
        printf("Frame %d - pageNo: %d, modified: %d, lastUsed: %d\n", 
               i, frames[i].pageNo, frames[i].modified, frames[i].lastUsed);
    }

    // Test 2: Create MMU with 1 frame
    numFrames = 1;
    result = createMMU(numFrames);
    assert(result == 0);  // Ensure createMMU succeeded
    assert(frames != NULL);  // Ensure memory was allocated

    // Check initialization
    assert(frames[0].pageNo == -1);
    assert(frames[0].modified == 0);
    assert(frames[0].lastUsed == 0);

    printf("Single frame - pageNo: %d, modified: %d, lastUsed: %d\n", 
           frames[0].pageNo, frames[0].modified, frames[0].lastUsed);

    // Test 3: Edge case with 0 frames (optional, depending on how your system should handle this)
    numFrames = 0;
    result = createMMU(numFrames);
    assert(result == 0);  // Ensure createMMU succeeded even with 0 frames

    // Test 4: Simulate memory allocation failure (harder to test without modifying malloc)
    // You could mock malloc or force it to fail, but this requires more setup
    // For now, let's assume you cannot do this easily, and skip this part.

    printf("All tests passed!\n");

    free(frames);  // Clean up memory
    return 0;
}
