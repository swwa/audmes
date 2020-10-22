cmake_minimum_required (VERSION 3.0)

project (AudMeS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AM_SOURCES
	AudMeS.cpp
	CtrlOScope.cpp
	dlg_audiointerface.cpp
	fourier.c
	RWAudio_IO.cpp
	rtaudio/RtAudio.cpp
)

include_directories(.) # for local libfccp

if (WIN32)
        set(AM_SOURCES ${AM_SOURCES} audmes.rc)
	set(wxWidgets_ROOT_DIR "C:/Users/ws/projects/wx3.0.5")
	set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/gcc810_dll")
endif()

set(wxWidgets_CONFIGURATION mswu)
set(wxWidgets_USE_STATIC ON)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include("${wxWidgets_USE_FILE}")

add_executable(AudMeS ${AM_SOURCES})

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-D__LINUX_PULSE__ -D__LINUX__ -DHAVE_GETTIMEOFDAY)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} pulse pulse-simple)
endif()

if (${APPLE})
	add_definitions(-D__MACOSX_CORE__ -DHAVE_GETTIMEOFDAY)
	find_library(COREAUDIO_LIB CoreAudio)
	find_library(COREFOUNDATION_LIB CoreFoundation)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${COREAUDIO_LIB} ${COREFOUNDATION_LIB})
endif()

if (WIN32)
        add_definitions(-D__WINDOWS_DS__ -D__WXMSW__)
	target_link_libraries(${PROJECT_NAME} -mwindows -static ${wxWidgets_LIBRARIES} dsound)
endif()

install(TARGETS "AudMeS" DESTINATION "bin")
install(FILES "audmes.desktop" DESTINATION "share/applications")
install(FILES "audmes.png" DESTINATION "share/pixmaps")
install(FILES "audmes.png" DESTINATION "share/icons/hicolor/128x128/apps/")
