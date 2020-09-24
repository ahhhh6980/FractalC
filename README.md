

# FractalC  v0.5b-1
## Terminal Activated Mandelbrot & Julia Fractal Generator in C/C++
![example](https://i.imgur.com/xeWEB2n.png)
![logo](https://i.imgur.com/LbFowGf.png)
## Now with advanced coloring!
![Large Image](https://doggo.ninja/Lh9yae.png)

## Setup;

### ---Compile From Source---
***Linux***: **$** `gcc -o FractalCv0.5b-1 FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires GCC (any C++ compiler will do though), but it should already be installed

***Windows x86-x64***: **>** `i686-w64-mingw32-gcc -o FractalCv0.5b-1.exe FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires MinGW

### ---Running the binaries---
***Linux***: **$** `./FractalCv0.5b-1 [ARGUMENTS]`

***Windows***: **>** `start FractalCv0.5b-1.exe [ARGUMENTS]`
# Input flags;
### You can input these in any order you want as long as they all appear no more than once in your input
## There are 32 possible flags
-**Flags starting with two dashes have no parameters**
* **-mode &nbsp; [mode (int)]**
	- `Mode of 0 = Mandelbrot`
	- `Mode of 1 = Burning Ship`
	- `Mode of 2 = Spade Fractal (computationally expensive to render)`

* **-name &nbsp; [name (string)]**
	- `Name ending in .png or .jpg`
	
* **-pos &nbsp; [posX (double)] &nbsp; [posY (double)]**
	 - `Complex Coordinate the image is centered on; two numbers`

* **-rot  &nbsp; [angle (double)]**
    - `Specifies Rotation of the entire fractal in degrees`

* **-zoom &nbsp; [zoom (double)]**
	- `Multiplier of magnification`

* **-lim  &nbsp; [limit (int)]**
	- `The iteration limit`

* **-cExp &nbsp; [exponent (double)]**
	- `The scale tuning of colors`
	
* **-res &nbsp; [multiplier (double) &nbsp; OR  &nbsp; shortcut (string)]**
	- `Multiplier of resolution`
	- `Image size = 60 * resolution * ratio`
	- `Pass lower-case "p" for a resolution width of 480`
	- `Pass lower-case "s" for a resolution width of 1920`
	- `Pass lower-case "l" for the largest resolution possible`
	- `Pass "4k" for a resolution width of 3840`
	- `Pass "8k" for a resolution width of 7680`
	- `Pass "16k" for a resolution width of 15360`
	
* **-ratio &nbsp; [ratio X (double)] &nbsp; [ratio Y (double)]**
	- `Ratio of image`
	
* **--isJulia**
	- `if specified, it will render a Julia Fractal`

* **-jPos &nbsp; [posX (double)] &nbsp; [posY (double)]**
	- `Julia Coordinates; enter two numbers`

* **-orbitTrap &nbsp; [mode (int)] &nbsp; [posX (double)] &nbsp; [posY (double)]**
	- `Render with orbit trap`
	- `Mode 1` **: cross trap**
	- `Mode 2` **: point trap**
	- `Mode 3` **: boundary trap**
	- `Mode 4` **: bound/cross trap**
	- `Mode 5` **: radial point trap**

* **--exclude**
	- `if specified, it will disable the exclusion of not rendering the main cardiod/bulb on the mandelbrot set`

* **-zMax  &nbsp; [limit (float)]**
	- `if Z.real or z.imag exceeds this value, it will stop the iteration for that pixel`
	- `example: 1e300`

* **-log  &nbsp; [number (int)]**
	- `Specifies number of console logs`

* **-cOffset  &nbsp; [offset (double)]**
	- `Multiple of 45 to offset the color`

* **-cScale  &nbsp; [multiplier (double)]**
	- `Multiplies frequency of color`
* **--bw**
	- `Specify for black and white`
* **--inverted**
	- `Inverts colors`
* **-fade &nbsp; [direction (string)]**
	- `Specify for a fade effect`
	- `Input must be "in" or "out"`
* **--fadeDark**
	- `If specified, fade will be dark`
	_ `If unspecified, fade will be light`
* **-lScale &nbsp; [scalar (double)]**
	- `Fade scale`
* **-lExp &nbsp; [exponent (double)]**
	- `Fade exponent, affects the outreach of the fade scale`
* **-cSpace &nbsp; [mode (int)] &nbsp; [exponent (int)]**
	- `coordinate space`
	- `input an integer then a double`
	- `first number is the mode (1 or 2)`
	- `second number is an exponential to warp the warp the coordinate space of the fractal`
* **-cSpaceComplex &nbsp; [mode (int)] &nbsp; [x Exponent (double)] &nbsp; [y Exponent(double)]**
	- `complex number coordinate space exponent`
	- `input an integer then two doubles`
	- `first number is the mode (1 or 2)`
	- `second number is complex number that acts as an exponential to warp the coordinate space of the fractal`
* **--useTexture**
    - `Might not behave as you would expect`
    - `Tells program to use a texture for depth map`
    - `X axis of image is chosen based off of the rotation around the center, or a given point`
    - `Y axis of image is chosen based off of the iteration depth`
    - `Must be a jpg, default is named "useThis.jpg"`
    - `Image must be a square`
* **-tName &nbsp; [name (string)]**
    - `Tells the program the name of the texture to use`
* **-tScale &nbsp; [X scale (double)] &nbsp; [Y scale ()double]**
    - `Scales the frequency of looping on the X and Y axi's of the texture`
* **-tOffset &nbsp; [X offset (double)] &nbsp; [Y offset ()double]**
    - `Offset the pixels on the X and Y axi's of the texture`
* **-tPos  &nbsp; [X Pos (double)] &nbsp; [Y Pos (double)]**
    - `Specifies a position for the texture to be centered on`
* **-tRot &nbsp; [angle (double)]**
    - `Rotates the rendering of the texture around the position`
* **-rMode &nbsp; [mode (int)]**
    - `Specifies render mode`
    - `Mode 1 = basic`
    - `Mode 2 = tree mode (long compute times)`

**Example Of An Input;**
- **$** `./FractalC --useTexture -pos 0 0 --isJulia -cSpace 1 2 -orbitTrap 4 1.5 0 -cScale 5 -cExp 0.1 -zMax 1e300 -jPos 0.2 0.65 -cSpaceComplex 2 0 1 -tOffset 960 0 -rot 45 -ratio 16 9 -lExp 2.5 -lScale 0.2 -zoom 1.4 -fade out -lim 10000 -res s `
- **>** `start FractalC.exe  --useTexture -pos 0 0 --isJulia -cSpace 1 2 -orbitTrap 4 1.5 0 -cScale 5 -cExp 0.1 -zMax 1e300 -jPos 0.2 0.65 -cSpaceComplex 2 0 1 -tOffset 960 0 -rot 45 -ratio 16 9 -lExp 2.5 -lScale 0.2 -zoom 1.4 -fade out -lim 10000 -res s `

![Example of use with new features](https://doggo.ninja/0UdqKE.png)

# License
```
MIT License

Copyright (c) 2020 ahhhh6980

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```



