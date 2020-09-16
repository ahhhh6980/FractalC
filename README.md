# FractalC
Version 0.1a-0
Terminal Activated Mandelbrot Generator in C

To Compile On Linux;
$ gcc -o FractalC FractalCMain.C -lm -fno-threadsafe-statics -O2 -lstdc++

To Run On Linux;
$ ./FractalC [ARGUMENTS]

Arguments you can pass;

You can pass 1 argument
1:: The MODE;
	Mode of 0 = Mandelbrot
	Mode of 1 = Burning Ship
	Example; 0
	
You can pass 6 arguments
1:: the MODE

2:: the NAME;
	Name ending in .png or .jpg.
	Example; new.png
	
3 & 4:: the POSITION;
	3; Position X
	4; Position Y
	Example; 0.0 0.0
	
5:: the ZOOM;
	Multiplier of magnification
	Example; 0.7

6:: the LIMIT;
	The iteration limit
	Example; 2500
	
You can pass 9 arguments
1:: the MODE
2:: the NAME;
3 & 4:: the POSITION;
5:: the ZOOM;
6:: the LIMIT;

7:: the RESOLUTION;
	Multiplier of resolution
	Image size = 60 * resolution * ratio
	Example; 2.0

8 & 9:: the RATIO;
	Image ratio
	Example; 4.0 3.5

EXAMPLE OF FULL INPUT;
$ ./FractalC 0 new.png -0.75 0 0.7 2500 2.0 4.0 3.5
