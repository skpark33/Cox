/*
 *  Copyright (C) 2014 Jens Thoms Toerring
 *
 *  This file is part of ciAES256.
 *
 *  ciAES256 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 *  ciAES256 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma once
#if ! defined ciByte_Block_hpp_
#define ciByte_Block_hpp_


// The whole thing only works on machines where a char has 8 bits,
// so make sure it's already detected at compile time when that's
// not the case!

#include <climits>
#if CHAR_BIT != 8
#error "CHAR_BIT isn't 8 which is required for the ciByte_Block class"
#endif


#include "ci/libAes256/ciPadding_Type.h"
#include <algorithm>
#include <string>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <ios>


/*---------------------------------------------*
 * Utility class for dealing with blocks of fixed numbers of bytes
 * (the template value) and implementing operations done on them
 * all over the place.
 *---------------------------------------------*/

// size_t를 전부 int로 수정했음.
template< int N >
class ciByte_Block : public ciPadding_Type
{
    template< int M > friend class ciByte_Block;

  public :

    using ciPadding_Type::Padding_Mode;

	// Constructor, sets up all data bytes with padding

    ciByte_Block( )
    {
        pad( 0 );
    }


	// Constructor from an object of different length (and possible with
    // an offset into its data).

    template< int M >
    ciByte_Block( ciByte_Block< M > const & other,
                int                  offset = 0 )
        : m_init_len( N )
    {
        unsigned char const * data = other.m_data + offset;

        if ( offset > M )
            pad( 0 );
        else if ( M - offset >= N )
            std::copy( data, data + N, m_data );
        else
        {
            std::copy( data, data + M - offset, m_data );
            pad( M - offset );
        }
    }


	// Constructor from array of unsigned char of the given lenghth
    // (bytes beyond the length of the object are silently dropped,
    // if there are less bytes padding is appended)

    ciByte_Block( unsigned char const * data,
                int                length = N )
        : m_init_len( N )
    {
        if ( length >= N )
            std::copy( data, data + N, m_data );
        else
        {
            std::copy( data, data + length, m_data );
            pad( length );
        }
    }


	// Constructor from an array of char of the given lenghth or, if
    // no length is supplied (or is 0), from a C string, i.e. stopping
    // at the first '\0' byte. Also here bytes beyond the size of the
    // object are silently dropped and if there are less bytes padding
    // is added.

    ciByte_Block( char const * data,
                int       length = 0 )
        : m_init_len( N )
    {
		if ( length == 0 )
			length = strlen( data );

        if ( length >= N )
            std::copy( data, data + N, m_data );
        else
        {
            std::copy( data, data + length, m_data );
            pad( length );
        }
    }


	// Constructor from a std::string, with an optional offset into
    // its data

    ciByte_Block( std::string const & str,
                int              offset = 0 )
        : m_init_len( N )
    {
        char const * data = str.data( ) + offset;
        int len = (int) str.size( );

        if ( offset >= len )
            pad( 0 );
        else if ( len - offset >= N )
            std::copy( data, data + N, m_data );
        else
        {
            std::copy( data, data + len - offset, m_data );
            pad( len - offset );
        }
    }


	// Destructor, overwrites all data with 0 to make sure they're not
    // left in memory

    ~ciByte_Block( )
    {
        std::fill( m_data, m_data + N, 0 );
    }


    // Sets the padding mode (for all instances of the class!)

    static
    void
    set_padding_mode( Padding_Mode mode )
    {
        s_padding_mode = mode;
    }


    // Comparison operators

    bool
    operator == ( ciByte_Block const & other ) const
    {
        for ( int i = 0; i < N; ++i )
            if ( m_data[ i ] != other.m_data[ i ] )
                return false;

        return true;
    }


    bool
    operator != ( ciByte_Block const & other ) const
    {
        return ! ( *this == other );
    }


	// Assignment operator from other Byste_Block object (of possibly
    // different size) or a std::string

    template< int M >
    ciByte_Block &
    operator = ( ciByte_Block< M > & other )
    {
        if ( other == *this )
            return *this;

        if ( M >= N )
            std::copy( other.m_data, other.m_data + N, m_data );
        else
        {
            std::copy( other.m_data, other.m_data + M, m_data );
            pad( M );
        }

        return *this;
    }


