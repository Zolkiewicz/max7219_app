# MAX7219 app

This project enables wireless (or local) control of an 8x32 LED matrix powered by the **MAX7219** driver, connected to a Raspberry Pi 
<img width="564" height="226" alt="Screenshot From 2026-07-15 21-40-51" src="https://github.com/user-attachments/assets/826d9c83-b838-462d-a7df-c98d10e28899" />
<img width="600" height="330" alt="eg3PI_ny" src="https://github.com/user-attachments/assets/ad3d7b6e-69ea-49cf-94d5-ffc787940aca" />

## Project Architecture

```text
.
├── pc/
│   ├── gui.py          # Interactive Python GUI (CustomTkinter)
│   └── sender.c        # C source for sending UDP packets (compiled to .so)
└── rpi/
    ├── Makefile        # Build orchestration for Raspberry Pi
    ├── inc/
    │   └── max7219.h   # MAX7219 register definitions & SPI driver headers
    └── src/
        ├── max7219.c   # SPI driver implementation for MAX7219
        └── receiver.c  # UDP Socket listener & hardware controller
 ```
### 1. Compile the Code

Raspberry Pi
```bash
cd rpi
make
```
PC
```bash
cd pc
gcc -shared -o libsender.so -fPIC sender.c
```
### 2. Turn It On (Run)

Raspberry Pi 
```bash
./max7219-rpi
```
PC
```bash
cd pc
python gui.py
```
