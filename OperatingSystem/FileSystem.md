# What is a File System?

- A **file system** is how an OS organizes and manages data on storage (disk, SSD, flash).
- It provides:
  - **File abstraction** → hides raw blocks, shows you named files.
  - **Directory hierarchy** → lets you organize files.
  - **Access control** → who can read/write.
  - **Metadata** → info like size, timestamps, permissions.

# File System Layers

Think of it as **stacked layers**:

1. **User-level API** (open, read, write, close in C).
2. **Virtual File System (VFS)** – abstraction layer that allows different file systems (ext4, NTFS, FAT) to work under one interface.
3. **Concrete FS implementation** (ext4 driver, FAT32 driver, etc.).
4. **Block layer** – communicates with storage device (e.g., HDD, SSD).
5. **Disk hardware**.

# Key Concepts

- **Inode (index node)**: metadata about file (size, owner, timestamps, data block pointers).
- **Directory**: mapping from file name → inode.
- **Superblock**: metadata about the whole FS (size, block size, free space, etc.).
- **Block**: smallest unit of storage FS allocates (e.g., 4 KB).

**Example**:

```c++
int fd = open("test.txt", O_CREAT | O_WRONLY, 0644);
write(fd, "hello", 5);
close(fd);
```

`open` → VFS locates inode of `test.txt`.

`write` → FS allocates a data block and updates inode.

`close` → flushes changes.

# **Why both filename and inode?**

- **Filename** = human-friendly label (e.g., `report.doc`).
- **Inode** = actual metadata + storage mapping (file size, permissions, data block addresses).

**Efficiency**: inode numbers are fixed-size integers → much faster to look up than long variable-length filenames.

**Flexibility**: multiple filenames (hard links) can point to the same inode. That’s impossible if metadata lived inside the filename.

**Clean design**: directory = simple table: filename → inode number. The inode handles the “real” data.

# Benefit of a Virtual File System (VFS) layer

**Abstraction**: Programs use the same syscalls (`open`, `read`, `write`) regardless of FS type.

**Interoperability**: Linux can mount FAT32 (USB), ext4 (root), NTFS (Windows partition) all at once under `/`.

**Extensibility**: Add a new file system (e.g., XFS, Btrfs) without rewriting user apps.

**Uniform namespace**: `/home/user/data` may live on ext4, while `/mnt/usb` is FAT32, but the path structure looks unified.

# **Common File System Syscalls**

- `open()` → open/create a file, returns a **file descriptor** (FD = small integer index).
- `read()` / `write()` → read/write bytes using FD.
- `lseek()` → move the “cursor” (offset) inside a file.
- `close()` → release the FD.
- `stat()` → query metadata (inode info).
- `unlink()` → delete a file (removes filename → inode link).

```c++
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd = open("test.txt", O_CREAT | O_WRONLY, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *msg = "Hello File System!\n";
    write(fd, msg, strlen(msg));   // write data
    lseek(fd, 0, SEEK_SET);        // move back to start
    write(fd, "X", 1);             // overwrite first byte with 'X'

    close(fd); // release file descriptor
    return 0;
}
```

# **Why File Descriptors (FDs)?**

Integer handles → efficient indexing into kernel’s FD table.

Uniform abstraction: sockets, pipes, devices also use FDs.

Multiple processes can share FDs (after `fork()` or `dup()`).

`dup2(oldfd, newfd)` → duplicates an existing FD (`oldfd`) onto another FD (`newfd`).

If `newfd` is already open, it is closed first.

Most importantly: **FD 1 = `stdout`, FD 2 = `stderr`, FD 0 = `stdin`**.

So if we `dup2(file_fd, 1)`, all `printf()` or `puts()` will go into the file.

```c++
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Redirect stdout (fd=1) to the file
    if (dup2(fd, 1) < 0) {
        perror("dup2");
        return 1;
    }
    close(fd); // not needed anymore, stdout now points to file

    printf("This will not show on terminal!\n");
    printf("It will go inside output.txt\n");

    return 0;
}
```

