#ifndef __COMPLEX_H_INCLUDED__
#define __COMPLEX_H_INCLUDED__

#include <iostream>
#include <cmath>

//#include "Complex.hpp"

template <typename T>
class Complex 
{
        
        T i, j;

    public:

        Complex () { i = j = T{0}; }

        Complex ( T real, T imag ) 
        {
            i = real;
            j = imag;
        }

        T mod ( void )
        {
	        return i * i + j * j;
        }

        T arg ( void )
        {
            return atan2( j, i );
        }

        Complex<T> absC ( void )
        {
            return Complex<T> { (i >= T{0}) ? i : -i,  (j >= T{0}) ? j : -j };
        }

        Complex<T> conjugate ( void )
        {
            return Complex<T> { i, -j };
        }

        Complex<T> root ( T r=T{2} )
        {
            T sMod = pow( mod(), T{1} / (r*2) );
            T arg = atan( j / i ) / r;
            return Complex<T> { sMod * cos(arg), sMod * sin(arg) };
        }

        Complex<T> root ( Complex<T> r )
        {
            return Complex<T>{ i, j }.power( T{1} / r );
        }

        Complex<T> power ( T p )
        {

            int n = (int)(p);
            Complex<T> current = {this->real(), this->imag()};

            if( n == 1 ){ return current; }
	        Complex<T> temp = current.power( n/2 );
	        if( n % 2 == 0 ){ return temp * temp; }
	        else{ return ( current * ( temp * temp ) );}

        }

		T dot ( Complex<T>& z ) {
			return ((this->real() * z.real()) + (this->real() * z.imag()));
		}

        Complex<T> power ( Complex<T> z )
        {

        	if ( i == T{0} && j == T{0} )
        		return Complex<T> { T{0}, T{0} };
        	if ( z.real() == T{0} && z.imag() == T{0} )
        		return Complex<T> { T{1}, T{0} }; 
        
        	T temp1 = i*i + j*j;
        	T temp2 = z.real() * atan2(j,i) + ( (z.imag() / 2) * log( temp1 ) );
        
        	T va = pow( temp1, z.real() / 2.0 );
        	T vb = exp( -(z.imag()) * atan2(j,i) );
        
        	return (Complex<T> { cos(temp2), sin(temp2) } * (va*vb));   

        }

        Complex<T> lg ( void ){
            return Complex<T> {log(mod())/2, atan( j / i ) };
        }

        Complex<T> sinC( void ){
            return Complex<T> { sin(i)*cosh(j) , cos(i)*sinh(j)};
        }

        Complex<T> cosC( void ){
            return Complex<T> { cos(i)*cosh(j) , sin(i)*sinh(j)};
        }

        Complex<T> tanC( void ){
            return (Complex<T> { sin(i)*cosh(j) , cos(i)*sinh(j)}) / (Complex<T> { cos(i)*cosh(j) , -sin(i)*sinh(j)});
        }

        Complex<T> rotate ( T theta ){
            double angle = atan2(j,i) - ((theta * acos(-1)) / 180.0);
            double m = sqrt(mod());
	        return Complex<T> { m*cos(angle), m*sin(angle) } ;
        }

        void rot ( T theta ){
            double angle = atan2(j, i) - ((theta * acos(-1)) / 180.0);
            double m = sqrt(mod());
	        this->i = m*cos(angle);
            this->j = m*sin(angle);
        } 

        bool operator > ( Complex<T> z )
        {
            if( this->i > z.real() && this->j > z.imag() ){
                return 1;
            } else {
                return 0;
            }
        }

        bool operator < ( Complex<T> z )
        {
            if( this->i < z.real() && this->j < z.imag() ){
                return 1;
            } else {
                return 0;
            }
        }

        bool operator >= ( Complex<T> z )
        {
            if( this->i >= z.real() && this->j >= z.imag() ){
                return 1;
            } else {
                return 0;
            }
        }

