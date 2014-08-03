
CC=g++
CFLAGS=-Wall -I../include

SOURCES=version.cpp String.cpp Date.cpp Time.cpp DateTime.cpp Timer.cpp Console.cpp
SOURCES+=sys/System.cpp sys/Mutex.cpp sys/Semaphore.cpp sys/Process.cpp sys/Thread.cpp
SOURCES+=sys/ThreadPool.cpp sys/SystemInfo.cpp sys/CpuInfo.cpp sys/ProcessList.cpp 
SOURCES+=io/TextReader.cpp io/TextWriter.cpp io/BinaryReader.cpp io/BinaryWriter.cpp 
SOURCES+=io/ByteBuffer.cpp fs/FS.cpp fs/FsObject.cpp fs/Directory.cpp fs/File.cpp 
SOURCES+=fs/TextFile.cpp fs/FileStream.cpp fs/BinaryFile.cpp log/Logger.cpp log/LogFile.cpp
SOURCES+=log/SysLog.cpp net/InetAddress.cpp net/InetSocketAddress.cpp net/Socket.cpp 
SOURCES+=net/DatagramSocket.cpp net/MulticastSocket.cpp net/TcpSocket.cpp net/IcmpMessage.cpp 
SOURCES+=net/IcmpSocket.cpp net/icmpdll.cpp net/ServerSocket.cpp net/NetworkInterface.cpp 
SOURCES+=net/HttpClient.cpp net/HttpHeaders.cpp net/HttpServer.cpp net/URI.cpp util/CheckSum.cpp 
SOURCES+=util/Base64.cpp math/Random.cpp app/ArgumentParser.cpp app/IniParser.cpp xml/XML.cpp 
SOURCES+=xml/XmlRpcProtocol.cpp rpc/RpcValue.cpp rpc/RpcProtocol.cpp rpc/RpcSystemService.cpp
SOURCES+=json/JSON.cpp

UNAME:= $(shell uname)

SHARED_LIBRARY=libsrl.so
LIB_DIR=$(shell pwd)
OBJECTS=$(SOURCES:.cpp=.o)
LDFLAGS= -shared -lpthread

ifeq ($(UNAME), Darwin)
	SOURCES+=sys/linux/System.cpp sys/osx/SystemInfo.cpp sys/osx/System.cpp sys/osx/ProcessList.cpp sys/linux/Keyboard.cpp app/unix/Daemon.cpp fs/osx/FsObject.cpp fs/linux/FS.cpp fs/linux/Directory.cpp net/osx/NetworkInterface.cpp
	SHARED_LIBRARY=libsrl.dylib
	LDFLAGS= -dynamiclib -install_name $(LIB_DIR)/$(SHARED_LIBRARY)
endif

ifeq ($(UNAME), Linux)
	SOURCES+=sys/linux/System.cpp sys/linux/SystemInfo.cpp sys/linux/ProcessList.cpp sys/linux/Keyboard.cpp app/unix/Daemon.cpp fs/linux/FsObject.cpp fs/linux/Directory.cpp fs/linux/FS.cpp net/linux/NetworkInterface.cpp
endif



all: $(SOURCES) $(SHARED_LIBRARY)
	@echo "\tSRL has been built"
	@echo "\tSRL is located at lib/$(SHARED_LIBRARY)"

$(SHARED_LIBRARY): $(OBJECTS)
	@echo ==== creating shared library $@ ====
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo copying $@ to srl/lib/
	@mkdir -p ../lib
	@cp $@ ../lib/
	@echo $@ has been built

.cpp.o:
	@echo "||||  building $@  ||||"
	$(CC) $(CFLAGS) -c $< -o $@
	@echo 

clean:
	@echo "deleting SRL object files..."
	@find . -name "*.o" -delete;
	@echo "deleting $(SHARED_LIBRARY)"
	@-rm $(SHARED_LIBRARY)
	@-rm ../lib/$(SHARED_LIBRARY)
