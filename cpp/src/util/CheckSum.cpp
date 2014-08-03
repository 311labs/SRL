
#include "srl/util/CheckSum.h"

using namespace SRL;
using namespace SRL::Util;

uint16 CheckSum::value16()
{
	uint32 _fold = _sum;
	while (_fold >> 16)
	{
		_fold = (_fold >> 16) + ( _fold & 0x0000FFFF );
	}
	return (uint16)~_fold;
}

void CheckSum::doIpCheckSum(uint16 *buf, uint32 len)
{
	while ( len > 1)
	{
		_sum += *buf++, len -=2;
	}

	if (len == 1)
	{
		_sum += *(SRL::byte*) buf;
	}
}

void CheckSum::add(SRL::byte val)
{
	SRL::byte cipher = (val ^ (_r >> 8));
	_r = (cipher + _r) * _c1 + _c2;
	_sum += cipher;
}

void CheckSum::add(SRL::int16 val)
{
	add((SRL::byte*)&val, sizeof(int16));
}

void CheckSum::add(SRL::int32 val)
{
	add((SRL::byte*)&val, sizeof(int32));
}

void CheckSum::add(SRL::int64 val)
{
	add((SRL::byte*)&val, sizeof(int64));
}

// TODO implement
// void CheckSum::add(SRL::float32 val)
// {
//  //add((SRL::byte*)val, sizeof(float32));
// }
// 
// void CheckSum::add(SRL::float64 val)
// {
//  //add((SRL::byte*)val, sizeof(float64));
// }


void CheckSum::add(SRL::String val)
{
	add((SRL::byte*)val._strPtr(), val.size());
}

void CheckSum::add(SRL::byte* arry, int arry_size)
{
	for (int i=0; i < arry_size; ++i)
		add(arry[i]);
}

