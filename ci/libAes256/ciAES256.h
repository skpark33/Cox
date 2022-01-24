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
#if ! defined ciAES256_hpp_
#define ciAES256_hpp_


#include "ci/libAes256/ciAES256_Base.h"
#include "ci/libAes256/ciAES256_PRNG.h"
#include "ci/libAes256/ciPadding_Type.h"
#include <iostream>
#include <stdexcept>


template< int N > class ciByte_Block;


/*---------------------------------------------*
 * Class implementing the encryption and decryption of arbitrary
 * long blocks of data, in the form of C++ std::strings (which
 * can hold binary data).
 *
 * Several different block chaining modes are implemented,
 * currently these are ECB, CBC, PCBC, CFB-128. CFB-8, OFB
 * and CTR. The default, when none is explicitly selected,
 * is CBC.
 *
 * Also different padding modes can be selected (if the
 * chaining mode requires padding) can be selected. Per
 * default, messages with lengths divisible by 16 are
 * (for the modes requiring padding) padded with a full
 * 16 byte long block.
 *
 * Except in ECB mode, which doesn't use an initialization
 * vector the (unencrpted) IV is prepended to the encrypted
 * data.
 *---------------------------------------------*/


class ciAES256 : public ciPadding_Type
{
  public :

    typedef unsigned char byte;

	enum Chaining_Mode
	{
		ECB,
		CBC,
		PCBC,
		CFB,
        CFB128 = CFB,
        CFB8,
		OFB,
		CTR
	};

    using ciPadding_Type::Padding_Mode;

    // Construtor, requires a std::string with a 32-byte key and,
    // for secure operation, an (at least) 16 byte string to seed
    // the random generator used for creating a secure IV etc. It
    // also accepts a block cipher mode (which defaults to CBC)

    ciAES256( std::string const & key,
            std::string const & seed,
            Chaining_Mode       chaining_mode = CBC,
            Padding_Mode        padding_mode = ANSIX9_23 );

    // Constructor without seed for random generator, should only
    // be used when a new IV is set before each encryption or for
    // testing purposes.

    ciAES256( std::string const & key,
            Chaining_Mode       chaining_mode = CBC,
            Padding_Mode        padding_mode = ANSIX9_23 );

    // Selects the chaining mode

	void
	set_chaining_mode( Chaining_Mode mode );

    // Selects the padding mode

    void
    set_padding_mode( Padding_Mode mode );

    // Allows to set am  initialization vector. When called with
    // an empty string (or no argument) it switches back to usind
    // a new, randomly chosen IV for each encryption.

    void
    set_IV( std::string const & IV = std::string( ) );


    // Returns the (current state of the ) IV - only use this if an IV
    // has been set via set_IV()

    std::string
    get_IV( ) const;


    // Returns the key

    std::string
    get_key( ) const;


    // Returns if current chaining mode uses padding

    bool
    uses_padding( ) const;


    // Encrypts a std::string (returned as a new string). Per default for
    // modes with padding a complete block of padding is added if the
    // message length is divisible by 16 - switch this off by setting
    // 'no_padding_block'.

    std::string
    encrypt( std::string const & in,
             bool                no_padding_block = false );

    // Encrypts a std::istream, outputs to a std::ostream

    std::ostream &
    encrypt( std::istream & in,
             std::ostream & out,
             bool no_padding_block = false );

    // Decrypts an encrypted std::string (returned as a new string)
    // If 'no_padding_block' is set the method expects that a full
    // block of padding (for messages that originally had a length
    // divisible by 16) is NOT present.

    std::string
    decrypt( std::string const & in,
             bool no_padding_block = false );


	// Decrypts a std::istream, outputs to a std::ostream

    std::ostream &
    decrypt( std::istream & in,
             std::ostream & out,
             bool           no_padding_block = false );

  private :

    // ECB mode encryption method

    ciByte_Block< 16 > &
    ecb( ciByte_Block< 16 > & buf );

    // ECB mode decryption method

    ciByte_Block< 16 > &
    ecb_inv( ciByte_Block< 16 > & buf );

    // CBC mode encryption method

    ciByte_Block< 16 > &
    cbc( ciByte_Block< 16 > & buf );

    // CBC mode decryption method

    ciByte_Block< 16 > &
    cbc_inv( ciByte_Block< 16 > & buf );

    // PCBC mode encryption method

    ciByte_Block< 16 > &
    pcbc( ciByte_Block< 16 > & buf );

    // PCBC mode decryption method

	ciByte_Block< 16 > &
	pcbc_inv( ciByte_Block< 16 > & buf );

    // CFB128 mode encryption method

    ciByte_Block< 16 > &
    cfb128( ciByte_Block< 16 > & buf );

    // CFB128 mode decryption method

	ciByte_Block< 16 > &
	cfb128_inv( ciByte_Block< 16 > & buf );

    // CFB-8 mode encryption method

    ciByte_Block< 16 > &
    cfb8( ciByte_Block< 16 > & buf );

    // CFB-8 mode decryption method

	ciByte_Block< 16 > &
	cfb8_inv( ciByte_Block< 16 > & buf );

    // OFB mode encryption and decryption method

    ciByte_Block< 16 > &
    ofb( ciByte_Block< 16 > & buf );

    // CTR mode encryption and decryption method

    ciByte_Block< 16 > &
    ctr( ciByte_Block< 16 > & buf );


    // Current IV

	ciByte_Block< 16 > m_IV;

    // Basic encryption class instance

	ciAES256_Base m_aes256_base;

    // Current block cipher mode

	Chaining_Mode m_mode;

    // Pointers to functions to be used in current block cipher mode

	ciByte_Block< 16 > & ( ciAES256::* enc )( ciByte_Block< 16 > & );
	ciByte_Block< 16 > & ( ciAES256::* dec )( ciByte_Block< 16 > & );

    // Pseudo-random generator to be used for generating IVs

    ciAES256_PRNG m_prng;

    // Set when a new, randomly generated IV is to be used for each encryption

	bool m_use_random_IV;

    // Set for modes that require padding

    bool m_use_padding;
};


#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
