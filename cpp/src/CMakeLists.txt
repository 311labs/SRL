PROJECT(srl)

IF (WIN32)
SET(SRL_SRC sys/windows/System.cpp
	sys/windows/SystemInfo.cpp
	sys/windows/ProcessList.cpp
	sys/windows/Keyboard.cpp
	fs/windows/FsObject.cpp
	fs/windows/Directory.cpp	
)
ELSE (WIN32)
	SET(SRL_SRC sys/linux/System.cpp
		sys/linux/SystemInfo.cpp
		sys/osx/SystemInfo.cpp	
		sys/linux/ProcessList.cpp
		sys/osx/ProcessList.cpp
		sys/linux/Keyboard.cpp
		fs/linux/FsObject.cpp
		fs/osx/FsObject.cpp
		fs/linux/Directory.cpp	
	)
ENDIF (WIN32)

	
ADD_LIBRARY(
	srl SHARED
	version.cpp
	String.cpp
	Date.cpp
	Time.cpp
	DateTime.cpp
	Timer.cpp
	Console.cpp
	sys/System.cpp
	sys/Mutex.cpp
	sys/Semaphore.cpp
	sys/Process.cpp
	sys/Thread.cpp
	sys/SystemInfo.cpp
	sys/ProcessList.cpp
	io/TextReader.cpp
	io/TextWriter.cpp
	io/BinaryReader.cpp
	io/BinaryWriter.cpp
	io/ByteBuffer.cpp
	fs/FS.cpp
	fs/FsObject.cpp
	fs/Directory.cpp
	fs/File.cpp
	fs/TextFile.cpp
	fs/FileStream.cpp
	fs/BinaryFile.cpp
	log/Logger.cpp
	log/LogFile.cpp
	log/SysLog.cpp
	net/InetAddress.cpp
	net/InetSocketAddress.cpp
	net/Socket.cpp
	net/DatagramSocket.cpp
	net/MulticastSocket.cpp
	net/IcmpMessage.cpp
	net/IcmpSocket.cpp
	net/icmpdll.cpp
	net/ServerSocket.cpp
	net/NetworkInterface.cpp	
	util/CheckSum.cpp
	math/Random.cpp
	app/ArgumentParser.cpp
	app/IniParser.cpp
	${SRL_SRC}
)



TARGET_LINK_LIBRARIES(srl pthread)

INSTALL_TARGETS(
        /lib srl
)
