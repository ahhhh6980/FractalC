#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>
#include <inttypes.h>
#include <time.h> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct {
	double real;
	double imag;
} complex_t;

typedef struct {
	uint16_t w;
	uint16_t h;
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
    uint16_t x;
    uint16_t y;
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

uint8_t convert_hue( int theta )
{
	theta %= 360;
	if( theta >= 300 or theta <= 60 ){ return 255; }
	if( theta < 300 and theta > 240 ){ return ((int)( map_to_range( theta % 60, range_t {0, 60}, range_t {0, 255} ) )); }
	if( theta > 60  and theta < 120 ){ return ((int)( 255 - map_to_range( theta % 60, range_t {0, 60}, range_t {0, 255} ) )); }
	return 0;
}

rgba_t hsv_to_rgb( int h, double s, double v )
{
	rgba_t pureColor = { convert_hue( h ), convert_hue( h - 120 ), convert_hue( h + 120 ), 255 };
	int m = MAX(MAX(pureColor.r,pureColor.g),pureColor.b);
    pureColor.r = (int)( (m - ((m - pureColor.r ) * s )) * v );
    pureColor.g = (int)( (m - ((m - pureColor.g ) * s )) * v );
    pureColor.b = (int)( (m - ((m - pureColor.b ) * s )) * v );
	return pureColor;
}

// Get Frame size
inline bounds_t get_frame( range_t ratio, double res ){
	return { (uint16_t)( res * ratio.a * 60 ), (uint16_t)( res * ratio.b * 60 ) };
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
uint8_t* pixels;
rgba_t rgba;
i_out_t out;

int mode;

inline complex_t c_div_n( complex_t a, int b){
	return { a.real / b, a.imag / b };
}

inline complex_t c_add( complex_t a, complex_t b ){
	return { a.real+b.real , a.imag+b.imag };
}

inline complex_t c_mult( complex_t a, complex_t b ){
	return { a.real * b.real - ( a.imag * b.imag ), a.imag * b.real + b.imag * a.real };
}

inline complex_t n_pow_c( double n, complex_t c ){
	return c_mult( complex_t { cos( c.imag * log( n ) ), sin( c.imag * log( n ) ) } ,complex_t { pow(n,c.real) , 0 } );
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
	complex_t pb = n_pow_c( exp(1), c_mult(  c_mult( complex_t {0,1}, b)  , complex_t { atan( b.imag / a.real ), 0 } ) );
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

inline complex_t mandelbrotCompute( complex_t z, complex_t c){
	return c_add( c_mult( z, z ), c );
}

// Compute pixel
i_out_t compute( pixel_t pixel )
{

	float d = 1e20;

	uint16_t i = 0;
	//pixel_t pixel = rgba_to_pixel( color );

	complex_t c = { map_to_range( pixel.x, range_t { 0, (double)(frame.w) }, xRange), map_to_range( pixel.y, range_t { 0, (double)(frame.h) }, yRange) };
	complex_t z = c;
	
	double p = sqrt( (c.real - 0.25)*(c.real - 0.25) + c.imag*c.imag );

	if( (mode==1) or (c.real > p - 2*p*p + 0.25) && ((c.real+1)*(c.real+1) + c.imag*c.imag  > 0.0625)  ){
		while(i < limit){
			//z = {abs(z.real), abs(z.imag)};
			if(mode==1){
				z = { abs(z.real), abs(z.imag) };
				z = mandelbrotCompute( z, c );
			} else {
				z = mandelbrotCompute( z, c );
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
	vbv = ( double)(frame.w * frame.h) / 2;
	xRange = { ( -1 / zoom ) + position.real, ( 1 / zoom ) + position.real };
	yRange = { ratio.b/ratio.a *( (-1 / zoom ) + position.imag ), ratio.b/ratio.a * (( 1 / zoom ) + position.imag )};
	pixels = new uint8_t[frame.w * frame.h * 3];
	rgba = {0,0,0,0};
	out = {0,0};
	double v = 0.0;
	int index = 0;
	for(int y = 0; y<frame.h; y++){
		for(int x = 0; x < frame.w; x++){
			for(double fy = 0.0; fy < logs; fy++ ){
				if( iterator / vbv == fy/logs ){
					printf("#%d: %d%%, @%lds\n", (int)( fy + 1 ), (int)((iterator / vbv ) * 100), time(NULL)-start );
				}
			}
			out = compute( pixel_t { x, y } );
			if( out.i==-1 ){
				pixels[index++] = 0;
   				pixels[index++] = 0;
   				pixels[index++] = 0;
			} else {
				if(fancyColors){
					v = scale_to_range( out.i, range_t {0.0, (double)(limit)}, range_t {0.0, 360}, exponent*out.d);
				} else {
					v = scale_to_range( out.i, range_t {0.0, (double)(limit)}, range_t {0.0, 360}, exponent);
				}

				rgba = hsv_to_rgb( v, 1, 1 );
			
				pixels[index++] = 255 - rgba.r ;
   				pixels[index++] = 255 - rgba.g ;
   				pixels[index++] = 255 - rgba.b ;
				
			}
			iterator++;
		}
	}
	printf("%lf\n",out.d);
	printf("Finished in %ld seconds\n",time(NULL)-start);
	stbi_write_png(name, frame.w, frame.h, 3, pixels, frame.w * 3);

	//unsigned char* rgb = stbi_load( "myimage.png", &width, &height, &bpp, 3 );
	//printf("%d\n", output.i);

}

int main(int argc, char *argv[])
{	

	char* name = "new.png";
	if( argc >= 1 ){
		mode = atoi(argv[1]);
		printf("%d\n",mode);
	} else {
		mode = 0;
	}

	if( argc >= 6 ){
		char* name = argv[2];
		position = { strtod(argv[3], NULL), strtod(argv[4], NULL) };
		zoom = strtod(argv[5], NULL);
		limit = atoi(argv[6]);
	} else {
		position = { -0.75, 0 };
		zoom = 0.7;
		limit = 2500;
	}
	
	if( argc == 9 ){
		resolution = strtod(argv[7], NULL);
		ratio = { strtod(argv[8], NULL), strtod(argv[9], NULL) };
	} else {
		resolution = 2.0;
		ratio = { 4.0, 3.5 };
	}
	
	exponent = 0.25;
	fancyColors = 0;
	zLimit = 100000;
	logs = 10;
	frame = get_frame( ratio, resolution );

	printf("Starting [%dpx, %dpx] (%lf:%lf)\n", frame.w, frame.h, ratio.a, ratio.b);
	draw_image( name );

	return 0;

}