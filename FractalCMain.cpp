#include <iostream>
#include <cmath>
#include <tgmath.h>
#include <string>
#include "Range.hpp"
#include "Complex.hpp"
#include <time.h> 
#include <thread>
#include <future>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define MIN(a,b) ( ( a < b ) ? a : b )
#define MAX(a,b) ( ( a > b ) ? a : b )



#define CONST_E exp( 1 )
#define CONST_PI acos( -1 )
#define CONST_PHI ((sqrt(5) + 1) / 2)

typedef struct {
	uint32_t w;
	uint32_t h;
} bounds_t;

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

typedef struct {
    Range<uint16_t> r;
    uint8_t * p;
} pp_t;


Range<double> r1 = {0, 60};
Range<double> r2 = {0, 255};

inline uint8_t convert_hue( int theta )
{
	theta %= 360;
	if( theta >= 300 || theta <= 60 ){ return 255; }
	if( theta < 300 && theta > 240 ){ return ((int)( r1.mapLin( theta % 60, r2 ) )); }
	if( theta > 60  && theta < 120 ){ return ((int)( 255 - r1.mapLin( theta % 60, r2 ) )); }
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
inline bounds_t get_frame( Range<double> ratio, double res ){
	return { (long long unsigned int)( res * ratio.A() * 60 ), (long long unsigned int)( res * ratio.B() * 60 ) };
}

double resolution;
Range<double> ratio;
bounds_t frame ;

double zoom ;

Complex<double> position;

Range<double> xRange;
Range<double> yRange;

uint16_t limit;
double zLimit;

bool orbitTrap;

double exponent;
double logs;
double e;

int mode;
Complex<double>  jCoord;
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

Complex<double> orbitTrapPos;


int coordSpace;
double coordSpaceExp;
int coordSpaceC;
Complex<double> coordSpaceExpC;

int tW;
int tH;
int tC;
unsigned char* tImage;
rgba_t cColor;
int useTexture;

int rMode;

double theta;

bounds_t texOffset;
bounds_t texScale;
double texRot;
Complex<double> texRadialPos;
int setTexPos;

char* tN;

int s;
pp_t inp;

// Compute pixel
i_out_t compute( pixel_t pixel )
{

	float d = 1e20;

	uint16_t i = 0;
	
    
	Complex<double> c = { Range<double>{ frame.w }.mapLin( pixel.x, xRange ), Range<double>{ frame.h }.mapLin( pixel.y, yRange ) };
    
    if(theta!=0){
        c = c - position;
        c = c.rotate(theta) + position;
    }
    
	switch(coordSpace){
		case 1:
			c = ( 1.0/c ).power(coordSpaceExp);
			break;
		case 2:
			c = 1.0 / ( 1.0/c ).power(coordSpaceExp);
			break;
	}

	switch(coordSpaceC){
		case 1:
			c = ( 1.0/c ).power(coordSpaceExpC) ;
			break;
		case 2:
			c = 1.0 / ( 1.0/c ).power(coordSpaceExpC) ;
			break;
	}

	

	Complex<double> z = c;

	if(isJulia==1){
		c = jCoord;
	}

	double p = sqrt( (c.real() - 0.25)*(c.real() - 0.25) + c.imag()*c.imag() );

	if( (exclude==0) || (mode!=0) || ((c.real() > p - 2*p*p + 0.25) && ((c.real()+1)*(c.real()+1) + c.imag()*c.imag()  > 0.0625)  )){
		while(i < limit){

			switch(mode){
				case -1:
					return { -1, d };
				case 0:
					z = z*z + c;
					break;
				case 1:
					z = z.absC();
					z = z*z + c;
					break;
				case 2:
					z = z.power(z) + ( z / c );
					break;
                case 3:
					z = z*z + c;
                    c = c/2.0 + z;
					break;
                case 4:
                    z = (1.0/z).power(2) + c;
                    break;
                case 5:
                    z = (2*CONST_PI*z).sinC() + c;
                    break;
                case 6:
                    z = (z*z) + c + z.root(abs(c.real())) + z.root(abs(c.imag()));
                    break;
                case 7:
                     z = (z*z) + c + z.root(c);
                     break;
			}

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

			i++;
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
		}
	} else { return { -1, d }; }
	return { -1, d };
}

rgba_t getPixel( int x, int y, unsigned char* imageData, int w, int h, int c ){
	int indexP = c*((x%w) + ((y%h)*h));
	return { imageData[indexP], imageData[indexP+1], imageData[indexP+2] };
}

void draw_image( pp_t input )
{
    Range<uint16_t> range = input.r;
    uint8_t* pixels = input.p;
	double iterator = 0.0;
	double vbv = ((double)(frame.w)*(double)(frame.h));

	rgba_t rgba = {0,0,0,0};
	i_out_t out = {0,0};
    
	uint64_t index = frame.w*range.A()*3;

	Range<double> range1 = {0.0, (double)(limit)};
	Range<double> range2 = {0.0, 360};

    double v = 0.0;
	double valueChange;

	Complex<double> tempV;

	for(uint16_t y = range.A(); y < range.B(); ++y){
		for(uint16_t x = 0; x < frame.w; ++x){

			out = compute( pixel_t { x, y } );
			if( out.i == -1 ){
				pixels[index++] = 0;
   				pixels[index++] = 0;
   				pixels[index++] = 0;
			} else {
				if(orbitTrap){
					v = range1.mapExp( out.i, range2, exponent*out.d);
				} else {
					v = range1.mapExp( out.i, range2, exponent);
				}

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

				if(useTexture==1){

					tempV = { Range<double>{frame.w}.mapLin( x, xRange ), Range<double>{frame.h}.mapLin( y, yRange ) };
                    
                    if(theta!=0){
                        tempV = tempV - position;
                        tempV = tempV.rotate( theta ) + position;
                        tempV = tempV - texRadialPos;
                    }
                    //tempV = c_sub(tempV, rotate_theta(texRadialPos, theta));
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
					if(orbitTrap){
                        cColor = getPixel(  texOffset.w + (int)( texOffset.w + Range<double>{360}.mapLin( (int)((atan2(tempV.imag(),tempV.real()) * (180/CONST_PI) + 180 + texRot) * texScale.w ) % 360, Range<double>{tW} ) ),  texOffset.h + (int)( texOffset.h + (Range<double>{ -1, limit }).mapExp( (int)(out.i * texScale.h * out.d) % limit, Range<double>{tH*cMult}, exponent ) ), tImage, tW, tH, tC );
                    } else {
                        cColor = getPixel(  texOffset.w + (int)( texOffset.w + Range<double>{360}.mapLin( (int)((atan2(tempV.imag(),tempV.real()) * (180/CONST_PI) + 180 + texRot) * texScale.w ) % 360, Range<double>{tW} ) ),  texOffset.h + (int)( texOffset.h + (Range<double>{ -1, limit }).mapExp( (int)(out.i * texScale.h) % limit, Range<double>{tH*cMult}, exponent ) ), tImage, tW, tH, tC );
                    }

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
    {"-tName", 32},
    {"-mt", 33}
};

int get_key( const char *key ){
	for( int i = 0; i < 33; ++i ){
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
    setTexPos = 1;    pp_t inp;
    rMode = 1;
    s = 3;
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
                if(strcmp("m",argv[i+1])==1){
                    s = std::thread::hardware_concurrency();
                } else {
                    s = atoi(argv[i+1]);
                }
		}
	}

    if(setTexPos==0){
        texRadialPos = position;
    }

	if( fade!=0 && fadeDark==1 ){ presub = 0; }
	if(largest==1){
		resolution = sqrt( ((INT_MAX/(3 * 60 * 60)) / ratio.A()) / ratio.B() ) ;
	}
	
	frame = get_frame( ratio, resolution );
	if( resolution > sqrt( ((INT_MAX/(3 * 60 * 60)) / ratio.A()) / ratio.B() )  ){
		printf("!!SPECIFIED RESOLUTION TOO  LARGE!!\n    EXITING PROGRAM BECAUSE THE\n!!IMAGE WOULDN'T SAVE EVEN IF RAN!!\n\n");
		exit(0);
	}
    time_t start = time(NULL);
    //uint32_t pixelCheck[frame.w][frame.h];
	printf("Starting [%dpx, %dpx] (%.5g:%.5g)\n", frame.w, frame.h, ratio.A(), ratio.B());

    uint8_t* pixArray = (uint8_t* ) malloc((uint64_t)(frame.w) * (uint64_t)(frame.h) * (uint64_t)(3) * sizeof(uint8_t));
    
    xRange = {  ( -1 / zoom ) + position.real(), ( 1 / zoom ) + position.real() };
	yRange = {  ( ( -(ratio.B()/ratio.A()) / zoom ) + position.imag() ), (( (ratio.B()/ratio.A()) / zoom ) + position.imag() )};

    printf("\nX RANGE: {%lf, %lf}\n", xRange.A(), xRange.B());
	printf("Y RANGE: {%lf, %lf}\n\n", yRange.A(), yRange.B());

    std::vector<std::thread> threads;
    int it = 0;
    for ( it = 0 ; it < s; ++it){
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
	stbi_write_png(n, frame.w, frame.h, 3, pixArray, frame.w * 3);

	printf("\nFinished and saved @%lds\n", time(NULL) - start );
	free(pixArray);
    
    //th2.join();
	printf("\nPixel Array is now free from memory\nIt is now safe to close the program\n\n");

	return 0;
}
