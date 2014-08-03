PROJECT(jsrl)

INCLUDE_DIRECTORIES(
	${jsrl_SOURCE_DIR}/include
	${jsrl_SOURCE_DIR}/../cpp/include
	$ENV{JDK_HOME}/include
	/usr/local/include
	$ENV{JDK_HOME}/include/linux
	/System/Library/Frameworks/JavaVM.framework/Versions/1.5.0/Headers/
)

LINK_DIRECTORIES(
	${jsrl_SOURCE_DIR}/../lib
)

SET(   LIBRARY_OUTPUT_PATH ${jsrl_SOURCE_DIR}/../lib    )

SUBDIRS(
	jni
)

SET ( JAVA_COMPILER ant )
SET ( BUILD_XML ${jsrl_SOURCE_DIR}/build.xml )

ADD_CUSTOM_TARGET (JavaBindings POST_BUILD)

ADD_CUSTOM_COMMAND(
    COMMAND ${JAVA_COMPILER}
    TARGET JavaBindings
)

ADD_CUSTOM_COMMAND(
    SOURCE JavaBindings
    TARGET JavaBindings
    COMMENT "building java bindings"
    DEPENDS ${BUILD_XML}
)
