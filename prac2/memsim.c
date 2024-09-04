#include <stdio.h>
#include <string.h>

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
enum repl { random, fifo, lru, clock };
int createMMU(int);
int checkInMemory(int);
int allocateFrame(int);
page selectVictim(int, enum repl);
const int pageoffset = 12; /* Page size is fixed to 4 KB */
int numFrames;

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

  return result;
}

/* allocate page to the next free frame and record where it put it */
int allocateFrame(int page_number) {
  // to do
  return;
}

/* Selects a victim for eviction/discard according to the replacement algorithm,
 * returns chosen frame_no  */
page selectVictim(int page_number, enum repl mode) {
  page victim;
  // to do

  // random algorithm
  if (mode == random) {
    int random = rand() % numFrames;
    victim.pageNo = frames[random].pageNo;
    victim.modified = frames[random].modified;
    frames[random].pageNo = page_number;
    frames[random].modified = 0;
    return (victim);
  }
  // least recently used algorithm
  if (mode == lru) {
    int min = frames[0].lastUsed;
    int index = 0;
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
    return (victim);
  }
  // clock algorithm
  if (mode == clock) {
    // assume clock hand is at 0
    int i = 0;
    while (1) {
      if (frames[i].lastUsed == 0) {
        victim.pageNo = frames[i].pageNo;
        victim.modified = frames[i].modified;
        frames[i].pageNo = page_number;
        frames[i].modified = 0;
        return (victim);
      }
      frames[i].lastUsed = 0;
      i = (i + 1) % numFrames;
    }
  }
  // fifo algorithm
  if (mode == fifo) {
    int min = frames[0].lastUsed;
    int index = 0;
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
    return (victim);
  }
  victim.pageNo = 0;
  victim.modified = 0;
  return (victim);
}

main(int argc, char *argv[]) {
  char *tracename;
  int page_number, frame_no, done;
  int do_line, i;
  int no_events, disk_writes, disk_reads;
  int debugmode;
  enum repl replace;
  int allocated = 0;
  int victim_page;
  unsigned address;
  char rw;
  page Pvictim;
  FILE *trace;

  if (argc < 5) {
    printf(
        "Usage: ./memsim inputfile numberframes replacementmode debugmode \n");
    exit(-1);
  } else {
    tracename = argv[1];
    trace = fopen(tracename, "r");
    if (trace == NULL) {
      printf("Cannot open trace file %s \n", tracename);
      exit(-1);
    }
    numFrames = atoi(argv[2]);
    if (numFrames < 1) {
      printf("Frame number must be at least 1\n");
      exit(-1);
    }
    if (strcmp(argv[3], "lru\0") == 0)
      replace = lru;
    else if (strcmp(argv[3], "rand\0") == 0)
      replace = random;
    else if (strcmp(argv[3], "clock\0") == 0)
      replace = clock;
    else if (strcmp(argv[3], "fifo\0") == 0)
      replace = fifo;
    else {
      printf("Replacement algorithm must be rand/fifo/lru/clock  \n");
      exit(-1);
    }

    if (strcmp(argv[4], "quiet\0") == 0)
      debugmode = 0;
    else if (strcmp(argv[4], "debug\0") == 0)
      debugmode = 1;
    else {
      printf("Replacement algorithm must be quiet/debug  \n");
      exit(-1);
    }
  }

  done = createMMU(numFrames);
  if (done == -1) {
    printf("Cannot create MMU");
    exit(-1);
  }
  no_events = 0;
  disk_writes = 0;
  disk_reads = 0;

  do_line = fscanf(trace, "%x %c", &address, &rw);
  while (do_line == 2) {
    page_number = address >> pageoffset;
    frame_no = checkInMemory(page_number); /* ask for physical address */

    if (frame_no == -1) {
      disk_reads++; /* Page fault, need to load it into memory */
      if (debugmode) printf("Page fault %8d \n", page_number);
      if (allocated < numFrames) /* allocate it to an empty frame */
      {
        frame_no = allocateFrame(page_number);
        allocated++;
      } else {
        Pvictim = selectVictim(page_number,
                               replace); /* returns page number of the victim */
        frame_no = checkInMemory(
            page_number);     /* find out the frame the new page is in */
        if (Pvictim.modified) /* need to know victim page and modified  */
        {
          disk_writes++;
          if (debugmode) printf("Disk write %8d \n", Pvictim.pageNo);
        } else if (debugmode)
          printf("Discard    %8d \n", Pvictim.pageNo);
      }
    }
    if (rw == 'R') {
      if (debugmode) printf("reading    %8d \n", page_number);
    } else if (rw == 'W') {
      // mark page in page table as written - modified
      if (debugmode) printf("writting   %8d \n", page_number);
    } else {
      printf("Badly formatted file. Error on line %d\n", no_events + 1);
      exit(-1);
    }

    no_events++;
    do_line = fscanf(trace, "%x %c", &address, &rw);
  }

  printf("total memory frames:  %d\n", numFrames);
  printf("events in trace:      %d\n", no_events);
  printf("total disk reads:     %d\n", disk_reads);
  printf("total disk writes:    %d\n", disk_writes);
  printf("page fault rate:      %.4f\n", (float)disk_reads / no_events);
}
