
#include "srl/exports.h"

#define SRL_MAJOR 2
#define SRL_MINOR 10
#define SRL_REVISION 4


void SRL::GetVersion(int &major, int &minor, int &revision)
{
	major = SRL_MAJOR;
	minor = SRL_MINOR;
	revision = SRL_REVISION;
}

