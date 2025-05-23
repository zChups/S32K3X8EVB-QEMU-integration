# Introduction
This project focuses on the integration of the NXP S32K3X8EVB board into the QEMU emulator. The board is based on the S32K358 System-on-Chip (SoC), which features an ARM Cortex-M7 processor and several on-chip peripherals including LPUART and PIT. The objective is to enable full emulation of this embedded platform to support development and debugging activities in a virtualized environment.

# Commands for Testing the Project

Below is a step-by-step guide to set up and use the files included in this project.

## Clone the Repository
To get a local copy of the repository, run:

```bash
git clone --recurse-submodules <repository-url>
```

Ensure your system and packages are up-to-date by running the following commands:
```bash
sudo apt update
sudo apt upgrade
```

## Build QEMU
Navigate to the QEMU source directory ($/qemu/$) on the repository.

Run the following commands:
```bash
./configure --target-list=arm-softmmu
```
The command `./configure --target-list=arm-softmmu` is used to configure the QEMU build system to include only the target for ARM architecture with the softmmu (software memory management unit).

> You may be prompted to install additional dependencies. Below is a list of the required ones.
```bash

sudo apt install python3-venv \
                 ninja-build \
                 pkg-config \
                 libglib2.0-dev \
                 bzip2 python3-pip libfdt-dev \
                 libpixman-1-dev zlib1g-dev
```
***
Then execute:

```bash
make -j$(nproc)
```
The command `make -j$(nproc)` is used to build a project with GNU make by utilizing all available CPU cores for parallel compilation.

## Build FreeRTOS
Navigate to the Demo project of FreeRTOS ($/freeRTOS\_porting/Demo/$) on the repository.

Run the following commands:

```bash
make [all]
```
## Run the Demo Project of FreeRTOS with the New Board
Finally, you can now start the Demo project of FreeRTOS, which uses the NXP S32K3X8EVB board and implements peripherals (LPUART and a Timer).

1.	Locate yourself inside the Demo directory of FreeRTOS.
2.	To run QEMU, use `make qemu_start`.
3. If you want to debug the application, follow these steps:
	- Open two terminals.
	- In the first terminal, run `make qemu_debug`.
    - In the second terminal, run `make gdb-start`.
    
    Using the gdb interface execute: `target remote localhost:1234`.

