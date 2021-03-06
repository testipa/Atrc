﻿CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Atrc)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

############## optional submodules

OPTION(USE_EMBREE              "use embree for finding triangle mesh intersection" OFF)
OPTION(USE_OIDN                "use oidn denoiser"                                 OFF)
OPTION(BUILD_GUI               "build graphics user interface"                     OFF)
OPTION(BUILD_EDITOR            "build scene editor"                                OFF)
OPTION(BUILD_CLI               "build cmd-line launcher"                           ON)

############## CXX properties

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Options: Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF()

############## embree

IF(USE_EMBREE)
    FIND_PACKAGE(embree 3.0 REQUIRED)
ENDIF()

############## oidn

IF(USE_OIDN)
    ADD_DEFINITIONS(-DUSE_OIDN)
    FIND_PACKAGE(OpenImageDenoise REQUIRED)
ENDIF()

############## qt

IF(BUILD_GUI OR BUILD_EDITOR)
    FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Widgets)
    SET_PROPERTY(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP auto_gen)
    SET(CMAKE_AUTOMOC ON)
    SET(CMAKE_AUTOUIC ON)
    SET(CMAKE_AUTORCC ON)
ENDIF()

############## im3d

IF(BUILD_EDITOR)
    ADD_SUBDIRECTORY(lib/im3d)
    SET_TARGET_PROPERTIES(Im3D PROPERTIES FOLDER "ThirdParty")
ENDIF()

############## necessory submodules

SET(MISC_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/misc/)

ADD_SUBDIRECTORY(lib/spdlog)
ADD_SUBDIRECTORY(lib/utility)
SET_TARGET_PROPERTIES(spdlog AGZUtils PROPERTIES FOLDER "ThirdParty")

ADD_SUBDIRECTORY(src/tracer)
ADD_SUBDIRECTORY(src/factory)

IF(BUILD_CLI)
    ADD_SUBDIRECTORY(src/cli)
ENDIF()

IF(BUILD_GUI OR BUILD_EDITOR)
	ADD_SUBDIRECTORY(src/gui_common)
ENDIF()

IF(BUILD_GUI)
    ADD_SUBDIRECTORY(src/gui)
ENDIF()

IF(BUILD_EDITOR)
    ADD_SUBDIRECTORY(src/editor)
ENDIF()
