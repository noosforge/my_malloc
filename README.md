# my_malloc

A custom memory allocator written from scratch in C to understand how `malloc()` and `free()` work under the hood.

The goal is to build a small allocator step by step, starting with memory layout and block metadata, then implementing allocation, deallocation, block splitting, and coalescing.

## Goals

- Understand how dynamic memory allocation works internally.
- Manage memory inside a fixed-size arena.
- Implement block headers and footers.
- Track allocation state using packed metadata.
- Allocate memory with proper alignment.
- Split large free blocks to satisfy allocation requests.
- Free blocks and coalesce adjacent free blocks.
- Explore fragmentation, allocator invariants, and memory safety.

## Design

### Arena

The allocator manages a fixed-size arena of 1,024 bytes obtained from the system allocator.

Instead of requesting memory from the operating system for every allocation, the custom allocator manages blocks within this arena.

### Block layout

Each block contains a header, a payload, and a footer.

```text
+----------------+-------------------+----------------+
|     Header     |      Payload      |     Footer     |
+----------------+-------------------+----------------+
```

- **Header:** Stores the total block size and allocation flag.
- **Payload:** The memory available to the caller.
- **Footer:** Stores the total block size, helping locate the previous block.

The total block size includes the header, payload, and footer.

### Packed header metadata

The header stores the block size and allocation status in a single `size_t`.

The lowest bit indicates whether the block is allocated:

- `0` — free
- `1` — allocated

Because block sizes are aligned, the lowest bit can be used as a flag without losing size information.

### Allocation strategy

The allocator searches the arena for a suitable free block. When a block is larger than necessary, it can be split into an allocated block and a remaining free block, provided the remainder is large enough to form a valid block.

### Deallocation and coalescing

When memory is freed, the allocator marks the block as free. Adjacent free blocks can then be merged into a larger block to reduce external fragmentation.

These operations are being developed incrementally as part of the project.

## Building

The project uses Clang, Make, and the AddressSanitizer runtime for debugging.

Build the project:

```bash
make
```

Run the test target:

```bash
make test
```

Clean generated build files:

```bash
make clean
```

The Makefile enables compiler warnings and AddressSanitizer to help detect memory errors during development.

## Project structure

```text
my_malloc/
├── include/          # Shared header files
├── src/              # Allocator implementation
├── tests/            # Tests
├── build/            # Generated object files
├── Makefile
├── .gitignore
└── README.md
```

## Learning approach

This project is an exercise in understanding the mechanisms behind memory allocation rather than simply reproducing an existing implementation.

The allocator is developed incrementally, with attention to:

- Memory layout and pointer arithmetic
- Alignment and allocation sizes
- Metadata invariants
- Block splitting and coalescing
- Free-list management
- Fragmentation and allocator correctness
- Testing and debugging low-level C code

## Limitations

This is an educational allocator, not a production replacement for the standard C allocator.

The arena has a fixed capacity, and allocator features and safety checks are still under development.
