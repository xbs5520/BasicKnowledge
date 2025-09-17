# OS designs

## OS Type

**Monolithic Kernel**

- Everything (scheduling, memory management, drivers, file systems) runs in **kernel space**.
- Example: Linux, FreeRTOS, ThreadX
-  ✅ Fast (no overhead of message passing). Performance, Simplicity of design, Extensibility via modules.
- ❌ Harder to maintain/debug, a buggy driver can crash the whole system.

**Microkernel**

- Keep only the bare minimum in kernel: scheduling, IPC, basic memory.
- Everything else (drivers, file systems) runs in **user space** as services.
- Example: Minix, QNX.
- ✅ More stable, easier to update drivers.
- ❌ More overhead (lots of IPC).

**Hybrid Kernel**

- A mix: looks like microkernel, but some parts (drivers, filesystems) still in kernel for performance.
- Example: Windows NT, macOS (XNU).

**Exokernel** (research idea)

- Kernel does almost nothing — just **multiplexes hardware**.
- User programs get maximum control.
- ✅ Maximum flexibility.
- ❌ Unsafe, complex to program.

## In an embedded system, would you prefer microkernel (safety) or monolithic (speed)?

- If your system is **safety-critical** (e.g., medical device, avionics, automotive), microkernel is often preferred. Why? Because faults in one driver/service won’t crash the whole system.
- If your system is **resource-limited but needs speed** (e.g., consumer electronics, routers), monolithic is better because it avoids IPC overhead.

## For IoT device which style would you pick?

Devices are usually **tiny (low RAM/CPU)** and need **deterministic response times**.

A **hybrid/minimal microkernel** is often chosen (like Zephyr, QNX, or seL4).

- Core: only scheduling, IPC, memory mgmt.
- Drivers & services: in user space (crashes won’t kill the kernel).

If I care more about cost & performance (like a smart bulb), I’d choose a **lightweight monolithic RTOS** (like FreeRTOS).

# Protection & Security

## Protection Mechanisms

**Protection** = *internal mechanisms* to ensure processes don’t interfere (e.g., memory protection, access control).

- **Memory protection**
  - Use MMU to stop one process from reading another’s memory.
  - Example: segmentation fault when you dereference invalid pointer.
- **CPU modes**
  - **User mode** vs **Kernel mode**.
  - Kernel mode can do I/O, manage memory. User mode is restricted.
- **Access Control**
  - Permissions (rwx) on files, devices.
  - Access Control Lists (ACLs) for fine-grained control.

## Security Mechanisms

- **Authentication** – passwords, biometrics, tokens.
- **Authorization** – what you can do after login (permissions).
- **Encryption** – protect data at rest and in transit.
- **Auditing** – logs to detect misuse.

**Security** = *external defense* against malicious users/attacks (e.g., authentication, encryption).

## Common Security Threats

- **Buffer overflow** → inject code.
- **Privilege escalation** → normal user gets root access.
- **Race conditions** → two processes exploit timing to break protection.
- **Denial of Service (DoS)** → hog resources.

## Q&A

### Why do we need **user mode vs kernel mode**?

- **User mode**: runs normal applications with limited privileges.
  - Can’t directly access hardware (disk, NIC, etc.).
  - If a bug happens, damage is contained.
- **Kernel mode**: runs OS code with full privileges.
  - Can manage memory, devices, and schedule processes.

Separation prevents buggy or malicious apps from crashing the whole system.

### How does the OS stop one process from corrupting another’s memory?

- Each process has its own **virtual address space**.

- The **MMU (Memory Management Unit)** + page tables map virtual addresses → physical addresses.

- Access outside allowed pages = **segmentation fault** (SIGSEGV).

  This isolation ensures one process can’t read/write another’s memory.

### What’s the difference between **authentication** and **authorization**?

- **Authentication** = *Who are you?*
  - Example: password, fingerprint, SSH key.
- **Authorization** = *What are you allowed to do?*
  - Example: after login, you can read `/home/you`, but not `/etc/shadow`.
     👉 AuthN = identity check, AuthZ = permission check.

### Why are **buffer overflows** dangerous in C/C++?

- C/C++ doesn’t automatically check array bounds.
- Writing beyond the buffer can overwrite:
  - **Adjacent variables** → corrupt data.
  - **Return addresses** → attacker injects malicious code.
- Famous example: **stack smashing** → attacker hijacks control flow.
   👉 That’s why modern OSes use protections: **ASLR, stack canaries, DEP**.

Always connect buffer overflow → *security exploit*. It’s not just a crash, it can give attackers root shell.

# Virtualization&&Containers

## **Virtualization**

- Runs **multiple OSes** on one physical machine.
- Uses a **hypervisor** (e.g., VMware, KVM, Hyper-V).
- Each VM has **its own kernel + OS**.
- Pros: strong isolation, can run different OSes (Linux + Windows on same host).
- Cons: heavy — each VM duplicates kernel, memory overhead.

## **Containers**

- Share the **same host kernel**, but isolate **processes, file system, network**.
- Tools: Docker, Podman, containerd.
- Much lighter than VMs.
- You can run 100s of containers where maybe only 10–20 VMs fit.
- Pros: lightweight, fast startup, great for microservices.
- Cons: weaker isolation (since all share kernel).

## **OS Support**

- Linux provides:
  - **Namespaces** → isolation (PID, network, mount, user).
  - **cgroups** → resource control (CPU, memory limits).

## **Comparison**

| Feature        | Virtual Machine                      | Container                        |
| -------------- | ------------------------------------ | -------------------------------- |
| Isolation      | Strong (separate OS)                 | Weaker (shared kernel)           |
| Startup Time   | Slow (seconds–minutes)               | Fast (ms–s)                      |
| Resource Usage | Heavy (full OS per VM)               | Light (process-level)            |
| Portability    | OS-level images                      | Application-level images         |
| Use Cases      | Run Windows on Linux, full isolation | Microservices, cloud apps, CI/CD |

## Q&A

### Why are **containers faster than VMs**?

- **Shared kernel**: Containers don’t need to boot a full OS—just processes.
- **No hardware emulation**: VMs need a hypervisor to emulate hardware for each guest OS.
- **Lightweight filesystem**: Containers use layered images, so less disk/memory overhead.
- **Startup time**: Containers can start in milliseconds; VMs often take seconds or minutes.

### How do **containers achieve isolation**?

Containers use **Linux kernel features**:

- **Namespaces** → isolate process IDs, network stack, mount points, users.
- **cgroups (control groups)** → limit CPU, memory, I/O, preventing one container from starving others.
- **Capabilities** → restrict what processes inside a container can do, e.g., prevent them from changing kernel settings.

This gives the **illusion of separate OSes** without actually duplicating a kernel.

### When would you pick a **VM instead of a container**?

- **Strong isolation needed**: You don’t trust the code inside, or need different OSes.
- **Different OS environments**: Run Windows on Linux, or legacy OSes.
- **Full kernel testing**: If you need a completely separate kernel for testing.
- **Security-critical workloads**: Banking, medical, or government systems may prefer VMs.