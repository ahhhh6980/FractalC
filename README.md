
# FractalC  Version 0.1a-1
## Terminal Activated Mandelbrot Generator in C**
![Mandelbrot Zoom](https://i.imgur.com/lg1nzW1.png)
## Installation;
### Linux;
***To Compile***: **$** `gcc -o FractalC FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++`

***To Run***: **$** `./FractalC [ARGUMENTS]`
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

### You can pass 9 arguments
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


7. **RESOLUTION**
	`Multiplier of resolution`
	`Image size = 60 * resolution * ratio`

8. **RATIO X**
	`Ratio of image for X`
10. **RATIO Y**
	`Ratio of image for Y`
	
	
***Example;***
**$** `./FractalC 0 new.png -0.75 0 0.7 2500 2.0 4.0 3.5`

