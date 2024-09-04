#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
enum repl { randomAlg, fifo, lru, clockAlg };
int createMMU(int);
int checkInMemory(int);
int allocateFrame(int);
page selectVictim(int, enum repl);
const int pageoffset = 12; /* Page size is fixed to 4 KB */
int numFrames;
int fifoIndex = 0;
int clockHand = 0;

/* Creates the page table structure to record memory allocation */
int createMMU(int numFrames) {
  // to do
  frames = (frame *)malloc(numFrames * sizeof(frame));
  if (frames == NULL) {
    return -1;
  }

  for (int i = 0; i < numFrames; i++) {
    frames[i].pageNo = -1;
    frames[i].modified = 0;
    frames[i].lastUsed = 0;
  }

  return 0;
}

/* Checks for residency: returns frame no or -1 if not found */
int checkInMemory(int page_number) {
  int result = -1;

  // to do
  for (int i = 0; i < numFrames; i++) {
    if (frames[i].pageNo == page_number) {
      result = i;
      break;
    }
  }

  return result;
}

/* allocate page to the next free frame and record where it put it */
int allocateFrame(int page_number) {
  int frameIndex = -1;  // Initialize frameIndex to -1 (no free frame)

  // to do
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

/* Selects a victim for eviction/discard according to the replacement algorithm,
 * returns chosen frame_no  */
page selectVictim(int page_number, enum repl mode) {
  page victim;
  // random algorithm
  if (mode == randomAlg) {
    int randomIndex = rand() % numFrames;
    victim.pageNo = frames[randomIndex].pageNo;
    victim.modified = frames[randomIndex].modified;
    frames[randomIndex].pageNo = page_number;
    frames[randomIndex].modified = 0;
    return (victim);
    }

  // least recently used algorithm
  if (mode == lru) {
    int min = frames[0].lastUsed;
    int index = 0;

    // find the least recently used frame
    for (int i = 1; i < numFrames; i++) {
      if (frames[i].lastUsed < min) {
        min = frames[i].lastUsed;
        index = i;
      }
    }
    victim.pageNo = frames[index].pageNo;
    victim.modified = frames[index].modified;
    frames[index].pageNo = page_number;
    frames[index].modified = 0;
    // update last used for new frame
    frames[index].lastUsed = 0;

    return (victim);
  }
  // clock algorithm
  if (mode == clockAlg) {
    while (1) {
      if (frames[clockHand].lastUsed == 0) {
        victim.pageNo = frames[clockHand].pageNo;
        victim.modified = frames[clockHand].modified;
        frames[clockHand].pageNo = page_number;
        frames[clockHand].modified = 0;
        clockHand = (clockHand + 1) % numFrames;
        return (victim);
      } else {
        frames[clockHand].lastUsed = 0;
        clockHand = (clockHand + 1) % numFrames;
      }
    }
  }
  // fifo algorithm
  if (mode == fifo) {
    victim.pageNo = frames[fifoIndex].pageNo;
    victim.modified = frames[fifoIndex].modified;
    frames[fifoIndex].pageNo = page_number;
    frames[fifoIndex].modified = 0;
    fifoIndex = (fifoIndex + 1) % numFrames;
    return (victim);
  }

  victim.pageNo = 0;
  victim.modified = 0;
  return (victim);
}

int main() {
  srand(time(NULL));
  // Create MMU with 4 frames
  numFrames = 4;
  createMMU(numFrames);

  // Allocate some pages to frames
  allocateFrame(1);
  allocateFrame(2);
  allocateFrame(3);
  allocateFrame(4);

  // Test selectVictim function with different replacement modes
  int page_number = 5;

  enum repl mode = randomAlg;
  page victim = selectVictim(page_number, mode);
  // random alg, expected output is random
  printf("Victim page number: %d\n", victim.pageNo);

  mode = lru;
  victim = selectVictim(page_number, mode);
  printf("Victim page number: %d\n", victim.pageNo);

  mode = clockAlg;
  victim = selectVictim(page_number, mode);
  printf("Victim page number: %d\n", victim.pageNo);

  mode = fifo;
  victim = selectVictim(page_number, mode);
  printf("Victim page number: %d\n", victim.pageNo);

  return 0;
}
