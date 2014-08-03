PROJECT(srl)

ADD_EXECUTABLE(
	proclist
	proclist.cpp
	)


ADD_EXECUTABLE(
	socket_example
	SimpleSocket.cpp
	)

ADD_EXECUTABLE(
	random
	random.cpp
	)

ADD_EXECUTABLE(
	filetool
	filetool.cpp
	)

TARGET_LINK_LIBRARIES(proclist srl)	
TARGET_LINK_LIBRARIES(socket_example srl)
TARGET_LINK_LIBRARIES(random srl)
TARGET_LINK_LIBRARIES(filetool srl)
