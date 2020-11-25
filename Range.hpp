// Author; ahhhh#6980

#include <iostream>
#include <cmath>
#include <complex>
#include <string>

template <typename T>
class Range {

        T a, b;

        // helper function
        inline T logb( T b, T x )
        {
	        if( x == T{ 0 } || b == T{ 0 } ) { 
                return T{ 0 }; 
            } else { 
                return log(x) / log(b); 
            }
        }

    public:

        // default range
        Range () 
        {
            a = T{ 0 };
            b = T{ 1 };
        }

        Range ( T end ) 
        {
            a = 0;
            b = end;
        }
        
        Range ( T aInput, T bInput ) 
        {
            a = aInput;
            b = bInput;
        }

        // maps INPUT x, to INPUT range r2, LINEAR
        T mapLin ( T x, Range<T> r2 ) 
        {
            if ( x < a ) return r2.A(); 
	        if ( x > b ) return r2.B(); 

            T rA = ( x - a ) / ( b - a );
            T rB = r2.B() - r2.A();

	        return rA * rB + r2.A();
        }

        // maps INPUT x, to INPUT range r2, EXPONENTIAL
        T mapExp ( T x, Range<T> r2, double e ) 
        {
            if ( x < a ) return r2.A(); 
	        if ( x > b ) return r2.B(); 

            T exp = e + logb( x - a, r2.B() - r2.A() );
            T num = pow( x - a, exp );
            T den = pow( b - a, e ) + r2.A();

            return num / den;
        }

        void operator *= ( T b )
        { 
            this->a *= b;
            this->b *= b;
        }

        void operator /= ( T b )
        {
            this->a /= b;
            this->b /= b;
        }

        void operator += ( Range<T> b )
        {
            this->a += b.A();
            this->b += b.B();
        }

        void operator -= ( Range<T> b )
        {
            this->a -= b.A();
            this->b -= b.B();
        }

        // maps INPUT x, to THIS range * scale, LINEAR
        T mapLin ( T x, T scale ) 
        {
            Range<T> r2 = Range<T>{ a, b } * scale;

            if ( x < a ) return r2.A(); 
	        if ( x > b ) return r2.B(); 

            T rA = ( x - a ) / ( b - a );
            T rB = r2.B() - r2.A();

	        return rA * rB + r2.A();
        }

        // maps INPUT x, to THIS range * scale, EXPONENTIAL
        T mapExp ( T x, T scale, double e ) 
        {
            Range<T> r2 = Range<T>{ a, b } * scale;

            if ( x < a ) return r2.A(); 
	        if ( x > b ) return r2.B(); 

            T exp = e + logb( x - a, r2.B() - r2.A() );
            T num = pow( x - a, exp );
            T den = pow( b - a, e ) + r2.A();

            return num / den;
        }

        // return start of range
        T A ( void ) { return a; }
        // return end of range
        T B ( void ) { return b; }

        // returns random number in this range
        // with decimal precision of p
        T random  ( int p=4 ) 
        {
            double precision = pow(10, p);
            Range<T> map = Range<T>{ } * precision;
            return map.mapLin( (double) ((int)rand( ) % (int)precision), Range<T>{ a, b } );
        }

        // returns random number in this range
        // with decimal precision of p
        T randExp ( double e, int p=4 ) 
        {
            double precision = pow(10, p);
            Range<T> map = Range<T>{ } * precision;
            return map.mapExp( (double) ((int)rand( ) % (int)precision), Range<T>{ a, b }, e );
        }

};

template <typename T>
inline Range<T> operator*( Range<T> a, T b )
{
    return Range<T> { a.A() * b, a.B() * b };
}

template <typename T>
inline Range<T> operator / ( Range<T> a, T b )
{
    return Range<T> { a.A() / b, a.B() / b };
}

template <typename T>
inline Range<T> operator + ( Range<T>& a, Range<T>& b )
{
    return Range<T> { a.A() + b.A(), a.B() + b.B() };
}

template <typename T>
inline Range<T> operator + ( Range<T> a, T b )
{
    return Range<T> { a.A() + b, a.B() + b };
}

template <typename T>
inline Range<T> operator - ( Range<T> a, T b )
{
    return Range<T> { a.A() - b, a.B() - b };
}

template <typename T>
inline Range<T> operator - ( Range<T>& a, Range<T>& b )
{
    return Range<T> { a.A() - b.A(), a.B() - b.B() };
}

// prints "RANGE[A, B]"
template <typename T>
std::ostream& operator << ( std::ostream& out, Range<T>& rhs )
{   
    out << "RANGE:[" << rhs.A() << ", " << rhs.B() << "]";
	return out;
}