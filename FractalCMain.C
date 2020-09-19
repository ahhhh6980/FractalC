#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include <time.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define E exp(1)

typedef struct {
	double real;
	double imag;
} complex_t;

typedef struct {
	uint32_t w;
	uint32_t h;
} bounds_t;

typedef struct {
	double a;
	double b;
} range_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba_t;

typedef struct {
    uint32_t x;
    uint32_t y;
} pixel_t;

typedef struct {
	int i;
	double d;
} i_out_t;

typedef struct {
	double h;
	double s;
	double l;
} hsv_t;

// Maps i from range a to range b!
inline double map_to_range( double i, range_t r1, range_t r2 ){
	if(i<r1.a){ return r2.a; }
	if(i>r1.b){ return r2.b; }
	return ( ( ( i - r1.a ) / (r1.b - r1.a) ) * (r2.b - r2.a) ) + r2.a ;
}

#define C_RANGE1 range_t {0, 60}
#define C_RANGE2 range_t {0, 255}
inline uint8_t convert_hue( int theta )
{
	theta %= 360;
	if( theta >= 300 || theta <= 60 ){ return 255; }
	if( theta < 300 && theta > 240 ){ return ((int)( map_to_range( theta % 60, C_RANGE1, C_RANGE2 ) )); }
	if( theta > 60  && theta < 120 ){ return ((int)( 255 - map_to_range( theta % 60, C_RANGE1, C_RANGE2 ) )); }
	return 0;
}

inline rgba_t hsv_to_rgb( int h, double s, double v )
{
	rgba_t pureColor = { convert_hue( h ), convert_hue( h - 120 ), convert_hue( h + 120 ), 255 };
	int m = MAX( MAX(pureColor.r,pureColor.g), pureColor.b );
	rgba_t output;
    output.r = (int)( (m - ((m - pureColor.r ) * s )) * v );
    output.g = (int)( (m - ((m - pureColor.g ) * s )) * v );
    output.b = (int)( (m - ((m - pureColor.b ) * s )) * v );
	return output;
}

// Get Frame size
inline bounds_t get_frame( range_t ratio, double res ){
	return { (long long unsigned int)( res * ratio.a * 60 ), (long long unsigned int)( res * ratio.b * 60 ) };
}

double resolution;
range_t ratio;
bounds_t frame ;

double zoom ;

complex_t position;

range_t xRange;
range_t yRange;

uint16_t limit;
double zLimit;

bool fancyColors;

double exponent;
double logs;
double e;
double iterator;
double vbv;

time_t start;
time_t totalT;
rgba_t rgba;
i_out_t out;

int mode;
complex_t jCoord;
int isJulia;

double cOffset;
double cMult;
char* n;

double lightnessScale;
double lightnessExponent;
int monochrome;
int tempSv;

int fade;

int largest;
int presub;
int invert;
int fadeDark;

inline complex_t c_div_n( complex_t a, double b){
	return { a.real / (double)(b), a.imag / (double)(b) };
}

inline complex_t c_add( complex_t a, complex_t b ){
	return { a.real+b.real , a.imag+b.imag };
}

inline complex_t c_mult( complex_t a, complex_t b ){
	return { a.real * b.real - ( a.imag * b.imag ), a.imag * b.real + b.imag * a.real };
}

inline complex_t n_pow_c( double n, complex_t c ){
	return c_mult( complex_t { cos( c.imag * log( n ) ), sin( c.imag * log( n ) ) } ,complex_t { pow( n, c.real ) , 0 } );
}

inline complex_t conjugate( complex_t c ){
	return {c.real,-c.imag};
}

inline complex_t c_div( complex_t a, complex_t b ){
	return c_div_n( c_mult( a, conjugate(b) ) , (b.real*b.real + b.imag*b.imag));
}

