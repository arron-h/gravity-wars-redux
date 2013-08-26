LOCAL_PATH := $(realpath $(call my-dir)/../../../..)
PROJROOT := $(LOCAL_PATH)

ASSETDIR := $(PROJROOT)/Build/Android/assets

CPY := cp
SEPARATOR := /
ifeq ($(HOST_OS),windows)
CPY := copy
SEPARATOR := \\
endif

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
				   ../JAASL/jaasl.cpp \
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
				    $(PROJROOT)/../JAASL

LOCAL_CFLAGS := -DBUILD_OGLES2 -DPLATFORM_ANDROID -D_DEBUG -DDEBUG -DBUILD_DEBUG

LOCAL_LDLIBS :=  \
				-llog \
				-landroid \
				-lEGL \
				-lGLESv2 \
				-lz \
				-lOpenSLES

LOCAL_STATIC_LIBRARIES := android_native_app_glue \
						  libpng

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

### Copy our external files to the assets folder, but only do it for the first abi
ifeq ($(TARGET_ARCH_ABI),$(firstword $(NDK_APP_ABI)))

all:  \
	$(ASSETDIR)/courbd_18.fnt \
	$(ASSETDIR)/courbd_18.png \
	$(ASSETDIR)/courbd_36.fnt \
	$(ASSETDIR)/courbd_36.png \
	$(ASSETDIR)/courbd_64.fnt \
	$(ASSETDIR)/courbd_64.png \
	$(ASSETDIR)/dso01.png \
	$(ASSETDIR)/dso02.png \
	$(ASSETDIR)/dso03.png \
	$(ASSETDIR)/dso04.png \
	$(ASSETDIR)/dso05.png \
	$(ASSETDIR)/dso06.png \
	$(ASSETDIR)/dso07.png \
	$(ASSETDIR)/earth.png \
	$(ASSETDIR)/explosiondisc.png \
	$(ASSETDIR)/gameinput.wav \
	$(ASSETDIR)/gamemusic.mp3 \
	$(ASSETDIR)/glow.png \
	$(ASSETDIR)/jupiter.png \
	$(ASSETDIR)/keyboard-down-1024.png \
	$(ASSETDIR)/keyboard-down-512.png \
	$(ASSETDIR)/keyboard-up-1024.png \
	$(ASSETDIR)/keyboard-up-512.png \
	$(ASSETDIR)/mars.png \
	$(ASSETDIR)/menu-galaxy-bg.png \
	$(ASSETDIR)/menu-galaxy-bg.pvr \
	$(ASSETDIR)/menu.wav \
	$(ASSETDIR)/menumusic.mp3 \
	$(ASSETDIR)/mercury.png \
	$(ASSETDIR)/particle-cloud.png \
	$(ASSETDIR)/particle-cloud2.png \
	$(ASSETDIR)/particle.png \
	$(ASSETDIR)/playerindicator.png \
	$(ASSETDIR)/projectile-trail.png \
	$(ASSETDIR)/projectile.png \
	$(ASSETDIR)/spaceship-collision.tga \
	$(ASSETDIR)/spaceship.png \
	$(ASSETDIR)/star.tga \
	$(ASSETDIR)/star1.tga \
	$(ASSETDIR)/star2.tga \
	$(ASSETDIR)/star3.tga \
	$(ASSETDIR)/test.tga \
	$(ASSETDIR)/venus.png \
	$(ASSETDIR)/MVP_V_C.frag \
	$(ASSETDIR)/MVP_V_C.vert \
	$(ASSETDIR)/MVP_V_T_C.frag \
	$(ASSETDIR)/MVP_V_T_C.vert \
	$(ASSETDIR)/MVP_V_T_C_N.frag \
	$(ASSETDIR)/MVP_V_T_C_N.vert \
	$(ASSETDIR)/Planet.frag \
	$(ASSETDIR)/Planet.vert
	

$(ASSETDIR):
	-mkdir "$(ASSETDIR)"

$(ASSETDIR)/%.pvr: $(PROJROOT)/Program/%.pvr $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")

$(ASSETDIR)/%.png: $(PROJROOT)/Program/%.png $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")
	
$(ASSETDIR)/%.fnt: $(PROJROOT)/Program/%.fnt $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")

$(ASSETDIR)/%.wav: $(PROJROOT)/Program/%.wav $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")
	
$(ASSETDIR)/%.mp3: $(PROJROOT)/Program/%.mp3 $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")
	
$(ASSETDIR)/%.tga: $(PROJROOT)/Program/%.tga $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")

$(ASSETDIR)/%.vert: $(PROJROOT)/Shaders/GLES/%.vert $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")
	
$(ASSETDIR)/%.frag: $(PROJROOT)/Shaders/GLES/%.frag $(ASSETDIR)
	$(CPY) $(subst /,$(SEPARATOR),"$<" "$(ASSETDIR)")
		
endif
