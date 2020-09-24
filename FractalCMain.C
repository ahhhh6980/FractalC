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

#define CONST_E exp(1)
#define CONST_PI acos(-1)

typedef struct {
	long double real;
	long double imag;
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
	double v;
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

inline rgba_t revalue( rgba_t color, double s, double v ){
    int m = MAX( MAX(color.r, color.g), color.b );
	rgba_t output;
    output.r = (int)( (m - ((m - color.r ) * s )) * v );
    output.g = (int)( (m - ((m - color.g ) * s )) * v );
    output.b = (int)( (m - ((m - color.b ) * s )) * v );
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

bool orbitTrap;

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
int orbitMode;

int exclude;

complex_t orbitTrapPos;


int coordSpace;
double coordSpaceExp;
int coordSpaceC;
complex_t coordSpaceExpC;

int tW;
int tH;
int tC;
unsigned char* tImage;
rgba_t cColor;
int useTexture;

int rMode;

double theta;

bounds_t texOffset;
range_t texScale;
double texRot;
complex_t texRadialPos;
int setTexPos;

char* tN;

inline complex_t c_div_n( complex_t a, double b){
	return { a.real / (double)(b), a.imag / (double)(b) };
}

inline complex_t c_mult_n( complex_t a, double b){
	return { a.real*b, a.imag*b };
}

inline complex_t c_add( complex_t a, complex_t b ){
	return { a.real+b.real , a.imag+b.imag };
}

inline complex_t c_sub( complex_t a, complex_t b ){
	return { a.real - b.real, a.imag - b.imag};
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

inline complex_t n_div_c( double a, complex_t b ){
	return c_div_n( c_mult_n(  conjugate(b), a ) , (b.real*b.real + b.imag*b.imag));
}

// Complex "Modulus" (Distance from 0,0)
inline double modulus( complex_t c ){
	return sqrt( c.real * c.real + c.imag * c.imag );
}

inline double arg( complex_t c ){
	return atan( c.imag / abs(c.real) );
}

inline complex_t rotate_theta( complex_t c, double theta ){
    double angle = atan2(c.imag,c.real) - ((theta * CONST_PI) / 180.0);
    double m = modulus(c);
	return complex_t { m*cos(angle), m*sin(angle) } ;
}



inline complex_t c_pow_c( complex_t a, complex_t b ){

	if ( a.real == 0.0 && a.imag == 0.0 ){
		return { 0, 0 };
	}
	if ( b.real == 0.0 && b.imag == 0.0 ){
		return { 1, 0 }; 
	}
	
	double temp1 = ( a.real*a.real + a.imag*a.imag );
	double temp2 = b.real * arg(a) + ( (b.imag / 2) * log( temp1 ) );

	double va = pow( temp1, b.real / 2 );
	double vb = exp( -b.imag * arg(a) );

	return c_mult_n( complex_t { cos(temp2), sin(temp2) }, va*vb );

}

// Complex power function!
inline complex_t c_pow( complex_t c, uint32_t n )
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
	
    
	complex_t c = { map_to_range( pixel.x, range_t { 0, (double)(frame.w) }, xRange), map_to_range( pixel.y, range_t { 0, (double)(frame.h) }, yRange) };
    
    if(theta!=0){
        c = c_sub(c,position);
        c = c_add(rotate_theta( c, theta ),position);
    }
    
	switch(coordSpace){
		case 1:
			c =  c_pow(n_div_c( 1.0, c ), coordSpaceExp) ;
			break;
		case 2:
			c =  n_div_c( 1, c_pow(n_div_c( 1.0, c ), coordSpaceExp) ) ;
			break;
	}

	switch(coordSpaceC){
		case 1:
			c =  c_pow_c(n_div_c( 1.0, c ), coordSpaceExpC) ;
			break;
		case 2:
			c =  n_div_c( 1, c_pow_c(n_div_c( 1.0, c ), coordSpaceExpC) ) ;
			break;
	}

	

	complex_t z = c;

	if(isJulia==1){
		c = jCoord;
	}

	double p = sqrt( (c.real - 0.25)*(c.real - 0.25) + c.imag*c.imag );

	if( (exclude==0) || (mode!=0) || ((c.real > p - 2*p*p + 0.25) && ((c.real+1)*(c.real+1) + c.imag*c.imag  > 0.0625)  )){
		while(i < limit){

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
					z = c_add( c_pow_c( z, z ), c_div( z, c ) );
					break;
                case 3:
					z = c_add( c_mult( z, z ), c );
                    c = c_add( c_div_n( c, 2.0 ), z );
					break;
                case 4:
                    z = c_add(c_pow(n_div_c(1,z),2), c);
                    break;
			}

            switch(rMode){
                case 1:
                    if( z.real*z.real > zLimit || z.imag*z.imag > zLimit ){
				        return { i, d }; 
			        }
                    break;
                case 2:
                    if( abs(c_pow_c(z,z).real) > zLimit || abs(c_pow_c(z,z).imag) > zLimit ){
				        return { i, d }; 
			        }
                    break;
            }

			i++;
			if(orbitTrap){ 
				switch(orbitMode){
					case 1:
						d = MIN( abs( modulus(z) - modulus( c_sub(z, orbitTrapPos) ) ), d );
						break;
					case 2:
						d = MIN( abs( modulus( c_sub(z, orbitTrapPos) ) ), d );
						break;
					case 3:
						d = MIN( abs( modulus(z) - modulus( orbitTrapPos ) ), d );
						break;
					case 4:
						d = MIN( abs( modulus(z) - modulus( c_sub(c, orbitTrapPos) ) ), d );
						break;
					case 5:
						d = MIN( abs( modulus( c_sub(c, orbitTrapPos) ) ), d );
						break;
				}
			}
		}
	} else { return { -1, d }; }
	return { -1, d };
}

rgba_t getPixel( int x, int y, unsigned char* imageData, int w, int h, int c ){
	int indexP = c*((x%w) + ((y%h)*h));
	return { imageData[indexP], imageData[indexP+1], imageData[indexP+2] };
}

void draw_image( char name[] )
{
	
	
	start = time(NULL);
	iterator = 0.0;
	vbv = ((double)(frame.w)*(double)(frame.h));
	xRange = { ( -1 / zoom ) + position.real, ( 1 / zoom ) + position.real };
	yRange = {  ( (-(ratio.b/ratio.a) / zoom ) + position.imag ), (( (ratio.b/ratio.a) / zoom ) + position.imag )};
	printf("\nX RANGE: {%lf, %lf}\n", xRange.a, xRange.b);
	printf("Y RANGE: {%lf, %lf}\n\n", yRange.a, yRange.b);
	uint8_t* pixels = (uint8_t* ) malloc((uint64_t)(frame.w) * (uint64_t)(frame.h) * (uint64_t)(3) * sizeof(uint8_t));
	rgba = {0,0,0,0};
	out = {0,0};

	double v = 0.0;
	uint64_t index = 0;
	range_t range1 = {0.0, (double)(limit)};
	range_t range2 = {0.0, 360};
	double valueChange;
	double logIncrement = (int)(vbv / (double)(logs));
	double nextLog = logIncrement;
	double currentLog = 0;
	complex_t tempV;
    int averageTime = 0;
    int nextAverage = 0;
    int completeAverage = 0;
	for(uint16_t y = 0; y < frame.h; ++y){
		for(uint16_t x = 0; x < frame.w; ++x){
			if( (float)(index/3) >= nextLog ){
				nextLog += logIncrement;
				currentLog += 1;
                nextAverage = (int)( ( (double)( time(NULL) - start + 0.01) / (double)( currentLog ) ) * (double)(logs) );
                averageTime = ( nextAverage >= averageTime ) ? nextAverage : averageTime ;
                completeAverage = (nextAverage+averageTime+( (double)(time(NULL)-start) * ((double)(logs) / (double)( currentLog ) )))/3;
                         //(int)(((double)(time(NULL)-start + 0.01)/(double)(currentLog))*(double)(logs))
                printf("| log %d | %d%%, @%lds, | ETA @%lds, in %lds |\n", (int)( currentLog ), (int)( ((double)(currentLog) / (double)(logs) ) * 100 ), time(NULL)-start, completeAverage, completeAverage - (time(NULL)-start) );

			}
			out = compute( pixel_t { x, y } );
			if( out.i == -1 ){
				pixels[index++] = 0;
   				pixels[index++] = 0;
   				pixels[index++] = 0;
			} else {
				if(orbitTrap){
					v = scale_to_range( out.i, range1, range2, exponent*out.d);
				} else {
					v = scale_to_range( out.i, range1, range2, exponent);
				}

				switch(fade){
					case -1:
						valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						rgba = hsv_to_rgb( (v * cMult) + cOffset , 1, 1 - scale_to_range(valueChange, range_t { 0, limit }, range_t { 0, 1 }, 0.5 ) );
						break;
					case 0:
						rgba = hsv_to_rgb( (v * cMult) + cOffset, 1, 1 );
						break;
					case 1:
						valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						rgba = hsv_to_rgb( (v * cMult) + cOffset , 1, scale_to_range(valueChange, range_t { 0, limit }, range_t { 0, 1 }, 0.5 ) );
						break;
				}

				if(useTexture==1){

					tempV =  complex_t { map_to_range( x, range_t { 0, (double)(frame.w) }, xRange), map_to_range( y, range_t { 0, (double)(frame.h) }, yRange) };
                    
                    if(theta!=0){
                        tempV = c_sub(tempV, position);
                        tempV = c_add(rotate_theta( tempV, theta ), position);
                        tempV = c_sub(tempV, texRadialPos);
                    }
                    //tempV = c_sub(tempV, rotate_theta(texRadialPos, theta));
					switch(coordSpace){
						case 1:
							tempV =  c_pow(n_div_c( 1.0, tempV ), coordSpaceExp) ;
							break;
						case 2:
							tempV =  n_div_c( 1, c_pow(n_div_c( 1.0, tempV ), coordSpaceExp) ) ;
							break;
					}

					switch(coordSpaceC){
						case 1:
							tempV =  c_pow_c(n_div_c( 1.0, tempV ), coordSpaceExpC) ;
							break;
						case 2:
							tempV =  n_div_c( 1, c_pow_c(n_div_c( 1.0, tempV ), coordSpaceExpC) ) ;
							break;
					}

					//cColor = getPixel( (int)(map_to_range( atan(tempV.imag / abs(tempV.real)) , range_t { -2 * CONST_PI, 2*CONST_PI }, range_t { 0, tW } )), (int)(map_to_range( out.i, range_t { -2, modulus(tempV) }, range_t { 0.0, 5 } )), tImage, tW, tH, tC );
					if(orbitTrap){
                        cColor = getPixel(  texOffset.w + (int)(map_to_range( (int)((atan2(tempV.imag,tempV.real) * (180/CONST_PI) + 180 + texRot) * texScale.a ) % 360 , range_t { 0, 360 }, range_t { 0, tW } )),  texOffset.h + (int)(scale_to_range( (int)(out.i * texScale.b * out.d) % limit , range_t { -1, limit }, range_t { 0.0, tH*cMult }, exponent )), tImage, tW, tH, tC );
                    } else {
                        cColor = getPixel(  texOffset.w + (int)(map_to_range( (int)((atan2(tempV.imag,tempV.real) * (180/CONST_PI) + 180 + texRot) * texScale.a ) % 360 , range_t { 0, 360 }, range_t { 0, tW } )),  texOffset.h + (int)(scale_to_range( (int)(out.i * texScale.b) % limit , range_t { -1, limit }, range_t { 0.0, tH*cMult }, exponent )), tImage, tW, tH, tC );
                    }

                    switch(fade){
					    case -1:
						    valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						    cColor = revalue( cColor, 1, 1 - scale_to_range(valueChange, range_t { 0, limit }, range_t { 0, 1 }, 0.5 ));
						    break;
					    case 0:
						    break;
					    case 1:
						    valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
                            cColor = revalue( cColor, 1, scale_to_range(valueChange, range_t { 0, limit }, range_t { 0, 1 }, 0.5 ) );
						    break;
				    }

                    if(monochrome==1){
					    tempSv = (cColor.r + cColor.g + cColor.b)/3;
                        if(invert==1){ tempSv = 255 - tempSv; }
					    pixels[index++] = tempSv;
   					    pixels[index++] = tempSv;
   					    pixels[index++] = tempSv;
				    } else {
                        if(invert==1){ cColor = { 255 - cColor.r,  255 - cColor.g,  255 - cColor.b }; }
					    pixels[index++] = cColor.r;
   					    pixels[index++] = cColor.g;
   					    pixels[index++] = cColor.b;
				    }

				} else if(monochrome==1){
					tempSv = (abs(rgba.r + presub)+abs(rgba.g + presub)+abs(rgba.b + presub))/3;
                    if(invert==1){ tempSv = 255 - tempSv; }
					pixels[index++] = tempSv;
   					pixels[index++] = tempSv;
   					pixels[index++] = tempSv;
				} else {
                    if(invert==1){ rgba = { 255 - rgba.r,  255 - rgba.g,  255 - rgba.b }; }
					pixels[index++] = abs(rgba.r + presub);
   					pixels[index++] = abs(rgba.g + presub);
   					pixels[index++] = abs(rgba.b + presub);
				}

			}
			iterator++;
		}
	}

	printf("\nplease wait while it's saving... @%lds\n", time(NULL)-start);
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
	{"--isJulia", 8},
	{"-jPos", 9},
	{"-orbitTrap", 10},
	{"-zMax", 11},
	{"-log", 12},
	{"-cOffset", 13},
	{"-cScale", 14},
	{"-name", 15},
	{"--bw", 16},
	{"-lScale", 17},
	{"-lExp", 18},
	{"--fadeDark", 19},
	{"-fade", 20},
	{"--inverted", 21},
	{"--exclude", 22},
	{"-cSpace", 23},
	{"-cSpaceComplex", 24},
    {"--useTexture", 25},
    {"-tScale", 26},
    {"-tOffset", 27},
    {"-tRot", 28},
    {"-tPos", 29},
    {"-rot", 30},
    {"-rMode", 31},
    {"-tName", 32}
};

int get_key( const char *key ){
	for( int i = 0; i < 32; ++i ){
		if( !strcmp(key, check_key[i].key) )
			return check_key[i].v;    
	}
	return 0;
}

const static struct{
	const char* key;
	int v;
} check_res_key [] = {
	{"s", 1},
	{"4k", 2},
	{"8k", 3},
	{"16k", 4},
	{"l", 5},
    {"p", 6}
};

int get_res_key( const char *key ){
	for( int i = 0; i < 6; ++i ){
		if( !strcmp(key, check_res_key[i].key) )
			return check_res_key[i].v;    
	}
	return 0;
}

int main(int argc, char *argv[])
{	
	tImage = stbi_load("useThis.jpg", &tW, &tH, &tC, STBI_rgb);
	cColor = getPixel( 0, 0, tImage, tW, tH, tC );
	printf( "%d, %d, %d\n", tW, tH, tC );
	printf("\n");
	n = "generatedFractal.png";
    tN = "useThis.jpg";
	mode = 0;
	position = { -0.75, 0 };
	zoom = 0.7;
	limit = 2500;
	exponent = 0.25;
	resolution = 2.0;
	ratio = { 4.0, 3.5 };
	isJulia = 0;
	jCoord = {-0.7269, 0.188};
	orbitTrap = 0;
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
	orbitMode = 1;
	orbitTrapPos = { 0, 0 };
	exclude = 1;
	coordSpace = 0;
	coordSpaceExp = 1.0;
	coordSpaceExpC = {1.0, 0};
    useTexture = 0;
    texRadialPos = {0, 0};
    setTexPos = 1;
    texScale = {1, 1};
    theta = 0;
    rMode = 1;
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
				switch( get_res_key( argv[i+1] ) ){
					case 0:
						resolution = strtod(argv[i+1], NULL);
						break;
					case 1:
						resolution = ( 1920.0 / 60.0 ) / ratio.a;
						break;
					case 2:
						resolution = ( 3840.0 / 60.0 ) / ratio.a;
						break;
					case 3:
						resolution = ( 7680.0 / 60.0 ) / ratio.a;
						break;
					case 4:
						resolution = ( 15360.0 / 60.0 ) / ratio.a ;
						break;
					case 5:
						largest = 1;
						break;
                    case 6:
						resolution = ( 480.0 / 60.0 ) / ratio.a;
						break;
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
				orbitTrap = 1;
				orbitMode = atoi(argv[i+1]);
				orbitTrapPos = {strtod(argv[i+2], NULL), strtod(argv[i+3], NULL)};
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
					fade = -1;
				} else if ( strcmp(argv[i+1], "in" ) == 0 ) {
					fade = 1;
				} 
				break;
			case 21:
				invert = 1;
				break;
			case 22:
				exclude = 0;
				break;
			case 23:
				coordSpace = atoi(argv[i+1]);
				coordSpaceExp = strtod(argv[i+2], NULL);
				break;
			case 24:
				coordSpaceC = atoi(argv[i+1]);
				coordSpaceExpC = {strtod(argv[i+2], NULL), strtod(argv[i+3], NULL)};
				break;
            case 25:
                useTexture = 1;
                break;
            case 26:
                texScale = {strtod(argv[i+1], NULL), strtod(argv[i+2], NULL)};
                break;
            case 27:
                texOffset = {atoi(argv[i+1]), atoi(argv[i+2])};
                break;
            case 28:
                texRot = strtod(argv[i+1], NULL);
                break;
            case 29:
                setTexPos = 1;
                texRadialPos = {strtod(argv[i+1], NULL), strtod(argv[i+2], NULL)};
                break;
            case 30:
                theta = strtod(argv[i+1], NULL);
                break;
            case 31:
                rMode = atoi(argv[i+1]);
                break;
            case 32:
                tN = argv[i+1];
                break;
		}
	}

    if(setTexPos==0){
        texRadialPos = position;
    }

	if( fade!=0 && fadeDark==1 ){ presub = 0; }
	if(largest==1){
		resolution = sqrt( ((INT_MAX/(3 * 60 * 60)) / ratio.a) / ratio.b ) ;
	}
	
	frame = get_frame( ratio, resolution );
	if( resolution > sqrt( ((INT_MAX/(3 * 60 * 60)) / ratio.a) / ratio.b )  ){
		printf("!!SPECIFIED RESOLUTION TOO  LARGE!!\n    EXITING PROGRAM BECAUSE THE\n!!IMAGE WOULDN'T SAVE EVEN IF RAN!!\n\n");
		exit(0);
	}

	printf("Starting [%dpx, %dpx] (%.5g:%.5g)\n", frame.w, frame.h, ratio.a, ratio.b);
	draw_image( n );
	
	return 0;
}