inline complex_t c_pow_c( complex_t a, complex_t b )
{
	if ( a.real == 0.0 && a.imag == 0.0 )
		return { 0, 0 };
	if ( b.real == 0.0 && b.imag == 0.0 )
		return { 1, 0 }; 
	complex_t pa = n_pow_c( a.real*a.real + a.imag*a.imag, c_div_n( b, 2 ) );
	complex_t pb = n_pow_c( E, c_mult(  c_mult( complex_t {0,1}, b)  , complex_t { atan( b.imag / a.real ), 0 } ) );
	return  c_mult( pa, pb );
}

// Complex "Modulus" (Distance from 0,0)
inline double modulus( complex_t c ){
	return sqrt( c.real * c.real + c.imag * c.imag );
}

inline double arg( complex_t c ){
	return atan(c.imag/c.real);
}

// Complex power function!
inline complex_t c_pow( complex_t c, uint8_t n )
{
	if( n == 1 ){ return c; }
	complex_t temp = c_pow( c, n/2 );
	if( n % 2 == 0){ return c_mult( temp, temp ); }
	else{ return c_mult( c, c_mult( temp, temp ));}
}

inline double logb( double b, double x ){
	if( x == 0 || b == 0 ){ return 0; }
	else{ return log(x) / log(b); }
}

inline double scale_to_range( double i, range_t r1, range_t r2, double v){
	if(i<r1.a){ return r2.a; }
	if(i>r1.b){ return r2.b; }
	return ( pow(  i - r1.a, ( v + logb(i - r1.a, r2.b - r2.a) )  ) / pow(r1.b - r1.a, v) + r2.a );
}

// Encodes pixel index into RGBA!
inline rgba_t rgba_from_index(uint32_t x) {
    return *((rgba_t*) &x);
}

// Decodes RGBA to a pixel index!
inline pixel_t rgba_to_pixel( rgba_t rgba ){
	uint32_t pos = *((uint32_t*) &rgba);
	return { ((uint16_t) (pos % frame.w)), ((uint16_t) (pos / frame.h)) };
}

// Compute pixel
i_out_t compute( pixel_t pixel )
{

	float d = 1e20;

	uint16_t i = 0;
	//pixel_t pixel = rgba_to_pixel( color );

	complex_t c = { map_to_range( pixel.x, range_t { 0, (double)(frame.w) }, xRange), map_to_range( pixel.y, range_t { 0, (double)(frame.h) }, yRange) };
	complex_t z = c;
	if(isJulia==1){
		c = jCoord;
	}

	double p = sqrt( (c.real - 0.25)*(c.real - 0.25) + c.imag*c.imag );

	if( (mode!=0) || ((c.real > p - 2*p*p + 0.25) && ((c.real+1)*(c.real+1) + c.imag*c.imag  > 0.0625)  )){
		while(i < limit){
			//z = {abs(z.real), abs(z.imag)};
			switch(mode){
				case -1:
					return { -1, d };
				case 0:
					z = c_add( c_mult( z, z ), c );
					break;
				case 1:
					z = { abs(z.real), abs(z.imag) };
					z = c_add( c_mult( z, z ), c );
					break;
				case 2:
					z = c_add( c_pow_c( z, z ), c_div( z, c_add( c, complex_t {0.001, 0.001} ) ) );
					break;
			}

			if( z.real*z.real > zLimit || z.imag*z.imag > zLimit ){
				return { i, d }; 
			}

			i++;
			if(fancyColors){ d = MIN( abs( modulus(z) ) , d); }
		}
	} else { return { -1, d }; }
	return { -1, d };
}

