CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

project (oamlStudio)

# Configuration types
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("Debug mode: Yes (Disable by param -DCMAKE_BUILD_TYPE=Release)")
else()
	message("Debug mode: No (Enable by param -DCMAKE_BUILD_TYPE=Debug)")
endif()


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

find_package(OggVorbis)
find_package(VorbisFile)

if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	include_directories(${VORBISFILE_INCLUDE_DIR})
	set(OAML_LIBS ${OAML_LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
endif()

##
# Find SDL
#
set(SDL_BUILDING_LIBRARY true)
find_package(SDL)
set(OAML_LIBS ${OAML_LIBS} ${SDL_LIBRARY})

##
# Find minizip
#
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
	PKG_CHECK_MODULES(UNZIP minizip)
endif(PKG_CONFIG_FOUND)

set(OAML_LIBS ${OAML_LIBS} ${UNZIP_LIBRARIES})

##
# 
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL_INCLUDE_DIR})
set(LIBS ${OAML_LIBS} ${CMAKE_CURRENT_SOURCE_DIR}/liboaml.a)
set(SRCS src/audioFile.cpp src/oamlStudio.cpp src/waveformDisplay.cpp src/aif.cpp src/ogg.cpp src/wav.cpp)

if (APPLE)
	add_executable(oamlStudio MACOSX_BUNDLE ${SRCS} images/play.png images/pause.png)
	set_source_files_properties(images/play.png images/pause.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
else()
	add_executable(oamlStudio WIN32 ${SRCS})
endif()

target_link_libraries(oamlStudio ${wxWidgets_LIBRARIES} ${LIBS})
