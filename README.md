# Matt's OS

This may be the most futile project I've ever decided to embark on but I think it'll be fun nonetheless.

## Goals

At first I will mostly be learning about how things like APICs, IDTs, and whatever else comes up in the path toward a basic CLI. I expect to have a lot of redesigning and rewriting.

The long-term goal is a filesystem and CLI. After that is achieved the next goal will be a GUI with a basic window manager.

## Building and Running

```shell
make build-x86_64
qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
```
