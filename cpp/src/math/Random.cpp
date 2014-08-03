#include "srl/math/Random.h"
#include "srl/sys/System.h"

using namespace SRL;
using namespace SRL::Math;

Random::Random()
{
    TimeValue tv;
    System::GetCurrentTime(tv);
    initialize(tv.tv_usec);
}

Random::Random(const uint32& seed)
{
    initialize(seed);
}


void Random::initialize(const uint32& seed)
{
	// Initialize generator state with seed
	// See Knuth TAOCP Vol 2, 3rd Ed, p.106 for multiplier.
	// In previous versions, most significant bits (MSBs) of the seed affect
	// only MSBs of the state array.  Modified 9 Jan 2002 by Makoto Matsumoto.
	register uint32 *s = _values;
	register uint32 *r = _values;
	register int i = 1;
	*s++ = seed & 0xffffffffUL;
	for( ; i < N; ++i )
	{
		*s++ = ( 1812433253UL * ( *r ^ (*r >> 30) ) + i ) & 0xffffffffUL;
		r++;
	}
	reload();    
}

void Random::reload()
{
	// Generate N new values in state
	// Made clearer and faster by Matthew Bellew (matthew.bellew@home.com)
	register uint32 *p = _values;
	register int i;
	for( i = N - M; i--; ++p )
		*p = twist( p[M], p[0], p[1] );
	for( i = M; --i; ++p )
		*p = twist( p[M-N], p[0], p[1] );
	*p = twist( p[M-N], p[0], _values[0] );

	_remaining = N, _next = _values;    
}

float64 Random::random()
{
    return float64(randomInt()) * (1.0/4294967295.0);   
}

float64 Random::random(const float64& n)
{
    return random() * n;
}

uint32 Random::randomInt()
{
    // Pull a 32-bit integer from the generator state
    // Every other access function simply transforms the numbers extracted here

    if( _remaining == 0 ) 
        reload();
    
    --_remaining;
	
    register uint32 s1;
    s1 = *_next++;
    s1 ^= (s1 >> 11);
    s1 ^= (s1 <<  7) & 0x9d2c5680UL;
    s1 ^= (s1 << 15) & 0xefc60000UL;
    return ( s1 ^ (s1 >> 18) );    
}

uint32 Random::randomInt(const uint32& n)
{
	// Find which bits are used in n
	// Optimized by Magnus Jonsson (magnus@smartelectronix.com)
	uint32 used = n;
	used |= used >> 1;
	used |= used >> 2;
	used |= used >> 4;
	used |= used >> 8;
	used |= used >> 16;
	
	// Draw numbers until one is found in [0,n]
	uint32 i;
	do
		i = randomInt() & used;  // toss unused bits to shorten search
	while( i > n );
	
	return i;
}

