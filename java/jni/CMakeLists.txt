PROJECT(jsrl)

ADD_LIBRARY(
	jsrl SHARED
	JavaPingImpl.cpp
	JSystemInfo.cpp
)

TARGET_LINK_LIBRARIES(jsrl srl)
