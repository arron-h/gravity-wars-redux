LOCAL_PATH := $(realpath $(call my-dir)/../../../..)
PROJROOT := $(LOCAL_PATH)

ASSETDIR := $(PROJROOT)/Build/Android/assets

include $(CLEAR_VARS)

LOCAL_MODULE    := GravityWarsRedux

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := \
				   Source/Android/AppImpl.cpp \
				   Source/Android/AudioImpl.cpp \
				   Source/Android/BitmapImpl.cpp \
				   Source/Android/GraphicsImpl.cpp \
				   Source/Android/ParticleSystemImpl.cpp \
				   Source/Android/ResourceImpl.cpp \
				   Source/Android/ThreadImpl.cpp \
				   Source/Android/UtilsImpl.cpp \
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
				   Source/ViewManager.cpp \
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
				   Source/PVRTools/PVRTTexture.cpp \
				   Source/PVRTools/PVRTVertex.cpp \
				   Source/PVRTools/PVRTVector.cpp \
				   Source/PVRTools/PVRTTrans.cpp \
				   Source/PVRTools/PVRTString.cpp \
				   Source/PVRTools/PVRTResourceFile.cpp \
				   Source/PVRTools/PVRTQuaternionF.cpp \
				   Source/PVRTools/PVRTModelPOD.cpp \
				   Source/PVRTools/PVRTMatrixF.cpp \
				   Source/PVRTools/PVRTError.cpp \
				   Source/PVRTools/PVRTBoneBatch.cpp \

LOCAL_C_INCLUDES :=	\
				    $(PROJROOT)/Source	\
				    $(PROJROOT)/Source/PVRTools	\
				    $(PROJROOT)/Source/Framework	\
				    $(PROJROOT)/Source/Resources	\
				    $(PROJROOT)/libs/libpng \

LOCAL_CFLAGS := -DBUILD_OGLES2 -DPLATFORM_ANDROID -D_DEBUG -DDEBUG -DBUILD_DEBUG

LOCAL_LDLIBS :=  \
				-llog \
				-landroid \
				-lEGL \
				-lGLESv2 \
				-lz

LOCAL_STATIC_LIBRARIES := android_native_app_glue \
						  libpng

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