void draw_image( char name[] )
{

	start = time(NULL);
	iterator = 0.0;
	vbv = ( double )(frame.w * frame.h) / 2;
	xRange = { ( -1 / zoom ) + position.real, ( 1 / zoom ) + position.real };
	yRange = {  ( (-(ratio.b/ratio.a) / zoom ) + position.imag ), (( (ratio.b/ratio.a) / zoom ) + position.imag )};
	printf("X RANGE: {%lf, %lf}\n", xRange.a, xRange.b);
	printf("Y RANGE: {%lf, %lf}\n", yRange.a, yRange.b);
	uint8_t* pixels = (uint8_t* ) malloc((uint64_t)(frame.w) * (uint64_t)(frame.h) * (uint64_t)(3) * sizeof(uint8_t));
	rgba = {0,0,0,0};
	out = {0,0};

	double v = 0.0;
	uint64_t index = 0;
	range_t range1 = {0.0, (double)(limit)};
	range_t range2 = {0.0, 360};
	double valueChange;
	for(uint16_t y = 0; y < frame.h; y++){
		for(uint16_t x = 0; x < frame.w; x++){
			for(double fy = 0.0; fy < logs; fy++ ){
				if( iterator / vbv == fy/logs ){
					printf("log %d: %d%%, @%lds\n", (int)( fy + 1 ), (int)((iterator / vbv ) * 100), time(NULL)-start );
				}
			}
			out = compute( pixel_t { x, y } );
			if( out.i == -1 ){
				pixels[index++] = 0;
   				pixels[index++] = 0;
   				pixels[index++] = 0;
			} else {
				if(fancyColors){
					v = scale_to_range( out.i, range1, range2, exponent*out.d);
				} else {
					v = scale_to_range( out.i, range1, range2, exponent);
				}

				switch(fade){
					case -1:
						valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						rgba = hsv_to_rgb( ((invert==1) ? 360 - ((v * cMult) + cOffset) : ((v * cMult) + cOffset)) , 1, 1 - scale_to_range(valueChange, range_t { 0, 2500 }, range_t { 0, 1 }, 0.5 ) );
						break;
					case 0:
						rgba = hsv_to_rgb( ((invert==1) ? 360 - ((v * cMult) + cOffset) : ((v * cMult) + cOffset)), 1, 1 );
						break;
					case 1:
						valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						rgba = hsv_to_rgb( ((invert==1) ? 360 - ((v * cMult) + cOffset) : ((v * cMult) + cOffset)) , 1, scale_to_range(valueChange, range_t { 0, 2500 }, range_t { 0, 1 }, 0.5 ) );
						break;
				}
				

				if(monochrome==1){
					tempSv = (int)(255.0 * scale_to_range( out.i, range_t { 0, 2500 }, range_t { 0, 1 }, cMult ));
					pixels[index++] = tempSv;
   					pixels[index++] = tempSv;
   					pixels[index++] = tempSv;
				} else {
					pixels[index++] = abs(rgba.r + presub);
   					pixels[index++] = abs(rgba.g + presub);
   					pixels[index++] = abs(rgba.b + presub);
				}

			}
			iterator++;
		}
	}

	printf("\nplease wait while it's saving...\n");
	stbi_write_png(name, frame.w, frame.h, 3, pixels, frame.w * 3);

	totalT = time(NULL)-start;

	if(totalT!=1){
		printf("\nFinished and saved in %ld seconds\n",totalT);
	} else {
		printf("\nFinished and saved in %ld second\n",totalT);
	}
	free(pixels);
	printf("\nPixel Array is now free from memory\nIt is now safe to close the program\n\n");
}

const static struct{
	const char* key;
	int v;
} check_key [] = {
	{"-mode", 1},
	{"-pos", 2},
	{"-zoom", 3},
	{"-lim", 4},
	{"-cExp", 5},
	{"-res", 6},
	{"-ratio", 7},
	{"-isJulia", 8},
	{"-jPos", 9},
	{"-fancy", 10},
	{"-zMax", 11},
	{"-log", 12},
	{"-cOffset", 13},
	{"-cScale", 14},
	{"-name", 15},
	{"-bw", 16},
	{"-lScale", 17},
	{"-lExp", 18},
	{"-fadeDark", 19},
	{"-fade", 20},
	{"-inverted", 21}
};

int get_key( const char *key ){
	for( int i = 0; i < 21; ++i ){
		if( !strcmp(key, check_key[i].key) )
			return check_key[i].v;    
	}
	return 0;
}

