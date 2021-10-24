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
	string(REPLACE "\\" "/" HDIR $ENV{HOMEDRIVE}$ENV{HOMEPATH})
	set(wxWidgets_ROOT_DIR "${HDIR}/projects/wx3.0.5")
	set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/gcc810_dll")
	set(wxWidgets_CONFIGURATION mswu)
endif()

find_package(wxWidgets COMPONENTS core base REQUIRED)
include("${wxWidgets_USE_FILE}")

add_executable(AudMeS ${AM_SOURCES})

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	add_definitions(-D__LINUX_ALSA__ -D__LINUX_PULSE__ -D__LINUX__ -DHAVE_GETTIMEOFDAY)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} asound pulse pulse-simple)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

if (${APPLE})
	add_definitions(-D__MACOSX_CORE__ -DHAVE_GETTIMEOFDAY)
	find_library(COREAUDIO_LIB CoreAudio)
	find_library(COREFOUNDATION_LIB CoreFoundation)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${COREAUDIO_LIB} ${COREFOUNDATION_LIB})
	set_target_properties(AudMeS PROPERTIES
		MACOSX_BUNDLE TRUE
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
	)
	set(CMAKE_MACOSX_RPATH ON)
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
	set(CMAKE_INSTALL_RPATH /Applications/AudMes/Contents/FRAMEWORKS)
	set(CPACK_BINARY_DRAGNDROP ON)
endif()

if (MINGW)
	add_definitions(-D__WINDOWS_DS__ -D__WXMSW__)
	target_link_libraries(${PROJECT_NAME} -mwindows -static ${wxWidgets_LIBRARIES} dsound)
endif()

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	install(TARGETS "AudMeS" DESTINATION "bin")
	install(FILES "audmes.desktop" DESTINATION "share/applications")
	install(FILES "README.md" DESTINATION "share/doc/audmes")
	install(FILES "NEWS" DESTINATION "share/doc/audmes")
	install(FILES "audmes.png" DESTINATION "share/pixmaps")
	install(FILES "audmes.png" DESTINATION "share/icons/hicolor/128x128/apps/")

	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_VERSION "2021-03-14")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "swwa")
	set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "pulseaudio, libwxgtk3.0-0v5")
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://sourceforge.net/projects/audmes/")
endif()

if (MINGW)
    set(CMAKE_INSTALL_PREFIX ".")
	install(TARGETS "AudMeS" DESTINATION "bin")
	install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION "bin")
	install(FILES "${CMAKE_SOURCE_DIR}/NEWS" RENAME "NEWS.txt" DESTINATION "bin")
	install(FILES "c:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libwinpthread-1.dll" DESTINATION "bin")
	install(FILES "c:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libstdc++-6.dll" DESTINATION "bin")
	install(FILES "c:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin/libgcc_s_sjlj-1.dll" DESTINATION "bin")
	install(FILES "${wxWidgets_ROOT_DIR}/lib/gcc810_dll/wxbase30u_gcc810.dll" DESTINATION "bin")
	install(FILES "${wxWidgets_ROOT_DIR}/lib/gcc810_dll/wxmsw30u_core_gcc810.dll" DESTINATION "bin")
	set(CPACK_GENERATOR "ZIP")
endif()
INCLUDE(CPack)