    ciByte_Block &
    operator = ( std::string const & str )
    {
        char const * data = str.data( );
        int len = (int) str.size( );

        if ( len >= N )
            std::copy( data, data + N, m_data );
        else
        {
            std::copy( data, data + len, m_data );
            pad( len );
        }

        return *this;
    }


	// Operator/method for accessing an element
    // size_t index 를 전부 int index로 수정했음. override시 int 상수 값을 집에 넣게 되면
    // "ciByte_Block<N>::operator []' : 6개 오버로드에 비슷한 변환이 있습니다." 라는 에러 발생

    inline
    unsigned char
    operator [ ] ( int index ) const
    {
        return m_data[ index ];
    }



    inline
    unsigned char &
    operator [ ] ( int index )
    {
        return m_data[ index ];
    }


    unsigned char
    at( int index ) const
    {
        if ( index >= N )
            throw std::out_of_range( "Invalid acccess" );
        return m_data[ index ];
    }


	// XOR operator with another object (of the same size)

    ciByte_Block const
    operator ^ ( ciByte_Block const & other ) const
    {
        ciByte_Block res;
    
        for ( int i = 0; i < N; ++i ) {
            res[i] = m_data[ i ] ^ other[ i ];
        }

        return res;
    }

    ciByte_Block &
    operator ^= ( ciByte_Block const & other )
    {
        for ( int i = 0; i < N; ++i )
            m_data[ i ] ^= other[ i ];

        return *this;
    }


    // Left-shifting operator

    ciByte_Block const
    operator << ( int bit_cnt ) const
    {
        ciByte_Block res( *this );
        return res <<= bit_cnt;
    }


    ciByte_Block &
    operator <<= ( int bit_cnt )
    {
        int byte_cnt = bit_cnt / 8;

        if ( byte_cnt )
        {
            if ( byte_cnt >= N )
            {
                std::fill( m_data, m_data + N, 0 );
                return *this;
            }

            for ( int i = 0; i < N - byte_cnt; ++i )
                m_data[ i ] = m_data[ i + byte_cnt ];
            std::fill( m_data + N - byte_cnt, m_data + N, 0 );

            bit_cnt %= 8;
        }

        if ( bit_cnt != 0 )
        {
            for ( int i = 0; i < N - byte_cnt - 1; ++i )
                m_data[ i ] =   ( m_data[ i ] << bit_cnt )
                              | ( m_data[ i + 1 ] >> ( N - bit_cnt ) );
            m_data[ N - 1 ] <<= bit_cnt;
        }

        return *this;
    }


	// Prefix and postfix increment operators - data in the block are
    // treated as an unsigned integer type with N bytes, stored with
    // LSB first

	ciByte_Block &
	operator ++ ( )
	{
		int n = 0;
		while ( ++m_data[ n ] == 0 && ++n < N )
            /* empty */ ;

		return *this;
	}


	ciByte_Block const
	operator ++ ( int )
	{
		ciByte_Block res( *this );
        ++*this;
		return res;
	}


	// Implicit conversion to array of unsigned chars

    operator unsigned char * ( )
    {
        return m_data;
    }


    operator unsigned char const * ( ) const
    {
        return m_data;
    }


	// Implicit conversion to a pointer to chars (but with no '\0' at
    // the end, so NOT usable for conversion to a C string!)

    operator char * ( )
    {
        return cdata( );
    }


    operator char const * ( ) const
    {
        return cdata( );
    }


	// Returns a pointer to the data (at an optinal offset)

	unsigned char *
	data( int offset = 0 )
	{
		return m_data + offset;
	}


	unsigned char const *
	data( int offset = 0 ) const
	{
		return m_data + offset;
	}


	// Returns char pointer to the data (at an optional offset)

	char const *
	cdata( int offset = 0 ) const
	{
		return reinterpret_cast< char const * >( m_data + offset );
	}


	char *
	cdata( int offset = 0 )
	{
		return reinterpret_cast< char * >( m_data + offset );
	}

    
    // Returns the (copy of the) content of the data as a std::string

    std::string
    str( ) const
    {
        return std::string( cdata( ), N );
    }


