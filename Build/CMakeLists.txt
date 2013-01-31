cmake_minimum_required(VERSION 2.8)
project(GravityWarsRedux)

set(SRC_DIR				${CMAKE_SOURCE_DIR}/../Source)
set(SRC_FW_DIR			${CMAKE_SOURCE_DIR}/../Source/Framework)
set(SRC_PVRTOOLS_DIR	${CMAKE_SOURCE_DIR}/../Source/PVRTools)
set(SRC_IMPL_DIR		${CMAKE_SOURCE_DIR}/../Source/Win32)
set(LIB_DIR				${CMAKE_SOURCE_DIR}/../libs)

########################
# Source
set(SOURCE_GAME
	${SRC_DIR}/App.cpp
	${SRC_DIR}/GameData.cpp
	${SRC_DIR}/PerlinNoise.cpp
	${SRC_DIR}/Planet.cpp
	${SRC_DIR}/Player.cpp
	${SRC_DIR}/Projectile.cpp
	${SRC_DIR}/Spaceship.cpp
	${SRC_DIR}/StarfieldGenerator.cpp
	${SRC_DIR}/stdafx.cpp
	${SRC_DIR}/Strings.cpp
	${SRC_DIR}/ViewAppLoader.cpp
	${SRC_DIR}/ViewGame.cpp
	${SRC_DIR}/ViewGameFinished.cpp
	${SRC_DIR}/ViewGameLoader.cpp
	${SRC_DIR}/ViewGUI.cpp
	${SRC_DIR}/ViewManager.cpp
	${SRC_DIR}/ViewMenu.cpp
	${SRC_DIR}/ViewParticles.cpp)
	
set(SOURCE_FRAMEWORK
	${SRC_FW_DIR}/AHFont.cpp
	${SRC_FW_DIR}/Bitmap.cpp
	${SRC_FW_DIR}/Camera.cpp
	${SRC_FW_DIR}/CollisionBitmap.cpp
	${SRC_FW_DIR}/Graphics.cpp
	${SRC_FW_DIR}/HashMap.cpp
	${SRC_FW_DIR}/Interpolator.cpp
	${SRC_FW_DIR}/ParticleSystem.cpp
	${SRC_FW_DIR}/Primitives.cpp
	${SRC_FW_DIR}/Resource.cpp)

set(SOURCE_PVRTOOLS
	${SRC_PVRTOOLS_DIR}/PVRTBoneBatch.cpp
	${SRC_PVRTOOLS_DIR}/PVRTError.cpp
	${SRC_PVRTOOLS_DIR}/PVRTMatrixF.cpp
	${SRC_PVRTOOLS_DIR}/PVRTModelPOD.cpp
	${SRC_PVRTOOLS_DIR}/PVRTQuaternionF.cpp
	${SRC_PVRTOOLS_DIR}/PVRTResourceFile.cpp
	${SRC_PVRTOOLS_DIR}/PVRTString.cpp
	${SRC_PVRTOOLS_DIR}/PVRTTrans.cpp
	${SRC_PVRTOOLS_DIR}/PVRTVector.cpp
	${SRC_PVRTOOLS_DIR}/PVRTVertex.cpp)
	
set(SOURCE_IMPL
	${SRC_IMPL_DIR}/AppImpl.cpp
	${SRC_IMPL_DIR}/AudioImpl.cpp
	${SRC_IMPL_DIR}/BitmapImpl.cpp
	${SRC_IMPL_DIR}/GLEx.cpp
	${SRC_IMPL_DIR}/GraphicsImpl.cpp
	${SRC_IMPL_DIR}/ParticleSystemImpl.cpp
	${SRC_IMPL_DIR}/ResourceImpl.cpp
	${SRC_IMPL_DIR}/ThreadImpl.cpp
	${SRC_IMPL_DIR}/UtilsImpl.cpp)
	
########################
# Libs
find_package(OpenGL)
find_library(PNG_LIB_D		libpngd		${LIB_DIR}/libpng/bin)
find_library(PNG_LIB		libpng		${LIB_DIR}/libpng/bin)
find_library(Z_LIB_D		zlibd		${LIB_DIR}/zlib/bin)
find_library(Z_LIB			zlib		${LIB_DIR}/zlib/bin)
find_library(IRRKLANG_LIB	irrKlang	${LIB_DIR}/irrklang/lib)

########################
# Includes
include_directories("${LIB_DIR}/irrklang/include")
include_directories("${LIB_DIR}/zlib/include")
include_directories("${SRC_DIR}")
include_directories("${SRC_FW_DIR}")
include_directories("${SRC_IMPL_DIR}")
include_directories("${SRC_PVRTOOLS_DIR}")

########################
# Definitions
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} /DBUILD_DEBUG")

########################
# Executable	
add_executable(GravityWarsRedux ${SOURCE_GAME} ${SOURCE_FRAMEWORK} ${SOURCE_PVRTOOLS} ${SOURCE_IMPL})
target_link_libraries(GravityWarsRedux debug ${PNG_LIB_D} optimized ${PNG_LIB})
target_link_libraries(GravityWarsRedux debug ${Z_LIB_D} optimized ${Z_LIB})
target_link_libraries(GravityWarsRedux debug ${PNG_LIB_D} optimized ${PNG_LIB})
target_link_libraries(GravityWarsRedux ${IRRKLANG_LIB} ${OPENGL_LIBRARIES})

# glu32
	
