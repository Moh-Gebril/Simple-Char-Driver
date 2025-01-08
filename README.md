# Simple Character Driver

This repository contains a simple Linux kernel character device driver module that demonstrates basic kernel module programming concepts.

## Overview

The module creates a character device that can be read from and written to from userspace. It implements basic file operations including open, read, write, and release.

## Features

- Character device driver implementation
- Basic file operations (open, read, write, release)
- Dynamic major number allocation
- Device class and device file creation
- Proper cleanup in module exit
- Thread-safe operations

## Prerequisites

- Linux kernel headers
- GCC compiler
- Make build system

## Cloning the Project from GitHub

To clone this project from GitHub, use the following command:

```bash
git clone https://github.com/Moh-Gebril/Simple-Char-Driver.git
```

After cloning, navigate into the project directory:

```bash
cd SimpleCharDriver
```

## Building

To build the kernel module, simply run:

```bash
make
```

## Loading the Module

After building, you can load the module using:

```bash
sudo insmod build/chardev.ko
```

The device will be created at `/dev/chardev`

## Permissions Setup

Before using the character device, ensure that you have the necessary permissions. Run the following command:

```bash
sudo chmod 666 /dev/chardev
```

This command allows all users to read from and write to the device.

## Testing

You can test the device using standard file operations:

```bash
# Write to device
echo "Hello" > /dev/chardev

# Read from device
cat /dev/chardev
```

## Unloading the Module

To unload the module:

```bash
sudo rmmod chardev
```

## License

This project is licensed under the GPL v2 License - see the LICENSE file for details.

## Author

Created by Mohamed Gebril

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