        bool operator <= ( Complex<T> z )
        {
            if( this->i <= z.real() && this->j <= z.imag() ){
                return 1;
            } else {
                return 0;
            }
        }

        bool operator == ( Complex<T> z )
        {
            if( this->i == z.real() && this->j == z.imag() ){
                return 1;
            } else {
                return 0;
            }
        }

        bool operator != ( Complex<T> z )
        {
            if( this->i != z.real() || this->j != z.imag() ){
                return 1;
            } else {
                return 0;
            }
        }

        void operator = ( Complex<T> z )
        {
            this->i = z.real();
            this->j = z.imag();
        }

        void operator *= ( T r )
        {
            this->i *= r;
            this->j *= r;
        }

        void operator /= ( T r )
        {
            this->i /= r;
            this->j /= r;
        }

        void operator += ( T r )
        {
            this->i += r;
        }

        void operator -= ( T r )
        {
            this->i -=  r;
        }

        void operator *= ( Complex<T> z )
        {
            Complex<T> temp = { this->i, this->j };
            temp = temp * z;
            this->i = temp.real();
            this->j = temp.imag();  
        }

        void operator /= ( Complex<T> z )
        {
            Complex<T> temp = { this->i, this->j };
            temp = (temp * z.conjugate()) / z.mod();
            this->i = temp.real();
            this->j = temp.imag();  
        }

        void operator += ( Complex<T> z )
        {
            this->i += z.real();
            this->j += z.imag();
        }

        void operator -= ( Complex<T> z )
        {
            this->i -= z.real();
            this->j -= z.imag();
        }

        T real (void) { return i; }
        T imag (void) { return j; }

        void real ( T newReal )  { this->i = newReal; }
        void imag ( T newImag )  { this->j = newImag; }

};

template <typename T>
inline Complex<T> operator * ( Complex<T> a, T b )
{
    return Complex<T> { a.real() * b, a.imag() * b };
}

template <typename T>
inline Complex<T> operator / ( Complex<T> a, T b )
{
    return Complex<T> { a.real() / b, a.imag() / b };
}

template <typename T>
inline Complex<T> operator + ( Complex<T> a, T b )
{
    return Complex<T> { a.real() + b, a.imag() };
}

template <typename T>
inline Complex<T> operator - ( Complex<T> a, T b )
{
    return Complex<T> { a.real() - b, a.imag() };
}

template <typename T>
inline Complex<T> operator * ( T b, Complex<T> a )
{
    return Complex<T> { a.real() * b, a.imag() * b };
}

template <typename T>
inline Complex<T> operator / (T b, Complex<T> a )
{
    return ( a.conjugate() * b ) / a.mod();
}

template <typename T>
inline Complex<T> operator + ( T b, Complex<T> a )
{
    return Complex<T> { a.real() + b, a.imag() };
}

template <typename T>
inline Complex<T> operator - ( T b, Complex<T> a )
{
    return Complex<T> { a.real() - b, a.imag() };
}

template <typename T>
inline Complex<T> operator * ( Complex<T> a, Complex<T> b )
{
    return Complex<T> { a.real() * b.real() - ( a.imag() * b.imag() ), a.imag() * b.real() + b.imag() * a.real() };
}

template <typename T>
inline Complex<T> operator / ( Complex<T> a, Complex<T> b )
{
    Complex<T> temp = b.conjugate() * a;
    return Complex<T> { temp.real() / b.mod(), temp.imag() / b.mod() };
}

template <typename T>
inline Complex<T> operator + ( Complex<T> a, Complex<T> b )
{
    return Complex<T> { a.real() + b.real(), a.imag() + b.imag() };
}

template <typename T>
inline Complex<T> operator - ( Complex<T> a, Complex<T> b )
{
    return Complex<T> { a.real() - b.real(), a.imag() - b.imag() };
}

template <typename T>
Complex<T> absC( Complex<T> z )
{
    T a = z.real();
    T b = z.imag();
    return Complex<T> { (a >= T{0}) ? a : -a,  (b >= T{0}) ? b : -b };
}

#endif