int main(int argc, char *argv[])
{	
	printf("\n");
	n = "generatedFractal.png";
	mode = 0;
	position = { -0.75, 0 };
	zoom = 0.7;
	limit = 2500;
	exponent = 0.25;
	resolution = 2.0;
	ratio = { 4.0, 3.5 };
	isJulia = 0;
	jCoord = {-0.7269, 0.188};
	fancyColors = 0;
	zLimit = 1e10;
	logs = 10;
	cOffset = 0;
	cMult = 1;
	largest = 0;
	lightnessScale = 1;
	lightnessExponent = 2;
	monochrome = 0;
	fade = 0;
	presub = -255;
	invert = 0;
	fadeDark = 0;
	for(int i = 0; i < argc; ++i){
		switch( get_key( argv[i] ) ) {
			case 1:
				mode = atoi(argv[i+1]);
				break;
			case 2:
				position = { strtod(argv[i+1], NULL), strtod(argv[i+2], NULL) };
				break;
			case 3:
				zoom = strtod(argv[i+1], NULL);
				break;
			case 4:
				limit = atoi(argv[i+1]);
				break;
			case 5:
				exponent = strtod(argv[i+1], NULL);
				break;
			case 6:
				if(strcmp(argv[i+1], "l")==0){	
					largest = 1;
				} else {
					resolution = strtod(argv[i+1], NULL);
				}
				break;
			case 7:
				ratio = {strtod(argv[i+1], NULL), strtod(argv[i+2], NULL)};
				break;
			case 8:
				isJulia = 1;
				break;
			case 9:
				jCoord = {strtod(argv[i+1], NULL), strtod(argv[i+2], NULL)};
				break;
			case 10:
				fancyColors = 1;
				break;
			case 11:
				zLimit = strtod(argv[i+1], NULL);
				break;
			case 12:
				logs = atoi(argv[i+1]);
				break;
			case 13:
				cOffset = 45 * strtod(argv[i+1], NULL);
				break;
			case 14:
				cMult = strtod(argv[i+1], NULL);
				break;
			case 15:
				n = argv[i+1];
				break;
			case 16:
				monochrome = 1;
				break;
			case 17:
				lightnessScale = strtod(argv[i+1], NULL);
				break;
			case 18:
				lightnessExponent = strtod(argv[i+1], NULL);
				break;
			case 19:
				fadeDark = 1;
				break;
			case 20:
				if( strcmp(argv[i+1], "out" ) == 0 ){
					//cOffset+=4.5;
					//presub = 0;
					fade = -1;
				} else if ( strcmp(argv[i+1], "in" ) == 0 ) {
					//presub = 0;
					fade = 1;
				} 
				break;
			case 21:
				invert = 1;
				break;
		}
	}
	if( fade!=0 && fadeDark==1 ){ presub = 0; }
	if(largest==1){
		resolution = sqrt( ((INT_MAX/(3 * 60 * 60)) / ratio.a) / ratio.b ) ;
	}
	
	frame = get_frame( ratio, resolution );
	printf("%s\n", n);
	printf("F    %lld\n", (long long unsigned int)(3) * (long long unsigned int)(frame.w) * (long long unsigned int)(frame.w));
	printf("IMAX %lld\n", INT_MAX);
	if( resolution > sqrt( ((INT_MAX/(3 * 60 * 60)) / ratio.a) / ratio.b )  ){
		printf("!!SPECIFIED RESOLUTION TOO  LARGE!!\n    EXITING PROGRAM BECAUSE THE\n!!IMAGE WOULDN'T SAVE EVEN IF RAN!!\n\n");
		exit(0);
	}
	printf("Starting [%dpx, %dpx] (%.5g:%.5g)\n", frame.w, frame.h, ratio.a, ratio.b);
	draw_image( n );

	return 0;
}
//./FractalC 0 1.png 0 0 8 10000 1 0.25 16 9 1 -0.041747 0.699 0 1e2 20 3 10