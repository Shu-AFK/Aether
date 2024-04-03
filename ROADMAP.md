# Kernel Development Roadmap

The development roadmap focuses on three main areas: improving memory management,
transitioning to a more advanced filesystem, and implementing a graphical user
interface (GUI).

## 1. Memory Managment Imporvements

The current memory management implementation is very basic, which is why I want
to improve it in the future.

### Objectives:
- **Implement a More Efficient malloc and Memory Allocation Mechanism:** Enhance the
current memory allocation system to be more efficient and reduce fragmentation.
This includes implementing a better malloc, free, and possibly realloc functions
that can handle memory more gracefully.

- **Introduce Slab Allocation:** For kernel objects of fixed sizes, implement
slab allocation to improve memory usage efficiency and speed up memory allocation and deallocation.

## 2. Transition to a More Advanced Filesystem
The current file system is FAT16 but it has a lot of downsides, which is why
I want to switch to a better one in the future.

### Objectives:
- **Evaluate Potential Filesystem Candidates:** Consider filesystems like FAT32,
ext2, or a custom filesystem. Each has its benefits, with FAT32 providing a
straightforward upgrade path from FAT16, and ext2 offering features more
aligned with UNIX-like systems.

- **Implement Long File Name Support:** Regardless of the chosen filesystem, ensure
support for long file names, improving upon the limitations of the FAT16 filesystem.

## 3. Graphical User Interface (GUI)

The current kernel is terminal based but it would be cool to have a UI in the future.

### Objectives:
- **Frame Buffer Management:** Implement a basic frame buffer management system for
drawing pixels to the screen, laying the groundwork for more complex GUI elements.

- **Windowing System:** Develop a windowing system that supports moving, resizing,
and interacting with multiple windows. This includes managing window states and
rendering window contents efficiently.

- **Input Handling:** Integrate keyboard and mouse input, allowing for user
interaction with the GUI. This involves managing input devices at a low level
and translating input events into actions within the GUI.

- **Basic GUI Applications:** Develop basic GUI applications such as a
file explorer, text editor, and terminal emulator. These applications will
serve as both useful tools and examples for further application development.




