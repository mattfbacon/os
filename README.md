# Matt's OS

This may be the most futile project I've ever decided to embark on but I think it'll be fun nonetheless.

## Goals

At first I will mostly be learning about how things like APICs, IDTs, and whatever else comes up in the path toward a basic CLI. I expect to have a lot of redesigning and rewriting.

The long-term goal is a filesystem (probably FAT-32) and CLI. After that is achieved the next goal will be a GUI with a basic window manager.

## Building and Running

```shell
make
# using QEMU
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
# or Bochs
bochs -q
```

## Progress

### Kernel Space

- [x] Boots!
- [x] Interrupt Handlers (including error codes)
- [x] Paging
- [x] Long Mode
- [ ] Filesystem (in progress)
- [ ] Memory Manager
- [ ] Tasks + Scheduler
- [ ] Software Interrupts (System Calls)
  - [ ] Request to map more memory (basis of something like `malloc`)
  - [ ] File IO (including `stdin` and `stdout`)
  - [ ] Task Control (`fork`, `kill`, `waitpid`)
  - [ ] Sleep
  - [ ] Exit
  - [ ] Shutdown, Reboot
- [ ] Executable Loading (then enter user-space!)
  - [ ] Simple C library

### User Space

- [ ] Init Binary
- [ ] Programs
  - [ ] Shell
  - [ ] Utilities a là GNU Coreutils
  - [ ] Package Manager

### Kernel Space Again

- [ ] Expanded Device Support
  - [ ] USB
  - [ ] Displays
  - [ ] Thunderbolt??
- [ ] Implement `syscall` (as opposed to `int 0x80`)
- [ ] Framebuffers (instead of BIOS text mode)
  - [ ] Basic Support
  - [ ] Double Buffering
- [ ] Allow Processes to Write to Graphics
  - [ ] Probably another syscall

### User Space Again

- [ ] Basic Graphics Testing
- [ ] Window Manager
  - [ ] Graphics Library
  - [ ] Display Server (like X11 or Wayland)
  - [ ] Window Manager itself
- [ ] GUI Toolkit (like GTK or QT)
- [ ] GUI Applications
  - [ ] Terminal Emulator
  - [ ] File Manager
  - [ ] Web Browser
- [ ] Porting other OSS to this OS
