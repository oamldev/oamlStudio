CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

# Creates C resources file from files in given directory
function(create_resources dir output)
    # Create empty output file
    file(WRITE ${output} "")
    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    foreach(bin ${bins})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${output} "const unsigned char ${filename}[] = {${filedata}};\nconst unsigned ${filename}_size = sizeof(${filename});\n")
    endforeach()
endfunction()

project(oamlStudio)
set(oamlStudio_VERSION_MAJOR 1)
set(oamlStudio_VERSION_MINOR 1)
set(oamlStudio_VERSION_PATCH 1)

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
# Resource files
#
create_resources(images ${CMAKE_CURRENT_BINARY_DIR}/resources.h)

##
# 
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${LibArchive_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
set(SRCS
	src/audioFile.cpp
	src/audioPanel.cpp
	src/audioFilePanel.cpp
	src/oamlCallbacks.cpp
	src/controlPanel.cpp
	src/layerPanel.cpp
	src/oamlStudio.cpp
	src/playbackFrame.cpp
	src/settingsFrame.cpp
	src/startupFrame.cpp
	src/studioFrame.cpp
	src/trackControl.cpp
	src/trackPanel.cpp
	src/waveformDisplay.cpp
	src/aif.cpp
	src/ogg.cpp
	src/wav.cpp)

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
set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP ON)
set(CMAKE_INSTALL_DEBUG_LIBRARIES ON)
include(InstallRequiredSystemLibraries)

install(TARGETS oamlStudio DESTINATION bin)

if (APPLE)
	set(APPS ${CMAKE_CURRENT_BINARY_DIR}/oamlStudio.app)  # paths to executables
	set(DIRS . /usr/lib /usr/local/lib)   # directories to search for prerequisites

	install(CODE "
	   include(BundleUtilities)
	   fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
	   " COMPONENT Runtime)
endif()
