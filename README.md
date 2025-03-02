# Message Slot Kernel Module

This project implements a kernel module that provides a new inter-process communication (IPC) mechanism, called a message slot. The message slot is a character device file through which processes can communicate by reading and writing messages. The module supports multiple message channels and handles message communication in a concurrent manner.

## Overview

The project includes three main components:
1. **Message Slot Kernel Module**: A kernel module that creates a message slot device for inter-process communication.
2. **Message Sender**: A user-space program that sends messages to the message slot.
3. **Message Reader**: A user-space program that reads messages from the message slot.

The message slot device operates as a pseudo-device that doesn't correspond to physical hardware. Each message slot file corresponds to a different channel, and messages can be read or written using the `ioctl()`, `write()`, and `read()` system calls.

## Features

- **Message Channels**: Each message slot file supports multiple channels (up to 220).
- **Message Handling**: The kernel module handles atomic writes and reads, where each write writes the entire message and each read retrieves the last message written to the channel.
- **IPC Mechanism**: Provides a communication channel between user-space processes, with support for multiple simultaneous processes communicating over different channels.
- **Error Handling**: Proper error handling for invalid operations such as unsupported `ioctl()` commands, invalid message sizes, or uninitialized channels.

## File Structure

- **message_slot.c**: The kernel module that implements the message slot device driver.
- **message_slot.h**: Header file for the message slot module.
- **message_sender.c**: A user-space program that sends messages to the message slot.
- **message_reader.c**: A user-space program that reads messages from the message slot.
- **Makefile**: A makefile to compile the kernel module and user-space programs.

## How to Use

### Step 1: Load the Kernel Module
As root, load the `message_slot` kernel module:

```bash
sudo insmod message_slot.ko
```

### Step 2: Create a Message Slot Device File
Use the `mknod` command to create message slot device files with different minor numbers:

```bash
sudo mknod /dev/slot0 c 235 0
sudo mknod /dev/slot1 c 235 1
```

### Step 3: Set Permissions
Change the file permissions to make it readable and writable by your user:
```bash
sudo chmod 666 /dev/slot0 /dev/slot1
```

### Step 4: Send a Message
Use the `message_sender` program to send a message to the message slot. For example:
```bash
./message_sender /dev/slot0 1 "Hello, Slot 1"
```
### Step 5: Read a Message
Use the `message_reader` program to read the message from the message slot:
```bash
./message_reader /dev/slot0 1
```
### Step 6: Repeat for Different Channels
You can send and read messages on different channels by specifying the correct channel ID.

## Functions and Operations

### `ioctl()`
Sets the message channel ID for the file descriptor. It accepts a single unsigned integer specifying the channel ID.

- **Error Handling**: 
  - If an invalid channel ID is passed (e.g., `0`), returns `-1` and sets `errno` to `EINVAL`.

### `write()`
Writes a message to the specified channel. The message can be up to 128 bytes in length.

- **Error Handling**: 
  - If no channel is set, returns `-1` and sets `errno` to `EINVAL`.
  - If the message size is invalid (0 or more than 128 bytes), returns `-1` and sets `errno` to `EMSGSIZE`.

### `read()`
Reads the last message from the specified channel.

- **Error Handling**: 
  - If no channel is set, returns `-1` and sets `errno` to `EINVAL`.
  - If no message exists on the channel, returns `-1` and sets `errno` to `EWOULDBLOCK`.
  - If the buffer is too small, returns `-1` and sets `errno` to `ENOSPC`.

## Memory Management

The kernel module uses dynamic memory allocation (`kmalloc()`) for storing messages. It ensures efficient memory usage based on the number of channels and the size of messages. Memory is freed when a message slot file is closed or the module is unloaded.

## Compilation

To compile the kernel module and user-space programs, use the provided Makefile:

```bash
make
```
This will compile the `message_slot.ko` kernel module and the user-space programs `message_sender` and `message_reader`.

## Example Session

1. **Load the kernel module**:

   ```bash
   sudo insmod message_slot.ko

2. **Create message slot files**:

   ```bash
   sudo mknod /dev/slot0 c 235 0
   sudo mknod /dev/slot1 c 235 1

3. **Set permissions:**

   ```bash
   sudo chmod 666 /dev/slot0 /dev/slot1


4. **Send a message:**
   ```bash
   ./message_sender /dev/slot0 1 "Hello, Slot 1"

5. **Read the message:**
   ```bash
   ./message_reader /dev/slot0 1

## Conclusion
This project provides a practical implementation of an IPC mechanism using message slots. It demonstrates how kernel modules can manage device files and inter-process communication in a Linux environment.





