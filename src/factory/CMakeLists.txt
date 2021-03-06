CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(FACTORY)

FILE(GLOB_RECURSE FACTORY_SRC
		"${PROJECT_SOURCE_DIR}/src/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/*.h"
		"${PROJECT_SOURCE_DIR}/include/agz/factory/*.h"
		"${PROJECT_SOURCE_DIR}/include/agz/factory/*.inl")

ADD_LIBRARY(Factory STATIC ${FACTORY_SRC})

FOREACH(_SRC IN ITEMS ${FACTORY_SRC})
    GET_FILENAME_COMPONENT(FACTORY_SRC "${_SRC}" PATH)
    STRING(REPLACE "${PROJECT_SOURCE_DIR}/include/agz/factory" "include" _GRP_PATH "${FACTORY_SRC}")
    STRING(REPLACE "${PROJECT_SOURCE_DIR}/src" "src" _GRP_PATH "${_GRP_PATH}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()

SET_PROPERTY(TARGET Factory PROPERTY CXX_STANDARD 17)
SET_PROPERTY(TARGET Factory PROPERTY CXX_STANDARD_REQUIRED ON)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	IF(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
		SET(LINKER_FLAGS "-lc++fs -ldl -pthread")
	ELSE()
		SET(LINKER_FLAG "-lstdc++fs -ldl -pthread")
	ENDIF()
ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(LINKER_FLAGS "-lstdc++fs -ldl -pthread")
ENDIF()

TARGET_INCLUDE_DIRECTORIES(Factory PUBLIC "${PROJECT_SOURCE_DIR}/include/")
TARGET_LINK_LIBRARIES(Factory PUBLIC Tracer)
