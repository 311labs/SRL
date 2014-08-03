/**
 * File: Random.h
 * Project: srl3
 * Created by: Ian Starnes on 2006-11-21.
 * 
 * This library is free software; you can redistribute it and/or                 
 * modify it under the terms of the GNU Lesser General Public                    
 * License as published by the Free Software Foundation; either                  
 * version 2.1 of the License, or (at your option) any later version.            
 *                                                                               
 * This library is distributed in the hope that it will be useful,               
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU             
 * Lesser General Public License for more details.                               
 *                                                                               
 * You should have received a copy of the GNU Lesser General Public              
 * License along with this library; if not, write to the Free Software           
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.   
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "srl/types.h"
#include "srl/exports.h"

namespace SRL
{
    /** Classes and Functions related to Math */
    namespace Math
    {
        /** 
         * This Random Class uses the Mersenne Twister random number generator
         * Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
         * and code from Richard J. Wagner
         * 
         * The Mersenne twister is a pseudorandom number generator developed
         * in 1997 by Makoto Matsumoto ( ) and Takuji Nishimura ( )[1] that
         * is based on a matrix linear recurrence over a finite binary field
         * . It provides for fast generation of very high quality pseudorandom
         * numbers, having been designed specifically to rectify many of the
         * flaws found in older algorithms.  Its name derives from the fact
         * that period length is chosen to be a Mersenne prime. There are at
         * least two common variants of the algorithm, differing only in the
         * size of the Mersenne primes used. The newer and more commonly used
         * one is the Mersenne Twister MT19937, with 32-bit word length.  There
         * is also a variant with 64-bit word length, MT19937-64, which generates
         * a different sequence.
         * 
         */
        class SRL_API Random
        {
        public:
            /** auto-initialized with a seed from the system clock */
            Random();
            /** initialize with a simple seed */
            Random(const uint32& seed);
            /** destructor */
            virtual ~Random(){};
            
            /** generate the next random number(0.0 to 1.0) */
            float64 random();
            /** generate the next random number (0.0 to n) */
            float64 random(const float64& n);
            /** generate a 32bit random integer (0 - 2^32-1) */
            uint32 randomInt();
            /** generate a 32bit random integer (0 - n) */
            uint32 randomInt(const uint32& n);
            
        protected:
            enum { M = 397 };
            enum { N = 624 };
            /** vector used to hold random numbers */
            uint32 _values[N];
            /** pointer to the next value to use */
            uint32 *_next;
            /** number of values remaining */
            int _remaining;
            
            /** initialize with the seed */
            void initialize(const uint32& seed);
            /** reload the random value vector */
            void reload();
            
            /** Twister Logic */
        	uint32 hiBit(const uint32& u) const { return u & 0x80000000UL; }
        	uint32 loBit(const uint32& u ) const { return u & 0x00000001UL; }
        	uint32 loBits(const uint32& u) const { return u & 0x7fffffffUL; }
        	uint32 mixBits(const uint32& u, const uint32& v ) const{ return hiBit(u) | loBits(v); }
        	uint32 twist(const uint32& m, const uint32& s0, const uint32& s1 ) const
        		{ return m ^ (mixBits(s0,s1)>>1) ^ (-loBit(s1) & 0x9908b0dfUL); }
        };
    }
}

#endif /* _RANDOM_H_ */

