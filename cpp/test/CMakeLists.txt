PROJECT(srl)

ADD_EXECUTABLE(
	run_tests
	test.cpp
	TypeTest.cpp
	SystemTest.cpp
	StringTest.cpp
	util/VectorTest.cpp
	util/DictTest.cpp
	fs/FsTests.cpp
	net/SocketTest.cpp
	)
	
TARGET_LINK_LIBRARIES(run_tests srl)

