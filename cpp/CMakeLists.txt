PROJECT(srl)

INCLUDE( ${srl_SOURCE_DIR}/include/cmake_modules/CheckPrototypeExists.cmake )

INCLUDE_DIRECTORIES(
	${srl_SOURCE_DIR}/include
	${srl_SOURCE_DIR}/test
	/usr/local/include
)

LINK_DIRECTORIES(
	${srl_SOURCE_DIR}/../lib
)

SET(   LIBRARY_OUTPUT_PATH ${srl_SOURCE_DIR}/../lib    )
SET(   EXECUTABLE_OUTPUT_PATH ${srl_SOURCE_DIR}/../bin    )

CHECK_PROTOTYPE_EXISTS(gettimeofday "sys/time.h;unistd.h" HAVE_GETTIMEOFDAY )

CHECK_INCLUDE_FILE( sys/time.h HAVE_SYS_TIME_H )
CHECK_INCLUDE_FILE( pthread.h HAVE_PTHREAD_H )
CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE( unistd.h HAVE_UNISTD_H )

CONFIGURE_FILE(
  ${srl_SOURCE_DIR}/config_cmake.h.in
  ${srl_SOURCE_DIR}/include/srl/config.h
)

SET ( UPDATE_REVISION ${srl_SOURCE_DIR}/update_revision )
SET ( REV_FILE ${srl_SOURCE_DIR}/src/version.cpp )

ADD_CUSTOM_TARGET (UpdateRevision ALL)

ADD_CUSTOM_COMMAND(
    COMMAND ${UPDATE_REVISION}
    TARGET UpdateRevision
    COMMENT "updating srl version"
)

ADD_CUSTOM_COMMAND(
    SOURCE UpdateRevision
    TARGET UpdateRevision
    COMMENT "updating srl revision"
    DEPENDS ${REV_FILE}
)

SUBDIRS(
	src
    test
	examples
)
