// FractalC Pv0.6b-2
// ahhhh6980
// Last modified Sun Nov 15 1:59 AM EST

#include <iostream>
#include <fstream>
#include <cmath>
#include <tgmath.h>
#include <string>
#include "Range.hpp"
#include "Complex.hpp"
#include <time.h> 
#include <thread>
#include <future>
#include <vector>
#include <sstream>

// Libraries needed to be able to create image output
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define MIN(a,b) ( ( a < b ) ? a : b )
#define MAX(a,b) ( ( a > b ) ? a : b )

// Useful Constants
#define CONST_E exp( 1 )
#define CONST_PI acos( -1 )
#define CONST_PHI ((sqrt(5) + 1) / 2)

// Struct used in computing/processing the dimensions of the image
typedef struct {
	uint32_t w;
	uint32_t h;
} bounds_t;

// Important for colors (obviously)
// Alpha isnt actually used
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba_t;

// Useful for passing around a pixel coordinate
typedef struct {
    uint32_t x;
    uint32_t y;
} pixel_t;

// Data passed out of compute(), i is iterations, d is furthest distance
typedef struct {
	int i;
	double d;
} i_out_t;

// HSV is used because its easier to make looping colors
// I compute the HSV then convert to RGB
typedef struct {
	double h;
	double s;
	double v;
} hsv_t;

// Important for passing around data to the threads
typedef struct {
    Range<uint16_t> r;
    uint8_t * p;
    int * neb;
} pp_t;

// This is declared outside of convert_hue to prevent it from constantly recreating the value
Range<double> r1 = {0, 60};
Range<double> r2 = {0, 255};

// Takes an angle as input and outputs an RGB value, I made this from scratch
inline uint8_t convert_hue ( int theta )
{
	theta %= 360;
	if ( theta >= 300 || theta <= 60 ) return 255; 
	if ( theta < 300 && theta > 240 )  return ( (int)(r1.mapLin( theta % 60, r2 )) ); 
	if ( theta > 60  && theta < 120 )  return ( (int)(255 - r1.mapLin( theta % 60, r2 )) ); 
	return 0;
}

// Used to apply a saturation and value modifier to a pure RGB, I made this from scratch
inline rgba_t revalue( rgba_t color, double s, double v )
{
    int m = MAX( MAX(color.r, color.g), color.b );
    return rgba_t {
        (int)( (m - ((m - color.r ) * s )) * v ),
        (int)( (m - ((m - color.g ) * s )) * v ),
        (int)( (m - ((m - color.b ) * s )) * v ),
        255  
    };
}

// The entire HSV to RGB function, I made this from scratch
inline rgba_t hsv_to_rgb ( int h, double s, double v )
{
	return revalue (rgba_t {convert_hue( h ), convert_hue( h - 120 ), convert_hue( h + 120 ), 255 }, s, v );
}

// Gets the correct Frame size
inline bounds_t get_frame( Range<double> ratio, double res )
{
	return { (uint64_t)( res * ratio.A() * 60 ), 
             (uint64_t)( res * ratio.B() * 60 )  };
}

// Multiplier for resolution formula
double resolution;
// Scalar for resolution
Range<double> ratio;
// The image size itself
bounds_t frame;

// The coordinate space is divided by this value
double zoom ;
// The complex coordinate at the center of the image
Complex<double> position;
// Initialization parameters for z
uint8_t set_zInit;
Complex<double> zInit;
// The angle in degrees that the complex plane should be rotated by
double theta;

// Used for computing the coordinate space along X axis and Y axis
Range<double> xRange;
Range<double> yRange;

// Specifies the kind of fractal to generate
int mode;
// Specifies if the special rendering mode ought to be used
int rMode;

// Iteration limit
uint16_t limit;
// Number Size Limit
double zLimit;

// If true, colors depend on the distance a point gets away from another point
bool orbitTrap;
int orbitMode;
// The point to check
Complex<double> orbitTrapPos;

// Modifier for the curve of the exponential interpolation used to color the data
double exponent;

// ???
double logs;
double e;

// Coordinate for the Julia Fractal, if specified
Complex<double> jCoord;
int isJulia;

