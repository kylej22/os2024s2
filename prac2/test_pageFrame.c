#include <stdio.h>
#include <stdlib.h>

/* Define the frame structure as in the main code */
typedef struct {
    int pageNo;
    int modified;
    int lastUsed;
} frame;

/* Global variables to simulate the MMU environment */
frame *frames;
int numFrames;

/* Function declarations */
int createMMU(int numFrames);
int checkInMemory(int page_number);
int allocateFrame(int page_number);

/* Function implementations */
int createMMU(int numFrames) {
    frames = (frame *)malloc(numFrames * sizeof(frame));
    if (frames == NULL) {
        return -1;
    }

    for (int i = 0; i < numFrames; i++) {
        frames[i].pageNo = -1;  // Initialize all frames as free
        frames[i].modified = 0;
        frames[i].lastUsed = 0;
    }

    return 0;
}

int checkInMemory(int page_number) {
    int result = -1; 

    // Loop through all frames to find the page number
    for (int i = 0; i < numFrames; i++) {
        if (frames[i].pageNo == page_number) {  
            result = i;  
            break;  
        }
    }

    return result;  
}

int allocateFrame(int page_number) {
    int frameIndex = -1;  

    // Loop through all frames to find a free one
    for (int i = 0; i < numFrames; i++) {
        if (frames[i].pageNo == -1) {  
            frames[i].pageNo = page_number;  
            frames[i].modified = 0;  
            frames[i].lastUsed = 0; 
            frameIndex = i;  
            break;  
        }
    }

    return frameIndex;  
}

/* Test code */
int main() {
    int result;

    numFrames = 3;  // Set number of frames for the test
    result = createMMU(numFrames);
    if (result == -1) {
        printf("Failed to create MMU.\n");
        return -1;
    } else {
        printf("MMU created with %d frames.\n", numFrames);
    }

    // Test cases

    printf("Test 1: Check when memory is empty\n");
    result = checkInMemory(5);
    printf("Expected: -1, Got: %d\n", result);

    printf("Test 2: Allocate first frame\n");
    result = allocateFrame(5);
    printf("Expected: 0, Got: %d\n", result);
    printf("Frame 0 pageNo: %d\n", frames[0].pageNo);

    printf("Test 3: Check newly allocated page\n");
    result = checkInMemory(5);
    printf("Expected: 0, Got: %d\n", result);

    printf("Test 4: Allocate another frame\n");
    result = allocateFrame(10);
    printf("Expected: 1, Got: %d\n", result);
    printf("Frame 1 pageNo: %d\n", frames[1].pageNo);

    printf("Test 5: Allocate until frames are full\n");
    result = allocateFrame(15);
    printf("Expected: 2, Got: %d\n", result);
    printf("Frame 2 pageNo: %d\n", frames[2].pageNo);

    printf("Test 6: Check non-existent page\n");
    result = checkInMemory(20);
    printf("Expected: -1, Got: %d\n", result);

    printf("Test 7: Check full memory allocation\n");
    result = allocateFrame(20);
    printf("Expected: -1 (as all frames are full), Got: %d\n", result);

    // Free allocated memory
    free(frames);

    return 0;
}