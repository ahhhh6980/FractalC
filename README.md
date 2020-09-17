

# FractalC  v0.3a-1
## Terminal Activated Mandelbrot & Julia Fractal Generator in C
![logo](https://i.imgur.com/9MKpYtN.png)
## Setup;

### ---Compile From Source---
***Linux***: **$** `gcc -o FractalC_linux64 FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires GCC (any C compiler will do though), but it should already be installed

***Windows x86-x64***: **>** `x86_64-w64-mingw32 -o  FractalC_winx64-x86.exe FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires MinGW

### ---Running the binaries---
***Linux***: **$** `./FractalCv0.2a-2 [ARGUMENTS]`

***Windows***: **>** `start FractalCv0.2a-2.exe [ARGUMENTS]`
# Input flags;
### You can input these in any order you want as long as they all appear no more than once in your input
## There are 15 possible flags
* **-mode**
	`Mode of 0 = Mandelbrot`
	`Mode of 1 = Burning Ship`

* **-name**
	`Name ending in .png or .jpg`
	
* **-pos**
	 `Complex Coordinate the image is centered on; two numbers`

* **-zoom**
	`Multiplier of magnification`

* **-lim**
	`The iteration limit`

* **-cExp**
	`The scale tuning of colors`
	
* **-res**
	`Multiplier of resolution ( Must be less than 101)`
	`Image size = 60 * resolution * ratio`
	
* **-ratio**
	`Ratio of image`
	
* **-isJulia**
	`if specified, it will render a Julia Fractal`

* **-jPos**
	`Julia Coordinates; enter two numbers`

* **-fancy**
	`If specified, it will enable trippy colors (slower rendering)`

* **-zMax**
	`if Z.real or z.imag exceeds this value, it will stop the iteration for that pixel`

* **-log**
	`Specifies number of console logs`

* **-cOffset**
	`Multiple of 45 to offset the color`

* **-cScale**
	`Multiplies frequency of color`
	
**Example Of An Input;**
- **$** `./FractalC -cExp 1 -cOffset 1 -pos 0 0 -res 10 -ratio 4 3 -fancy -isJulia -jPos -0.8 0.16 -cScale 0.55 `
- **>** `start FractalC -cExp 1 -cOffset 1 -pos 0 0 -res 10 -ratio 4 3 -fancy -isJulia -jPos -0.8 0.16 -cScale 0.55 `

![Input example](https://i.imgur.com/lZEAXW2.png)
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

