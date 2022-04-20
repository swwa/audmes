cmake_minimum_required (VERSION 3.1)

project(AudMeS)

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

# Default build type is Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "Release")
endif()
message("Build type is ${CMAKE_BUILD_TYPE}")

if (WIN32)
	set(AM_SOURCES ${AM_SOURCES} audmes.rc)
	string(REPLACE "\\" "/" HDIR "$ENV{HOMEDRIVE}$ENV{HOMEPATH}")
	set(wxWidgets_ROOT_DIR "${HDIR}/projects/wx3.0.5")
endif()

if (${APPLE})
	set(audmes_icon "${CMAKE_CURRENT_SOURCE_DIR}/audmes.icns")
	set(AM_SOURCES ${audmes_icon} ${AM_SOURCES})
endif()

find_package(wxWidgets REQUIRED core base)
include("${wxWidgets_USE_FILE}")

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${AM_SOURCES})

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
	set_source_files_properties(${audmes_icon} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	# Create app Bundle but skip signing
	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE TRUE
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
	)
endif()

if (MINGW)
	add_definitions(-D__WINDOWS_WASAPI__ -D__WINDOWS_DS__ -D__WXMSW__)
	target_link_libraries(${PROJECT_NAME} -mwindows ${wxWidgets_LIBRARIES} dsound ksuser mfplat mfuuid wmcodecdspuuid)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxbase30u_gcc810.dll" "."
	COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxbase30ud_gcc810.dll" "."
	COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxmsw30u_core_gcc810.dll" "."
	COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxmsw30ud_core_gcc810.dll" ".")
endif()

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	install(TARGETS ${PROJECT_NAME} DESTINATION "bin")
	install(FILES "audmes.desktop" DESTINATION "share/applications")
	install(FILES "README.md" DESTINATION "share/doc/audmes")
	install(FILES "NEWS" DESTINATION "share/doc/audmes")
	install(FILES "audmes.png" DESTINATION "share/pixmaps")
	install(FILES "audmes.png" DESTINATION "share/icons/hicolor/128x128/apps/")

	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_VERSION "2022-04-20")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "swwa")
	set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "pulseaudio, libwxgtk3.0-0v5")
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://sourceforge.net/projects/audmes/")
endif()

if (MINGW)
	if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		message(WARNING "Installation only in **Release** builds supported")
	endif()
	set(CMAKE_INSTALL_PREFIX .)
	install(TARGETS ${PROJECT_NAME} DESTINATION .)
	install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)
	install(FILES "${CMAKE_SOURCE_DIR}/NEWS" RENAME "NEWS.txt" DESTINATION .)
	# add runtime libraries - static linking does not work with wxWidgets binaries
	set(MINGW_LIBS "c:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-sjlj-rt_v6-rev0/mingw32/bin")
	install(FILES "${MINGW_LIBS}/libwinpthread-1.dll" DESTINATION .)
	install(FILES "${MINGW_LIBS}/libstdc++-6.dll" DESTINATION .)
	install(FILES "${MINGW_LIBS}/libgcc_s_sjlj-1.dll" DESTINATION .)
	install(FILES "${wxWidgets_LIB_DIR}/wxbase30u_gcc810.dll" DESTINATION .)
	install(FILES "${wxWidgets_LIB_DIR}/wxmsw30u_core_gcc810.dll" DESTINATION .)
	set(CPACK_GENERATOR "ZIP")
endif()

if (APPLE)
	set(APP_NAME ${PROJECT_NAME})
	# Install app and libraries
	install(TARGETS ${APP_NAME}
		BUNDLE DESTINATION . COMPONENT Runtime
		RUNTIME DESTINATION bin COMPONENT Runtime
	)
	set(APPS "\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app")
	set(CMAKE_MACOSX_RPATH ON)
	set(DIRS ${CMAKE_BINARY_DIR})
	# Replace absolut library paths with relative to app paths
	install(CODE "include(BundleUtilities)
		set(BU_CHMOD_BUNDLE_ITEMS ON)
		fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")")
	# Add files to top level dmg directoy
	install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)
	install(FILES "${CMAKE_SOURCE_DIR}/NEWS" DESTINATION .)
	set(CPACK_GENERATOR "DragNDrop")
endif()

INCLUDE(CPack)
