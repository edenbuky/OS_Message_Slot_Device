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
