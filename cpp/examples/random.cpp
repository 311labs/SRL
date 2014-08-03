
#include "srl/app/ArgumentParser.h"
#include "srl/Console.h"
#include "srl/math/Random.h"
#include "srl/util/Vector.h"

using namespace SRL;

void random_sort_float()
{
    SRL::Util::Vector<float64> vector;
    SRL::Math::Random rand(55);
    for (int i=0;i<20;++i)
    {
        float64 value = rand.random(20.0);
        Console::formatLine("random value: %0.4f", value);
        vector.add(value);
    }
    Console::writeLine("\nsorted: ");
    vector.sort();
    Console::writeLine("sort complete");
    //vector.swap(0, 5);
    Console::writeLine("dump");
    for (int i=0;i<20;++i)
    {
        Console::formatLine("random value: %0.4f", vector[i]);
        
    }    
}

void sort_str()
{
    const String strings[15] = {"google", "fool", "hello", "amazon", "zoo", "usaa", "baby",
                         "green", "red", "purple", "blue", "yellow", "white", "black"};
    SRL::Util::Vector<const String*> vector;
    for( uint32 i = 0; i < 14; ++i )
    {
        vector.add(&strings[i]);
    }
    
    Console::writeLine("\nsorted: ");
    vector.sort();
    for( uint32 i = 0; i < 14; ++i )
    {
        Console::writeLine(vector[i]->text());
    }
}


int main (int argc, char const* argv[])
{
    sort_str();
    
    return 0;
}