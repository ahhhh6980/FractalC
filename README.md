# FractalC  Version 0.2a-1
## Terminal Activated Mandelbrot & Julia Fractal Generator in C**
![Mandelbrot Zoom](https://i.imgur.com/lg1nzW1.png)![enter image description here](https://imgur.com/RiDgMZx.png)
## Setup;

### -Linux;
***To Compile***: **$** `gcc -o FractalC FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires GCC (any C compiler will do though), but it should already be installed

***To Run***: **$** `./FractalC [ARGUMENTS]`
### -Windows;
***To Compile 64-Bit***: **$** `x86_64-w64-mingw32 -o FractalC.exe FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires MinGW

***To Compile 32-Bit***: **$** `i686-w64-mingw32-gcc -o FractalC.exe FractalCMain.c -lm -fno-threadsafe-statics -O2 -lstdc++`
- Requires MinGW

***To Run***: **$** `start FractalC.exe [ARGUMENTS]`
## Arguments you can pass;
### You can pass 1 argument
1. **MODE**;
`Mode of 0 = Mandelbrot`
`Mode of 1 = Burning Ship`

***Example;***
**$** ` ./FractalC 0`
	
### You can pass 6 arguments
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

***Example;***
**$** `./FractalC 0 new.png 0.0 0.0 0.7 2500`

### You can pass 10 arguments
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
	
	
***Example;***
**$** `./FractalC 0 new.png -0.75 0 0.7 2500 0.25 2.0 4.0 3.5`
### You can pass 13 arguments
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
	
***Example;***
**$** `./FractalC 0 name.png 0.0 0.0 0.7 2500 0.25 2 4 2.5 1 -0.7269 0.188`