// Offsets the hue
double cOffset;
// Scalar for the reoccurance of color
double cMult;

// Stores filename
char* n;

// Supersampling specification
int supersampling;
double ssDV;

int fadeDark;
int fade;
// Scalar for the fade
double lightnessScale;
// Exponent for the fade
double lightnessExponent;
// Use to fix a color issue with fade
int presub;

// Specifies if the image should be black and white instead
// It is an int because I planned on adding several modes but idk
int monochrome;

// Temporary
int tempSv;

// Flag variable for if the program should adjust the image dimensions to be 
// maximum and still fit the ratio
int largest;

// Specifies if colors should be computed as 255-C
int invert;

// Specifies if the exclusion of the main cardioid and bulb in the mandelbrot set should be computed anyways
int exclude;

// Stuff to warp the complex plane for cool mirroring effects
int coordSpace;
double coordSpaceExp;
int coordSpaceC;
Complex<double> coordSpaceExpC;

// Info on texture
int tW;
int tH;
int tC;
unsigned char* tImage;
int useTexture;
bounds_t texOffset;
bounds_t texScale;
double texRot;
Complex<double> texRadialPos;
int setTexPos;
// Texture name
char* tN;

// Used for multithreading data?
int s;
pp_t inp;

// Compute pixel
i_out_t compute( Complex<double> c )
{   
    double p = 2.0;
    // Closest Distance Variable
	float d = 1e20;
    // Iterator
	uint16_t i = 0;
    
    // Rotate the coordinate by defined theta
    if(theta!=0){
        c = c - position;
        c = c.rotate(theta) + position;
    }
    
    // Switch case for normal warping of the complex plane
	switch(coordSpace){
		case 1:
            c = (1.0/c).power(coordSpaceExp);
			break;
		case 2:
			c = 1.0 / (( 1.0/c ).power(coordSpaceExp));
			break;
	}

    // Switch case for complex warping of the complex plane
	switch(coordSpaceC){
		case 1:
			c = ( 1.0/c ).power(coordSpaceExpC) ;
			break;
		case 2:
			c = 1.0 / (( 1.0/c ).power(coordSpaceExpC)) ;
			break;
	}

    // Our main point of computation
    Complex<double> z;
    if(set_zInit==1){
        z = zInit;
    } else {
        z = c;
    }

    // Change the addition coordinate to a static coord for a Julia
	if(isJulia==1) c = jCoord;

    // Approximates the main cardioid and bulb of the mandelbrot set
    //p = 0.0
	if( mode == 0 ) p = sqrt( (c.real() - 0.25)*(c.real() - 0.25) + c.imag()*c.imag() );
	if( (exclude==0) || (mode!=0) || ((c.real() > p - 2*p*p + 0.25) && ((c.real()+1)*(c.real()+1) + c.imag()*c.imag()  > 0.0625)  )){
		while(i < limit){

            // Switch case for fractal type
			switch(mode){
				case -1:
                    // Debugging
					return { -1, d };
				case 0:
                    // Mandelbrot
					z = z*z + c;
					break;
				case 1:
                    // Burning Ship
					z = z.absC();
					z = z*z + c;
					break;
				case 2:
                    // Spade Fractal
					z = z.power(z) + ( z / c );
					break;
                case 3:
                    // Spider Fractal
					z = z*z + c;
                    c = c/2.0 + z;
					break;
                case 4:
                    // newton Fractal
                    z = z - (z.power(3.0)-c) / (3.0*z.power(2.0));
                    break;
                case 5:
                    // Weird fractal A
                    z = (1.0/z).power(2) + c;
                    break;
                case 6:
                    // Weird Fractal B
                    z = (z*z) + c + z.root(abs(c.real())) + z.root(abs(c.imag()));
                    break;
                case 7:
                    // Good for Julia Fractals
                    z = (z*z) + c + z.root(c);
                    break;
                case 8:
                    // Weird Fractal C (Good for Julia Fractals I guess)
                    z = (z).power(c+6.0) + c;
                    break;
                case 9:
                    // Very Bizarre Fractal, but detail isnt good
                    z = z.power(c.power(z.tanC())) +z.power(c.power(z.cosC())) +z.power(c.power(z.sinC())) + c;
                    break;
                case 10:
                    // Another Very Bizarre Fractal
                    z = z.power(z.root(c) + c.root(z)) + c;
                    break;

			}

            // Switch case to determine rendering style
            switch(rMode){
                case 1:
                    if( z.real()*z.real() > zLimit || z.imag()*z.imag() > zLimit ){
				        return { i, d }; 
			        }
                    break;
                case 2:
                    if( abs( z.power(z).real() ) > zLimit || abs( z.power(z).imag() ) > zLimit ){
				        return { i, d }; 
			        }
                    break;
            }

			// Orbit Trap computation
			if(orbitTrap){ 
				switch(orbitMode){
					case 1:
						d = MIN( abs( z.mod() - (z - orbitTrapPos).mod() ), d );
						break;
					case 2:
						d = MIN( abs( (z - orbitTrapPos).mod() ) , d );
						break;
					case 3:
						d = MIN( abs( z.mod() - orbitTrapPos.mod() ), d );
						break;
					case 4:
						d = MIN( abs( z.mod() - (c - orbitTrapPos).mod() ), d );
						break;
					case 5:
						d = MIN( abs( (c - orbitTrapPos).mod() ), d );
						break;
				}
			}
            ++i;
		}
	} else { return { -1, d }; }
	return { -1, d };
}

