CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

PROJECT(QDataMatrix)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0005 NEW)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "1")
#SET(VERSION_SUFFIX "")

SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)

# check for Qt4
SET(QT_MIN_VERSION "4.4.0" COMPONENTS QtCore QtGui)
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})


CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/qdatamatrix.rc.in ${CMAKE_BINARY_DIR}/qdatamatrix.rc)

# set compiler flags
SET(CMAKE_C_FLAGS "-O2 -g -Wall ${CMAKE_C_FLAGS}")
SET(CMAKE_CXX_FLAGS "-O2 -g -fno-exceptions -Wall ${CMAKE_CXX_FLAGS}")

SET(qdatamatrix_SOURCES src/main.cpp 
    src/mainwindow.cpp)

SET(qdatamatrix_INCLUDES src/mainwindow.h)

IF(WIN32)
	INSTALL(FILES
				${QT_BINARY_DIR}/QtCore4.dll
				${QT_BINARY_DIR}/QtGui4.dll
				${QT_BINARY_DIR}/dmtx.dll
				DESTINATION .)
	SET(WINRC "${CMAKE_BINARY_DIR}/qdatamatrixrc.obj")
	ADD_CUSTOM_COMMAND(OUTPUT ${WINRC}
			COMMAND ${WINDRES}
				-I${CMAKE_SOURCE_DIR}
				-o${CMAKE_BINARY_DIR}/qdatamatrixrc.obj
				-i${CMAKE_BINARY_DIR}/qdatamatrix.rc
			DEPENDS ${CMAKE_BINARY_DIR}/qdatamatrix.rc)
ENDIF(WIN32)

SET(qdatamatrix_UI forms/mainwindow.ui forms/about.ui)

QT4_WRAP_CPP(qdatamatrix_MOC_out ${qdatamatrix_INCLUDES})
QT4_WRAP_UI(qdatamatrix_UIC_out ${qdatamatrix_UI})
QT4_ADD_RESOURCES(qdatamatrix_RCC_out data/qdatamatrix.qrc)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})
LINK_LIBRARIES(${QT_LIBRARIES} -L${CMAKE_SOURCE_DIR} -ldmtx)
ADD_EXECUTABLE(qdatamatrix ${qdatamatrix_SOURCES} ${qdatamatrix_UIC_out} ${qdatamatrix_MOC_out} ${qdatamatrix_RCC_out} ${WINRC})

IF(WIN32)
	SET_TARGET_PROPERTIES(qdatamatrix PROPERTIES LINK_FLAGS "${LINK_FLAGS} -mwindows")
        ADD_CUSTOM_COMMAND(TARGET qdatamatrix POST_BUILD COMMAND ${STRIP} ${CMAKE_BINARY_DIR}/qdatamatrix.exe)
        INSTALL(TARGETS qdatamatrix RUNTIME DESTINATION .)
ELSE(WIN32)
        INSTALL(TARGETS qdatamatrix RUNTIME DESTINATION bin)
ENDIF(WIN32)

#
# add dist target
#
SET(TMP "qdatamatrix-${VERSION}")
ADD_CUSTOM_TARGET(dist
                        COMMAND make clean
                        COMMAND rm -rf ${TMP}
                        COMMAND mkdir -p ${TMP}
                        COMMAND cp AUTHORS build_mingw32 CMakeLists.txt qdatamatrix.pro COPYING INSTALL qdatamatrix.rc.in README TODO ${TMP}
                        COMMAND cp -r 3rdparty cmake data forms src ${TMP}
                        COMMAND rm -rf `find ${TMP} -name cmake_install.cmake` `find ${TMP} -name Makefile` `find ${TMP} -type d -name CMakeFiles` ${TMP}/CMakeCache.txt
                        COMMAND tar cjf qdatamatrix-${VERSION}.tar.bz2 ${TMP}
                        COMMAND rm -rf ${TMP})


INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QDataMatrix")
SET(CPACK_PACKAGE_VENDOR "Electronic Design Chemnitz (EDC) - http://www.ed-chemnitz.de")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")

SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "QDataMatrix ${VERSION}")
IF(WIN32)
SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/data\\\\qdatamatrix.ico")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "qdatamatrix.exe")
SET(CPACK_NSIS_DISPLAY_NAME "QDataMatrix")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\qdatamatrix.sourceforge.net")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\qdatamatrix.sourceforge.net")
SET(CPACK_NSIS_CONTACT "tobias.doerffel@gmail.com")
SET(CPACK_PACKAGE_EXECUTABLES "qdatamatrix.exe;QDataMatrix")
SET(CPACK_NSIS_MENU_LINKS "qdatamatrix.exe;QDataMatrix")
ELSE(WIN32)
SET(CPACK_STRIP_FILES "bin/qdatamatrix")
SET(CPACK_PACKAGE_EXECUTABLES "qdatamatrix" "QDataMatrix binary")
ENDIF(WIN32)

SET(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/data/logo.png")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "QDataMatrix")
SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_NAME "QDataMatrix")
SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}")
SET(MACOSX_BUNDLE_COPYRIGHT "Tobias Doerffel, 2009")

SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "qdatamatrix-${VERSION}")
INCLUDE(CPack)

