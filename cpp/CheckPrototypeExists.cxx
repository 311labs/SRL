#include <unistd.h>
extern "C" {
typedef union { int member; } dummyStruct;
dummyStruct gettimeofday( dummyStruct );
}
int main(){return 0;}