// Keeps track of how many segments have been computed
int segCount = 0;
// Keeps track of how many threads have finished
int thCount = 0;

// Returns the pixel at (x,y) of imageData
rgba_t getPixel( int x, int y, unsigned char* imageData, int w, int h, int c ){
	int indexP = c*((x%w) + ((y%h)*h));
	return { imageData[indexP], imageData[indexP+1], imageData[indexP+2] };
}

// Coloring ranges


// The process of actually generating the color data to be written
void draw_image( pp_t input )
{   
    Range<double> range1 = {0.0, (double)(limit)};
    Range<double> range2 = {0.0, 360};
    
    // temporary fungible color value
    rgba_t cColor;
    // The vertical scope assigned to this function call
    Range<uint16_t> range = input.r;
    // The array of pixel colors
    uint8_t* pixels = input.p;
    // Used to move around and compute the actual color of the pixels
	rgba_t rgba = {0,0,0,0};
    // Used to store pixel computation results as they are used multiple times
	i_out_t out = {0,0};
    i_out_t tempCalc = {0,0.0};

    // Position iterator
	uint64_t index = frame.w*range.A()*3;

    // very uncreative variable name because I have too many temporary variables
    double v = 0.0;
    // variable that only exists to try and make the code shorter/compute less things
	double valueChange;

    // Wow another temporary variable
	Complex<double> tempV;

    double dvx = Range<double>{frame.w}.mapLin( 2, xRange ) - Range<double>{frame.w}.mapLin( 1, xRange );
    double dvy = Range<double>{frame.h}.mapLin( 2, yRange ) - Range<double>{frame.h}.mapLin( 1, yRange );

    Complex<double> sscD = {abs(dvx/ssDV), abs(dvy/ssDV)};

	for(uint16_t y = range.A(); y < range.B(); ++y){
		for(uint16_t x = 0; x < frame.w; ++x){

             // The pixel mapped to its approximate position on the complex plane
	        Complex<double> c = { Range<double>{ frame.w }.mapLin( x, xRange ), 
                                  Range<double>{ frame.h }.mapLin( y, yRange ) };

            switch(supersampling){
                case 0:
                    out = compute( c );
                    break;
                case 1:

                    out = compute( c );

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    out.i = out.i / 3;
                    out.d = out.d / 3.0;
                    break;
                    
                case 2:

                    out = compute( c );

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    out.i = out.i / 5;
                    out.d = out.d / 5.0;

                    break;

                case 3:

                    out = compute( c );

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    out.i = out.i / 9;
                    out.d = out.d / 9.0;

                    break;
                case 4:
                    out = compute( c );

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - sscD.real(),c.imag() + sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() + sscD.real(),c.imag() - sscD.imag()} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - abs(sscD.real() * cos(45)),c.imag() - abs(sscD.imag() * sin(45))} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() + abs(sscD.real() * cos(45)),c.imag() + abs(sscD.imag() * sin(45))} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() - abs(sscD.real() * cos(45)),c.imag() + abs(sscD.imag() * sin(45))} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    tempCalc = compute( Complex<double>{c.real() + abs(sscD.real() * cos(45)),c.imag() - abs(sscD.imag() * sin(45))} );
                    out.i = out.i + tempCalc.i;
                    out.d = out.d + tempCalc.d;

                    out.i = out.i / 13;
                    out.d = out.d / 13.0;
            }
			
			if( out.i == -1 ){
                // set to black
				pixels[index++] = 0;
   				pixels[index++] = 0;
   				pixels[index++] = 0;
			} else {

                // adjust for orbit trap
				if(orbitTrap){
					v = range1.mapExp( out.i, range2, exponent*out.d);
				} else {
					v = range1.mapExp( out.i, range2, exponent);
				}

                // adjust for the fade
				switch(fade){
					case -1:
						valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						rgba = hsv_to_rgb( (v * cMult) + cOffset , 1, 1 - Range<double>{limit}.mapExp(valueChange, Range<double>{}, 0.5 ) );
						break;
					case 0:
						rgba = hsv_to_rgb( (v * cMult) + cOffset, 1, 1 );
						break;
					case 1:
						valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						rgba = hsv_to_rgb( (v * cMult) + cOffset , 1, Range<double>{limit}.mapExp(valueChange, Range<double>{}, 0.5 ) );
						break;
				}

                // Conditionals for texture usage
				if(useTexture==1){
                    
                    // Assign temp to complex coordinates of pixel
					tempV = c;
                    
                    // Adjust texture for screen position and rotation
                    if(theta!=0){
                        tempV = tempV - position;
                        tempV = tempV.rotate(theta) + position;
                        tempV = tempV - texRadialPos;
                    }

                    // coord space warping for the texture
					switch(coordSpace){
						case 1:
							tempV = ( 1.0/tempV ).power(coordSpaceExp); ;
							break;
						case 2:
							tempV =  1.0 / ( 1.0/tempV ).power(coordSpaceExp) ;
							break;
					}

					switch(coordSpaceC){
						case 1:
							tempV = ( 1.0/tempV ).power(coordSpaceExpC); ;
							break;
						case 2:
							tempV =  1.0 / ( 1.0/tempV ).power(coordSpaceExpC);
							break;
					}

					//cColor = getPixel( (int)(map_to_range( atan(tempV.imag / abs(tempV.real)) , range_t { -2 * CONST_PI, 2*CONST_PI }, range_t { 0, tW } )), (int)(map_to_range( out.i, range_t { -2, modulus(tempV) }, range_t { 0.0, 5 } )), tImage, tW, tH, tC );
					// I'm sure theres a better way to write this
                    Range<double> baseR;

                    int tgX = texOffset.w + (int)( texOffset.w + (baseR*360.0).mapLin( 
                        (int) ((atan2(tempV.imag(), tempV.real()) * (180/CONST_PI) + 180 + texRot) * texScale.w) % 360, baseR*(double)(tW)) );

                    int tgY = texOffset.h + (int)( texOffset.h + (baseR*((double)(limit)+1.0)-1.0).mapExp( 
                        (int)( out.i * texScale.h * ((orbitTrap) ? out.d : 1) ) % limit, baseR * (double)(tH) * cMult, exponent) );
                    
                    cColor = getPixel( tgX, tgY, tImage, tW, tH, tC );
                    
                    // Fade computation
                    switch(fade){
					    case -1:
						    valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
						    cColor = revalue( cColor, 1, 1 - Range<double>{limit}.mapExp(valueChange, Range<double>{} , 0.5 ) );
						    break;
					    case 0:
						    break;
					    case 1:
						    valueChange = lightnessScale * pow((double)(out.i), lightnessExponent);
                            cColor = revalue( cColor, 1, Range<double>{limit}.mapExp(valueChange, Range<double>{} , 0.5 ) );
						    break;
				    }

                    // Texture Black and white computation, and/or inverted computation
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

                //Black and white computation, and/or inverted computation
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
		}
	}

    thCount+=1;

    std::stringstream outLog;

    outLog << "Thread #";
    outLog << thCount;
    outLog << " Finished! IMAGE #";
    outLog << segCount;
    outLog << "\n";
    
    printf((char*) outLog.str().c_str());
}

