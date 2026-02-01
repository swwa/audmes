include_directories(..) # for RWAudio

find_package(wxWidgets REQUIRED core base)
include("${wxWidgets_USE_FILE}")
message("wxWidgets lib settings: ${wxWidgets_LIBRARIES}")

add_definitions(-D__LINUX_ALSA__ -D__LINUX_PULSE__ -D__LINUX__ -DHAVE_GETTIMEOFDAY)

add_executable (sma_2d_test EXCLUDE_FROM_ALL sma_2d_test.cpp ../sma_2d.cpp)
add_executable (rwautio_io_test EXCLUDE_FROM_ALL rwaudio_io_test.cpp ../RWAudio_IO.cpp ../rtaudio/RtAudio.cpp)
target_link_libraries(rwautio_io_test ${wxWidgets_LIBRARIES} asound pulse pulse-simple)
