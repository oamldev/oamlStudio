CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

project (oamlStudio)
set(CMAKE_BUILD_TYPE Release)

find_package(wxWidgets REQUIRED html adv core base net aui xrc qa richtext)
include(${wxWidgets_USE_FILE})


##
# Set CXX_FLAGS depending on compiler
#
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -std=c++11")
endif()


##
# Find VorbisFile lib
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib;.a")

find_package(OggVorbis)
find_package(VorbisFile)

if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	include_directories(${VORBISFILE_INCLUDE_DIR})
	set(OAML_LIBS ${OAML_LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
endif()


##
# 
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIBS ${OAML_LIBS} ${CMAKE_CURRENT_SOURCE_DIR}/liboaml.a)

ADD_EXECUTABLE(oamlStudio WIN32 ${SRCS} src/audioFile.cpp src/oamlStudio.cpp src/aif.cpp src/ogg.cpp src/wav.cpp )
TARGET_LINK_LIBRARIES(oamlStudio ${wxWidgets_LIBRARIES} ${LIBS})
