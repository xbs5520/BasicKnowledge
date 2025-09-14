# Virtual Memory Basics

Virtual memory is an abstraction that makes each process *think* it has a big, continuous block of memory — while in reality, memory is split into small chunks and shared among processes.

- **Page**: A fixed-size block of virtual memory (e.g., 4 KB).
- **Frame**: A fixed-size block of physical memory (RAM).
- The OS + MMU (Memory Management Unit) map **pages → frames**.

This mapping is stored in the **page table**.

## Key Terms

**TLB (Translation Lookaside Buffer)**:

- A tiny, super-fast cache inside the CPU.
- Stores recent page → frame translations.
- If the translation is not in TLB → *TLB miss* → must check page table in memory.

**Page Fault**:

- Happens when the requested page is not in RAM.
- OS must bring it from disk (slow).
- Could require swapping out another page (if RAM is full).

##  Why Virtual Memory?

- **Isolation**: Each process has its own space (can’t corrupt others).
- **Convenience**: Programs can use large addresses even if physical RAM is small.
- **Efficiency**: Multiple processes can share code/data (e.g., libc).

## Simple Example

Imagine:

- Virtual address space: 16 KB
- Page size: 4 KB
- Physical RAM: 8 KB

Then:

- 16 KB / 4 KB = 4 virtual pages (P0–P3).
- 8 KB / 4 KB = 2 physical frames (F0–F1).

If process wants P0 and P1 → they map to F0 and F1.
 If later it needs P2, but RAM is full → OS swaps something out → **page replacement**.

