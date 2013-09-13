project(ss)

cmake_minimum_required(VERSION 2.6)

set (SOURCE_FILES	
	${CMAKE_SOURCE_DIR}/ss/bulk_setting.cpp 
	${CMAKE_SOURCE_DIR}/ss/configuration.cpp 
	${CMAKE_SOURCE_DIR}/ss/defaults_holder.cpp 
	${CMAKE_SOURCE_DIR}/ss/enum.cpp 
	${CMAKE_SOURCE_DIR}/ss/error.cpp 
	${CMAKE_SOURCE_DIR}/ss/file_storage.cpp 
	${CMAKE_SOURCE_DIR}/ss/registry_storage.cpp 
	${CMAKE_SOURCE_DIR}/ss/util.cpp
)

set (INCLUDE_FILES
    ${CMAKE_SOURCE_DIR}/ss/array.h 
	${CMAKE_SOURCE_DIR}/ss/bulk_setting.h 
	${CMAKE_SOURCE_DIR}/ss/configuration.h 
	${CMAKE_SOURCE_DIR}/ss/const_.h 
	${CMAKE_SOURCE_DIR}/ss/defaults_holder.h 
	${CMAKE_SOURCE_DIR}/ss/enum.h
	${CMAKE_SOURCE_DIR}/ss/error.h
	${CMAKE_SOURCE_DIR}/ss/file_storage.h
	${CMAKE_SOURCE_DIR}/ss/fwd.h
	${CMAKE_SOURCE_DIR}/ss/registry_storage.h
	${CMAKE_SOURCE_DIR}/ss/setting.h
	${CMAKE_SOURCE_DIR}/ss/setting_storage.h
	${CMAKE_SOURCE_DIR}/ss/template.h
	${CMAKE_SOURCE_DIR}/ss/ts.h
	${CMAKE_SOURCE_DIR}/ss/util.h
)

# Ensure that eclipse can parse GCCs output
IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSS_DONT_USE_BOOST")

include_directories(${CMAKE_SOURCE_DIR})
add_library(ss STATIC ${SOURCE_FILES})

install (TARGETS ss DESTINATION lib)
install (FILES ${INCLUDE_FILES} DESTINATION include/ss)