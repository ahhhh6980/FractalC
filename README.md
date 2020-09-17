

# FractalC  v0.2a-3
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
## Arguments you can pass;
### ---You can pass 1 argument---
1. **MODE**;
`Mode of 0 = Mandelbrot`
`Mode of 1 = Burning Ship`

**Example;**
- **$** ` ./FractalC 0`
- **>** `start FractalC.exe 0`
### ---You can pass 6 arguments---
1. **MODE**
	`Mode of 0 = Mandelbrot`
	`Mode of 1 = Burning Ship`
3. **NAME**
	`Name ending in .png or .jpg`
	
4. **POSITION X**
	 `Complex Coordinate X`
5.  **POSITION Y**
	`Complex Coordinate Y`
6. **ZOOM**
	`Multiplier of magnification`

7. **LIMIT**
	`The iteration limit`

**Example;**
- **$** `./FractalC 0 new.png 0.0 0.0 0.7 2500`
- **>** `start FractalC.exe 0 new.png 0.0 0.0 0.7 2500`
### ---You can pass 10 arguments---
1. **MODE**
	`Mode of 0 = Mandelbrot`
	`Mode of 1 = Burning Ship`
2. **NAME**
	`Name ending in .png or .jpg`
	
3. **POSITION X**
	 `Complex Coordinate X`
4.  **POSITION Y**
	`Complex Coordinate Y`
5. **ZOOM**
	`Multiplier of magnification`

6. **LIMIT**
	`The iteration limit`

7. **COLOR EXPONENT**
	`The scale of colors`
8. **RESOLUTION**
	`Multiplier of resolution ( Must be less than 101)`
	`Image size = 60 * resolution * ratio`

9. **RATIO X**
	`Ratio of image for X`
10. **RATIO Y**
	`Ratio of image for Y`
	
	
**Example;**
- **$** `./FractalC 0 new.png -0.75 0 0.7 2500 0.25 2.0 4.0 3.5`
- **>** `start FractalC.exe 0 new.png -0.75 0 0.7 2500 0.25 2.0 4.0 3.5`
### ---You can pass 13 arguments---
1. **MODE**
	`Mode of 0 = Mandelbrot`
	`Mode of 1 = Burning Ship`
2. **NAME**
	`Name ending in .png or .jpg`
	
3. **POSITION X**
	 `Complex Coordinate X`
4.  **POSITION Y**
	`Complex Coordinate Y`
5. **ZOOM**
	`Multiplier of magnification`

6. **LIMIT**
	`The iteration limit`

7. **COLOR EXPONENT**
	`The scale of colors`
8. **RESOLUTION**
	`Multiplier of resolution ( Must be less than 101)`
	`Image size = 60 * resolution * ratio`

9. **RATIO X**
	`Ratio of image for X`
10. **RATIO Y**
	`Ratio of image for Y`
11. **isJulia**
	`0 = Dont Render Julia (Default)` `1 = Render Julia`
12. **JuliaX**
	`Julia Coord X`
13. **JuliaY**
	`Julia Coord Y`
	
**Example;**
- **$** `./FractalC 0 name.png 0.0 0.0 0.7 2500 0.25 2 4 2.5 1 -0.7269 0.188`
- **>** `start FractalC.exe 0 name.png 0.0 0.0 0.7 2500 0.25 2 4 2.5 1 -0.7269 0.188`
### ---You can pass 18 arguments---
1. **MODE**
	`Mode of 0 = Mandelbrot`
	`Mode of 1 = Burning Ship`
2. **NAME**
	`Name ending in .png or .jpg`
	
3. **POSITION X**
	 `Complex Coordinate X`
4.  **POSITION Y**
	`Complex Coordinate Y`
5. **ZOOM**
	`Multiplier of magnification`

6. **LIMIT**
	`The iteration limit`

7. **COLOR EXPONENT**
	`The scale of colors`
8. **RESOLUTION**
	`Multiplier of resolution ( Must be less than 101)`
	`Image size = 60 * resolution * ratio`

9. **RATIO X**
	`Ratio of image for X`
10. **RATIO Y**
	`Ratio of image for Y`
11. **isJulia**
	`0 = Dont Render Julia (Default)` `1 = Render Julia`
12. **JuliaX**
	`Julia Coord X`
13. **JuliaY**
	`Julia Coord Y`
14. **FANCY COLORS**
	`Enable for very trippy colors (slower rendering)`
15. **Z MAGNITUDE LIMIT**
	`if Z.real or z.imag exceeds this value, it will stop the iteration for that pixel`
16. **LOG COUNT**
	`Specifies number of console logs`
17. **COLOR OFFSET**
	`Multiple of 45 to offset the color`
18. **COLOR MULTIPLIER**
	`Multiplies frequency of color`
	
**Example Of Maximum Input;**
- **$** `./FractalC 0 1.png 0 0 8 10000 1 0.25 16 9 1 -0.041747 0.699 0 1e2 20 3 10`
- **>** `start FractalC.exe 0 1.png 0 0 8 10000 1 0.25 16 9 1 -0.041747 0.699 0 1e2 20 3 10`
## License
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
