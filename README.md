# imgtwoascii
A simple tool written in C that turns any image (PNG, JPG, e.t.c) into ASCII art.


## Build

This tool uses [stb_image](https://github.com/nothings/stb) by Sean Barrett
to load images. Download the header into the same folder as `img2ascii.c`:

**Linux / macOS**
```
curl -O https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
```

**Windows (PowerShell)**
```
Invoke-WebRequest -Uri https://raw.githubusercontent.com/nothings/stb/master/stb_image.h -OutFile stb_image.h
```

As  compile:
```
gcc -O2 -o img2ascii img2ascii.c -lm
```

## Usage
```
./img2ascii -w 120 photo.jpg
```
Options: `-w N` width, `-c` colour, `-i` invert, `-s` short ramp, `-n` no contrast stretch.


<img width="1920" height="1074" alt="Screenshot_20261030_162011" src="https://github.com/user-attachments/assets/ce6a83de-1939-4f98-a8db-4382f6ed5b00" />

<img width="1920" height="1027" alt="Screenshot_20261030_162029" src="https://github.com/user-attachments/assets/616acf85-986a-45d6-9af7-579f9d1e3bcb" />

