
#include "srl/Console.h"
#include "srl/DateTime.h"

#include <cstdlib>
#include <string.h>

#include <stdio.h>
#include <stdarg.h> 
#ifndef WIN32
#include <execinfo.h>
#endif
using namespace SRL;

void Console::clear()
{
// FIXME grab the console type to handle clear better (Cygwin vs Win32)
#ifdef WIN32
	std::system("cls");
#endif
	std::system("clear");
}

char Console::read()
{
	return ::getchar();
}

String Console::readLine()
{
	// read at most 1024 bytes
	String buf;
	buf.resize(1024);
	return ::fgets(buf._strPtr(), buf.allocatedSize(), stdin);
}

void Console::readLine(String &buf)
{
	char *bufptr = ::fgets(buf._strPtr(), buf.allocatedSize(), stdin);
	buf.resize(static_cast<uint32>(strlen(bufptr)));
}

void Console::write(const String& output)
{
	for (uint32 i=0; i < output.length(); ++i)
	{
		putc(output[i], stdout);
	}
	fflush(stdout);
}

void Console::write(const char* output)
{
	int len = static_cast<int>(strlen(output));
	for (int i = 0; i < len; ++i)
	{
		putc(output[i], stdout);
	}
	fflush(stdout);
}

void Console::writeLine(const String& output)
{
	puts(output.text());
	fflush(stdout);
}

void Console::writeLine(const char* output)
{
	puts(output);
	fflush(stdout);
}

void Console::format(const char* format, ...)
{
	va_list args;
	va_start(args,format);
	vprintf(format, args);
	va_end(args);
	
	fflush(stdout);
}

void Console::formatLine(const char* format, ...)
{
	va_list args;
	va_start(args,format);
	vprintf(format, args);
	va_end(args);
	
	Console::writeEndLine();
}

void Console::writeDateTime()
{
	DateTime dt;
	Console::write(dt.asString());
	fflush(stdout);
}

void Console::writeEndLine()
{
#ifdef WIN32
	putc('\r', stdout);
#endif
	putc('\n', stdout);
	fflush(stdout);
}

// STD ERROR

void Console::err::write(const String& output)
{
	for (uint32 i=0; i < output.length(); ++i)
	{
		putc(output[i], stderr);
	}
	fflush(stderr);
}

void Console::err::write(const char* output)
{
	int len = static_cast<int>(strlen(output));
	for (int i=0; i < len; ++i)
	{
		putc(output[i], stderr);
	}
	fflush(stderr);
}

void Console::err::writeLine(const String& output)
{
	puts(output.text());
	fflush(stderr);
}

void Console::err::writeLine(const char* output)
{
	puts(output);
	fflush(stderr);
}

void Console::err::format(const char* format, ...)
{
	va_list args;
	va_start(args,format);
	vfprintf(stderr, format, args);
	va_end(args);
}

void Console::err::formatLine(const char* format, ...)
{
	va_list args;
	va_start(args,format);
	vfprintf(stderr, format, args);
	va_end(args);
	Console::err::writeEndLine();
}

void Console::err::writeDateTime()
{
	DateTime dt;
	Console::err::write(dt.asString());
	fflush(stderr);
}

void Console::err::writeEndLine()
{
#ifdef WIN32
	putc('\r', stderr);
#endif
	putc('\n', stderr);
	fflush(stderr);
}

void Console::err::dumpStack()
{
#ifdef WIN32
	
#else
    void *addresses[10];
    char **strings;

    int size = backtrace(addresses, 10);
    strings = backtrace_symbols(addresses, size);
    printf("Stack frames: %d\n", size);
    for(int i = 0; i < size; i++) 
    {
        printf("%d: %X\n", i, addresses[i]);
        printf("%s\n", strings[i]);
    }
    free(strings);
#endif
}
