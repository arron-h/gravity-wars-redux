LOCAL_PATH := $(realpath $(call my-dir)/../../..)
PROJROOT := $(LOCAL_PATH)

ASSETDIR := $(PROJROOT)/Build/Android/assets

include $(CLEAR_VARS)

LOCAL_MODULE    := GWRedux

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := \
				   Source/App.cpp \
				   Source/GameData.cpp \
				   Source/PerlinNoise.cpp \
				   Source/Planet.cpp \
				   Source/Player.cpp \
				   Source/Projectile.cpp \
				   Source/Spaceship.cpp \
				   Source/StarfieldGenerator.cpp \
				   Source/stdafx.cpp \
				   Source/Strings.cpp \
				   Source/ViewAppLoader.cpp \
				   Source/ViewGame.cpp \
				   Source/ViewGameFinished.cpp \
				   Source/ViewGameLoader.cpp \
				   Source/ViewGUI.cpp \
				   Source/ViewMenu.cpp \
				   Source/ViewParticles.cpp \
				   Source/App.cpp \
				   Source/Framework/AHFont.cpp \
				   Source/Framework/Bitmap.cpp \
				   Source/Framework/Camera.cpp \
				   Source/Framework/CollisionBitmap.cpp \
				   Source/Framework/Graphics.cpp \
				   Source/Framework/HashMap.cpp \
				   Source/Framework/Interpolator.cpp \
				   Source/Framework/ParticleSystem.cpp \
				   Source/Framework/Primitives.cpp \
				   Source/Framework/Resource.cpp \
				   Source/Framework/ViewManager.cpp \
				   Source/PVRTools/PVRTTexture.cpp \
				   Source/PVRTools/PVRTVertex.cpp \
				   Source/PVRTools/PVRTVector.cpp \
				   Source/PVRTools/PVRTTrans.cpp \
				   Source/PVRTools/PVRTString.cpp \
				   Source/PVRTools/PVRTResourceFile.cpp \
				   Source/PVRTools/PVRTQuaternionF.cpp \
				   Source/PVRTools/PVRTModelPOD.cpp \
				   Source/PVRTools/PVRTMemoryFileSystem.cpp \
				   Source/PVRTools/PVRTMatrixF.cpp \
				   Source/PVRTools/PVRTError.cpp \
				   Source/PVRTools/PVRTBoneBatch.cpp \

LOCAL_C_INCLUDES :=	\
				    $(PROJROOT)/Source	\
				    $(PROJROOT)/Source	\

LOCAL_CFLAGS := -DBUILD_OGLES2 -DPLATFORM_ANDROID


LOCAL_LDLIBS :=  \
				-llog \
				-landroid \
				-lEGL \
				-lGLESv2

LOCAL_STATIC_LIBRARIES := \
				          android_native_app_glue 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
