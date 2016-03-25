CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

project(oamlStudio)

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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

##
# Find VorbisFile lib
#
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(OggVorbis REQUIRED)
find_package(VorbisFile REQUIRED)

if (OGGVORBIS_FOUND AND VORBISFILE_FOUND)
	include_directories(${VORBISFILE_INCLUDE_DIR})
	set(LIBS ${LIBS} ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARIES})
endif()

##
# Find SDL
#
set(SDL_BUILDING_LIBRARY true)
find_package(SDL REQUIRED)
set(LIBS ${LIBS} ${SDL_LIBRARY})

##
# Find liblzip
#
if (APPLE)
	set(LibArchive_INCLUDE_DIR "/usr/local/opt/libarchive/include")
endif()
find_package(LibArchive REQUIRED)
set(LIBS ${LIBS} ${LibArchive_LIBRARIES})

##
# Find oaml
#
find_package(OAML REQUIRED)
set(LIBS ${LIBS} ${OAML_LIBRARIES})

##
# 
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${SDL_INCLUDE_DIR} ${LibArchive_INCLUDE_DIR})
set(SRCS src/audioFile.cpp src/audioPanel.cpp src/oamlCallbacks.cpp src/layerPanel.cpp src/oamlStudio.cpp src/playbackFrame.cpp src/waveformDisplay.cpp src/aif.cpp src/ogg.cpp src/wav.cpp)

if (APPLE)
	add_executable(oamlStudio MACOSX_BUNDLE ${SRCS} images/play.png images/pause.png)
	set_source_files_properties(images/play.png images/pause.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
else()
	add_executable(oamlStudio WIN32 ${SRCS})
endif()

target_link_libraries(oamlStudio ${wxWidgets_LIBRARIES} ${LIBS})

##
# Install rules
#
install(TARGETS oamlStudio DESTINATION bin)
