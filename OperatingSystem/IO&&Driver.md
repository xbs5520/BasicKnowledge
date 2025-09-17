# I/O Models

There are five classical I/O models in UNIX/Linux:

## 5 Models

1. **Blocking I/O**
   - The default. A `read()` call blocks until data is available.
   - Easy to reason about, but if the disk/network is slow → your process just sits idle.
2. **Non-blocking I/O**
   - You set the FD with `O_NONBLOCK`.
   - `read()` returns immediately:
     - if data is ready → you get it.
     - if not → you get `-1` with `errno = EAGAIN`.
   - CPU is not stuck, but now *you* must keep retrying (polling), which can be inefficient.
3. **I/O multiplexing** (`select()`, `poll()`, `epoll()`)
   - You wait on multiple FDs at once.
   - The kernel tells you which FDs are ready.
   - More efficient than blind polling.
4. **Signal-driven I/O**
   - Kernel sends a signal (e.g., `SIGIO`) when FD is ready.
   - Rarely used in practice (messy to handle signals).
5. **Asynchronous I/O (AIO)**
   - You submit a request and get notified when it’s complete (like callbacks or futures).
   - Linux has `io_uring` now, which is the shiny modern way.

## blocking VS No-blocking

```c++
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main() {
    char buf[10];

    // Open stdin in non-blocking mode
    // if change to blocking mode just delete these 2 line.
    int fd = fcntl(0, F_GETFL); // get current flags
    fcntl(0, F_SETFL, fd | O_NONBLOCK);

    printf("Try typing something (or press enter with no input):\n");

    int n = read(0, buf, sizeof(buf));
    if (n < 0) {
        if (errno == EAGAIN) {
            printf("No data available yet (non-blocking read).\n");
        } else {
            perror("read");
        }
    } else {
        buf[n] = '\0';
        printf("You typed: %s\n", buf);
    }

    return 0;
}
```

In blocking mode, `read()` would have *waited* forever.

in no-blocking mode no waiting, you can't type anything.

# Devices && Drivers

A **device** (like disk, keyboard, UART, network card) is just hardware.

The **driver** is software in the kernel that knows how to “speak the language” of that hardware.

User programs don’t talk directly to hardware. Instead, they use **system calls** (`open`, `read`, `write`, `ioctl`) → the kernel forwards these requests to the correct driver.

## Types of Devices

- **Character devices**: stream of bytes, like `/dev/ttyS0` (serial port).
- **Block devices**: random access in fixed-size blocks, like `/dev/sda` (hard disk).
- **Network devices**: special, exposed via sockets, not files.

## Virtual Files in `/dev`

Linux represents devices as files in `/dev`:

- `cat /dev/urandom` → gives you random bytes.
- `cat /dev/null` → discards everything you write.
- This is the famous **“everything is a file”** model.

```shell
# 1. See character and block devices
ls -l /dev | head -20

# 2. Redirect text into null device
echo "hello device" > /dev/null

# 3. Read random bytes (might look messy!)
head -c 16 /dev/urandom | hexdump

# 4. See your tty device (the terminal)
tty
```

## Why Linux looks every device as a file?

because it **unifies access**. Instead of inventing a new API for each device, the OS says:

“Want to talk to a terminal? Use `open`, `read`, `write`.”

“Want to talk to a disk? Same thing.”

“Want to talk to a random number generator? Still the same.”

### Benefits:

**Simplicity for programmers**:
 You don’t need to learn 50 different APIs for 50 devices. If you know how to work with files, you can work with almost anything.

**Composability (power of Unix philosophy)**:
 Because everything looks like a file, you can **pipe** and **redirect** seamlessly:

```
cat /dev/urandom > file.bin
cat /dev/input/mice > mouse.log
```

→ Same commands, different devices.

**Portability**:
 Applications don’t need to be rewritten when you swap hardware. The driver changes, but user programs keep using the file interface.

**Security and permissions**:
 Access can be controlled with file permissions (`chmod`, `chown`) instead of inventing a new security model per device.