// $ pix[(int)(xRange.mapLin(z.real(), frame.w))*(int)(yRange.mapLin(z.imag(), frame.h))]+=1;

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
    {"-tName", 32},
    {"-mt", 33},
    {"-split", 34},
    {"-zInit", 35},
    {"-ss", 36},
    {"-ssDV", 37}
};

// Command lookup
int get_key( const char *key ){
	for( int i = 0; i < 37; ++i ){
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
    {"p", 6},
    {"width", 7}
};

// Resolution shortcut lookup
int get_res_key( const char *key ){
	for( int i = 0; i < 7; ++i ){
		if( !strcmp(key, check_res_key[i].key) )
			return check_res_key[i].v;    
	}
	return 0;
}

int main(int argc, char *argv[])
{	
    // So many right? feel free to give suggestions on how to do this better
	n = "generatedFractal";
    tN = "texture";
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
	coordSpaceExp = 1;
	coordSpaceExpC = {1.0, 0};
    useTexture = 0;
    texRadialPos = {0, 0};
    texScale = {1, 6};
    setTexPos = 1;    
    pp_t inp;
    rMode = 1;
    s = 3;
    set_zInit = 0;
    zInit = {0,0};
    supersampling = 0;
    ssDV = 3.0;
    int factor = 1;
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
						resolution = ( 1920.0 / 60.0 ) / ratio.A();
						break;
					case 2:
						resolution = ( 3840.0 / 60.0 ) / ratio.A();
						break;
					case 3:
						resolution = ( 7680.0 / 60.0 ) / ratio.A();
						break;
					case 4:
						resolution = ( 15360.0 / 60.0 ) / ratio.A();
						break;
					case 5:
						largest = 1;
						break;
                    case 6:
						resolution = ( 480.0 / 60.0 ) / ratio.A();
						break;
                    case 7:
						resolution = ( (double) atoi(argv[i+2]) / 60.0 ) / ratio.A();
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
            case 33:
                // if m specified, make it maximum
                if(strcmp("m",argv[i+1])==1){
                    s = std::thread::hardware_concurrency();
                } else {
                    s = atoi(argv[i+1]);
                }
                break;
            case 34:
                factor = atoi(argv[i+1]);
                break;
            case 35:
                set_zInit = 1;
                zInit = {strtod(argv[i+1], NULL), strtod(argv[i+2], NULL)};
                break;
            case 36:
                supersampling = atoi(argv[i+1]);
                break;
            case 37:
                ssDV = strtod(argv[i+1], NULL);
                break;
		}
	}
    // Texture
    std::stringstream tName;
    tName << tN;
    tName << ".jpg";
	tImage = stbi_load( (char*) tName.str().c_str(), &tW, &tH, &tC, STBI_rgb);
	printf( "%d, %d, %d\n", tW, tH, tC );
	printf("\n");
    if(setTexPos==0){
        texRadialPos = position;
    }

	if( fade!=0 && fadeDark==1 ){ presub = 0; }

	frame = get_frame( ratio, resolution );

    if(largest==1){
		frame = { (int)((ratio.A()*sqrt((double)(INT_MAX)))/(ratio.A()+ratio.B())), 
                  (int)((ratio.B()*sqrt((double)(INT_MAX)))/(ratio.A()+ratio.B())) };
	}

	if( frame.w+frame.h > sqrt(INT_MAX) ){
        std::cout << " Frame W: " << frame.w << " | " << " Frame H: " << frame.h << std::endl;
        std::cout << " resolution: " << resolution << " | " << " factor: " << factor << std::endl;
        int factorAdd = ceil( (double)(frame.w+frame.h) / sqrt((double)(INT_MAX)) );
        resolution /= factorAdd;
        factor *= factorAdd;
        frame = get_frame( ratio, resolution );
        std::cout << " Frame W: " << frame.w << " | " << " Frame H: " << frame.h << std::endl;
        std::cout << " FactorAdd: " << factorAdd << " | " << " resolution: " << resolution << " | " << " factor: " << factor << std::endl;
	}
    time_t programStart = time(NULL);  
    for (int seg = 0; seg < (factor*factor) ; ++seg ){

        segCount += 1;

        thCount = 0;

        time_t start = time(NULL);
        //uint32_t pixelCheck[frame.w][frame.h];
	    printf("Starting [%dpx, %dpx] (%.5g:%.5g)\n", frame.w, frame.h, ratio.A(), ratio.B());

        uint8_t* pixArray = (uint8_t* ) malloc((uint64_t)(frame.w) * (uint64_t)(frame.h) * (uint64_t)(3) * sizeof(uint8_t));
    
        uint32_t maxNeb = 0;

        //int nebPix[frame.w][frame.h];

        xRange = { -1, 1 };
        yRange = { -(ratio.B() / ratio.A()), 
                     ratio.B() / ratio.A() };
        
        xRange = xRange / zoom;
        yRange = yRange / zoom;
        
        xRange += {position.real(), position.real()};
        yRange += {position.imag(), position.imag()};

        Range<double> tempX = {0, factor};
        Range<double> tempY = {0, factor};
	    
        xRange = { tempX.mapLin( seg%factor, xRange ), 
                   tempX.mapLin( seg%factor + 1, xRange ) };

        yRange = { tempY.mapLin( seg/factor, yRange ), 
                   tempY.mapLin( seg/factor + 1, yRange ) };

        printf("\nX RANGE: {%lf, %lf}\n", xRange.A(), xRange.B());
	    printf("Y RANGE: {%lf, %lf}\n\n", yRange.A(), yRange.B());

        std::vector<std::thread> threads;
        for ( int it = 0 ; it < s; ++it){
            if( it == s-1 ){
                inp = { Range<uint16_t>{ it * (frame.h/s), frame.h }, pixArray };
            } else {
                inp = { Range<uint16_t>{ it * (frame.h/s), (it+1) * (frame.h/s) }, pixArray };
            }
            printf("Starting Thread %d with Range: [%d, %d]\n",it+1, inp.r.A(),inp.r.B());
            threads.push_back(std::thread( &draw_image, inp ));

        }

        for ( int i = 0 ; i < s ; ++i){
            threads.at(i).join();
        }

        printf("\nplease wait while it's saving... @%lds\n", time(NULL) - start);
        std::stringstream nameIn;
        nameIn << "output/";
        nameIn << n;
        
        if(factor>1){

            nameIn << "_S";

            // Dumb but it works, might fix later
            // is barely an impact on performance
            if(factor*factor > 9 && seg < 9 )
                nameIn << "0";
            if(factor*factor > 99 && seg < 99 )
                nameIn << "0";
            if(factor*factor > 999 && seg < 999 )
                nameIn << "0";
            if(factor*factor > 9999 && seg < 9999 )
                nameIn << "0"; 

            nameIn << (seg+1);

        }
        nameIn << ".png";
        stbi_write_png( (char*) nameIn.str().c_str(), frame.w, frame.h, 3, pixArray, frame.w * 3);

	    printf("\nFinished and saved @%lds\n", time(NULL) - start );
	    free(pixArray);
    
        //th2.join();
        if(factor>1){
	        printf("\nPixel Array is now free from memory\nIt is now safe to close the program\n\n");
        }
    }
    if(factor>1){
        printf("\nFinished and saved @%lds\n", time(NULL) - programStart );
        printf("\nPixel Arrays are free from memory\nIt is safe to close the program\n\n");
    }
    std::stringstream commandOutput;

    for( int i = 0; i < argc; ++i ){
        commandOutput << argv[i];
        commandOutput << " ";
    }

    std::ofstream commandLog;
    std::time_t timeStamp = std::time(nullptr);

    commandLog.open ("commandLog.txt", std::fstream::app);
    commandLog << "\n\n";
    commandLog << std::asctime(std::localtime(&timeStamp));
    commandLog << (char*) commandOutput.str().c_str();
    commandLog.close();

	return 0;
}