	// Sets the data (at an optional offset) to those of a different object
	// (which may have a different length).

	template< int M >
	ciByte_Block &
	set( ciByte_Block< M > const & src,
		 int                  offset = 0 )
	{
		std::copy( src.m_data, src.m_data + std::min( N - offset, M ),
                   m_data + offset );
		return *this;
	}


	// Returns the objects data as a std::string. If the 'is_padded' flag
	// is set (which only happens during decryption) the padding bytes will
    // be removed. If the padding bytes aren't what they're supposed to be
    // an exception is thrown, indicating that the message was garbled.

    std::string
    as_string( bool is_padded = false ) const
    {
        int pos = N;

        if ( is_padded )
        {
            switch ( s_padding_mode )
            {
                case PKCS7 :
                    pos = N - m_data[ N - 1 ];
                    for ( int i = pos; i < N - 1; ++i )
                        if ( m_data[ i ] != pos )
                        {
                            pos = N + 1;
                            break;
                        }
                    break;


                case ANSIX9_23 :
                    pos = N - m_data[ N - 1 ];
                    for ( int i = pos; i < N - 1; ++i )
                        if ( m_data[ i ] != 0 )
                        {
                            pos = N + 1;
                            break;
                        }
                    break;

                case ISO7816_4 :                    \
                    while ( --pos > 0 && ! m_data[ pos ] )
                        /* empty */ ;

                    if ( m_data[ pos ] != 0x80 )
                        pos = N + 1;

                case ALL_NULL :
                    while ( --pos > 0 && ! m_data[ pos ] )
                        /* empty */ ;
                    ++pos;
                    break;
            }

            if ( pos == N + 1 )
                throw std::invalid_argument( "String to decrypt is garbled" );
        }

        return std::string( reinterpret_cast< char const * >( m_data ), pos );
    }


    // Returns the number of bytes initialized (excluding automatically
    // added padding - needed to be able to encrypt and decrypt blocks
    // with less than 16 bytes in CFB8 mode)

    int
    init_len( ) const
    {
        return m_init_len;
    }


	// Method for padding to the full length of N (if an initializer
	// or assigned value had less bytes). How that's done depends
    // what the padding mode is set to. Input is the number of "good"
    // bytes in the buffer.

    ciByte_Block &
    pad( int length )
    {
        if ( length == N )
        {
            m_init_len = 0;
            return *this;
        }

        switch ( s_padding_mode )
        {
            case PKCS7 :
                std::fill( m_data + length, m_data + N, N - length );
                break;

            case ANSIX9_23 :
                std::fill( m_data + length, m_data + N - 1, 0 );
                m_data[ N - 1 ] = N - length;
                break;

            case ISO7816_4 :                    \
                m_data[ length ] = 0x80;
                std::fill( m_data + length + 1, m_data + N, 0 );
                break;

            case ALL_NULL :
                std::fill( m_data + length, m_data + N, 0 );
                break;
        }

        m_init_len = length;
        return *this;
    }


  private :

	// Array of data

    unsigned char m_data[ N ];


    // Number of bytes that really got initialized

    unsigned int m_init_len;

    // Padding mode (global, i.e. applies to all instances of the class)

    static Padding_Mode s_padding_mode;
};


/*---------------------------------------------*
 * Definition (and initialization of the static 's_padding_mode'
 * member variable - there's one for each templated class.
 *---------------------------------------------*/

template< int N >
typename ciByte_Block< N >::Padding_Mode ciByte_Block< N >::s_padding_mode =
                                                   ciByte_Block< N >::ISO7816_4;


/*---------------------------------------------*
 * Function for printing out the data of an object
 * in a "raw" format, just an unstructured  chain of
 * hex nunbers (like it's used e.g. in the NIST
 * documentation).
 *---------------------------------------------*/

template< int N >
std::ostream &
operator << ( std::ostream          & out,
              ciByte_Block< N > const & block )
{
    std::ios_base::fmtflags flags( out.flags( ) );
    out.flags( std::ios::hex );

    for ( int i = 0; i < N; ++i )
        out << std::setw( 2 ) << std::setfill( '0' ) 
            << static_cast< unsigned int >( block[ i ] );

    out.flags( flags );
    return out;
}


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
