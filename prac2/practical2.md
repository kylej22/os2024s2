Member 1: Memory Management Unit Initialization and Data Structures

Implement createMMU():
Initialize data structures for the page table and frame management.
Allocate space for the specified number of frames.
Track which pages are loaded into memory.
Unit Tests: Create tests to ensure that the MMU is initialized correctly and that data structures are functioning as expected.

Member 2: Page Residency Check and Frame Allocation

Implement checkInMemory():
Search the page table to determine if a page is in memory and return its frame number.
Return -1 if the page is not in memory.
Implement allocateFrame():
Allocate a frame for a page and update the page table.
Handle cases where all frames are full (defer to selectVictim()).
Unit Tests: Ensure that the page residency check works correctly and that frames are allocated as expected.

Member 3: Page Replacement Algorithms

Implement selectVictim():
Develop logic for Random, LRU, and Clock page replacement algorithms.
Integrate with the allocateFrame() function to handle page evictions and replacements.
Unit Tests: Test each algorithm to ensure that the correct page is selected for eviction under different scenarios.