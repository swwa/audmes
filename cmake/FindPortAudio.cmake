# - Try to find PortAudio
# Once done this will define
#
#  PORTAUDIO_FOUND - system has PortAudio
#  PORTAUDIO_INCLUDE_DIRS - the PortAudio include directory
#  PORTAUDIO_LIBRARIES - Link these to use PortAudio
#  PORTAUDIO_DEFINITIONS - Compiler switches required for using PortAudio
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
# Redistribution and use is allowed according to the terms of the New BSD license.

if(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  # already in cache, be silent
  set(PORTAUDIO_FIND_QUIETLY TRUE)
endif(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)

find_path(PORTAUDIO_INCLUDE_DIR
  NAMES
    portaudio.h
  PATHS
    /usr/include
    /usr/local/include
    /opt/local/include
    /sw/include
)

find_library(PORTAUDIO_LIBRARY
  NAMES
    portaudio
    libportaudio
  PATHS
    /usr/lib
    /usr/local/lib
    /opt/local/lib
    /sw/lib
)

set(PORTAUDIO_INCLUDE_DIRS
  ${PORTAUDIO_INCLUDE_DIR}
)
set(PORTAUDIO_LIBRARIES
  ${PORTAUDIO_LIBRARY}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PortAudio DEFAULT_MSG PORTAUDIO_LIBRARIES PORTAUDIO_INCLUDE_DIRS)

# show the PORTAUDIO_INCLUDE_DIRS and PORTAUDIO_LIBRARIES variables only in the advanced view
mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)
