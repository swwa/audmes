cmake_minimum_required (VERSION 3.10)

project(AudMeS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPP_SOURCES
	AudMeS.cpp
	AudMeS.h
	CtrlOScope.cpp
	CtrlOScope.h
	dlg_audiointerface.cpp
	dlg_audiointerface.h
	RWAudio_IO.cpp
	RWAudio_IO.h
	sma_2d.cpp
	sma_2d.h
	)

set(AM_SOURCES
	${CPP_SOURCES}
	rtaudio/RtAudio.cpp
	rtaudio/RtAudio.h
	fourier.c
	fourier.h
	)

add_subdirectory (tests)

include_directories(.) # for local libfccp

# Default build type is Release
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "Release")
endif()
message("Build type is ${CMAKE_BUILD_TYPE}")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif()

if (WIN32)
	set(AM_SOURCES ${AM_SOURCES} audmes.rc)
	set(wxWidgets_USE_LIBS ON)
endif()

if (MINGW)
	execute_process(COMMAND bash -c "wx-config --prefix | tr -d '\n'" OUTPUT_VARIABLE MINGW_DIR)
	message("wxWidgets dir settings: ${MINGW_DIR}")
endif()

if (${APPLE})
	set(audmes_icon "${CMAKE_CURRENT_SOURCE_DIR}/audmes.icns")
	set(AM_SOURCES ${audmes_icon} ${AM_SOURCES})
endif()

find_package(wxWidgets REQUIRED core base)
include("${wxWidgets_USE_FILE}")
message("wxWidgets lib settings: ${wxWidgets_LIBRARIES}")

find_path(FCCP
	libfccp/csv.h
	HINTS .
	)
if (NOT FCCP)
	message(FATAL_ERROR "Could not find libfccp, please see README.md.")
endif()
message("libfccp found ${FCCP}")

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${AM_SOURCES})

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
	set(CMAKE_OSX_DEPLOYMENT_TARGET 15)
	# Create app Bundle but skip signing
	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE TRUE
		XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
		)
endif()

if (MSVC)
	add_definitions(-D__WINDOWS_WASAPI__ -D__WINDOWS_DS__)
	set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
	target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} dsound ksuser mfplat mfuuid wmcodecdspuuid winmm ole32)
	if (EXISTS "${CMAKE_BINARY_DIR}/Release")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxbase30u_vc142_x64.dll" "Release"
		COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxmsw30u_core_vc142_x64.dll" "Release")
	endif()
	if (EXISTS "${CMAKE_BINARY_DIR}/Debug")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxbase30ud_vc142_x64.dll" "Debug"
		COMMAND ${CMAKE_COMMAND} -E copy "${wxWidgets_LIB_DIR}/wxmsw30ud_core_vc142_x64.dll" "Debug")
	endif()
endif()

if (MINGW)
	add_definitions(-D__WINDOWS_WASAPI__ -D__WINDOWS_DS__)
	target_link_libraries(
		${PROJECT_NAME}
		${wxWidgets_LIBRARIES}
		dsound ksuser mfplat mfuuid wmcodecdspuuid winmm ole32
		)
endif()

set(CPACK_PACKAGE_VERSION_MAJOR "2025")
set(CPACK_PACKAGE_VERSION_MINOR "04")
set(CPACK_PACKAGE_VERSION_PATCH "05")

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
	install(TARGETS ${PROJECT_NAME} DESTINATION "bin")
	install(FILES "audmes.desktop" DESTINATION "share/applications")
	install(FILES "net.sourceforge.audmes.audmes.appdata.xml" DESTINATION "share/appdata")
	install(FILES "README.md" DESTINATION "share/doc/audmes")
	install(FILES "NEWS" DESTINATION "share/doc/audmes")
	install(FILES "audmes.png" DESTINATION "share/pixmaps")
	install(FILES "audmes.png" DESTINATION "share/icons/hicolor/128x128/apps/")

	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "swwa")
	set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://sourceforge.net/projects/audmes/")
endif()

if (MINGW)
	#if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	#	message(WARNING "Installation only in **Release** builds supported")
	#endif()
	set(CMAKE_INSTALL_PREFIX .)
	install(TARGETS ${PROJECT_NAME} DESTINATION .)
	install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)
	install(FILES "${CMAKE_SOURCE_DIR}/NEWS" RENAME "NEWS.txt" DESTINATION .)
	# add runtime libraries - static linking does not work with wxWidgets binaries
	install(FILES "${MINGW_DIR}/bin/libgcc_s_dw2-1.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libstdc++-6.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libwinpthread-1.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/wxbase32u_gcc_custom.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/wxmsw32u_core_gcc_custom.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/liblzma-5.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/zlib1.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libjpeg-8.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libpcre2-16-0.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libpng16-16.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libtiff-6.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libdeflate.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libjbig-0.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libLerc.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libzstd.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libwebp-7.dll" DESTINATION .)
	install(FILES "${MINGW_DIR}/bin/libsharpyuv-0.dll" DESTINATION .)
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
		fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\")"
		)
	# Add files to top level dmg directoy
	install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION .)
	install(FILES "${CMAKE_SOURCE_DIR}/NEWS" DESTINATION .)
	set(CPACK_GENERATOR "DragNDrop")
endif()

add_custom_target(
	clang-tidy-check cd ..\; clang-tidy -p ${CMAKE_BINARY_DIR}/compile_commands.json ${CPP_SOURCES} DEPENDS ${CPP_SOURCES}
	)

add_custom_target(check DEPENDS clang-tidy-check)

INCLUDE(CPack)
