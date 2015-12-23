cmake_minimum_required (VERSION 2.8.3)
project(Critter)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)


if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp")
  message(FATAL_ERROR "The Instant Meshes dependency repositories (AssImp, FreeImage, libzip etc.) are missing! "
    "You probably did not clone the project with --recursive. It is possible to recover "
    "by calling \"git submodule update --init --recursive\"")
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio")
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4005 /wd4996 /wd4477 /wd4267 /wd4244 /nologo" ) 

# Sanitize build environment for static build with C++11
if (MSVC)
  add_definitions (/D "_UNICODE")
  add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
  add_definitions (/D "__TBB_NO_IMPLICIT_LINKAGE")

  # Parallel build on MSVC (all targets)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

  if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

    # Disable Eigen vectorization for Windows 32 bit builds (issues with unaligned access segfaults)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DEIGEN_DONT_ALIGN")
  endif()

  # Static build
  set(CompilerFlags
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif()

# Defines.


include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/application
  ${CMAKE_CURRENT_SOURCE_DIR}/codecs
  ${CMAKE_CURRENT_SOURCE_DIR}/input
  ${CMAKE_CURRENT_SOURCE_DIR}/math
  ${CMAKE_CURRENT_SOURCE_DIR}/newui
  ${CMAKE_CURRENT_SOURCE_DIR}/nodes
  ${CMAKE_CURRENT_SOURCE_DIR}/renderAPI
  ${CMAKE_CURRENT_SOURCE_DIR}/resources
  ${CMAKE_CURRENT_SOURCE_DIR}/swizzling
  ${CMAKE_CURRENT_SOURCE_DIR}/ui
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/include/assimp
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/assimp/code
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/cmdLine
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/FreeImage
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/FreeImage/Source
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MurmerHash
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libzip/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libzip/src
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/nanovg
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/pugixml/src
  ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/zlib
  ${CMAKE_CURRENT_SOURCE_DIR}/rendererD3D11 
  ${CMAKE_CURRENT_SOURCE_DIR}/rendererD3D11/effectsD3D11
  ${CMAKE_CURRENT_SOURCE_DIR}/rendererD3D11/effectsD3D11/Binary
  ${CMAKE_CURRENT_SOURCE_DIR}/rendererD3D11/effectsD3D11/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/rendererD3D11/errorD3D11
  ${CMAKE_CURRENT_SOURCE_DIR}/resources
  ${CMAKE_CURRENT_SOURCE_DIR}/swizzling
  ${CMAKE_CURRENT_SOURCE_DIR}/ui
  )


# Compile with compiler warnings
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
  endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types")
  endif()
endif()

add_library(Critter STATIC 
            application/CtrApplication.cpp
            application/CtrApplication.h
            application/CtrHash.h
            application/CtrHash.cpp
            application/CtrLog.h
            application/CtrLog.cpp
            application/CtrMath.h
            application/CtrNonCopyable.h
            application/CtrPlatform.h
            application/CtrTimer.cpp
            application/CtrTimer.h
            application/CtrTitles.cpp
            application/CtrTitles.h
            application/CtrWindow.cpp
            application/CtrWindow.h
            codecs/CtrBitwise
            codecs/CtrCodec.cpp
            codecs/CtrCodec.h
            codecs/CtrColorValue.cpp
            codecs/CtrColorValue.h
            codecs/CtrDataStream.cpp
            codecs/CtrDataStream.h
            codecs/CtrDDSCodec.cpp
            codecs/CtrDDSCodec.h
            codecs/CtrFreeImageCodec.cpp
            codecs/CtrFreeImageCodec.h
            codecs/CtrImageCodec.h
            codecs/CtrImageResampler.h
            codecs/CtrIteratorRange.h
            codecs/CtrIteratorWrapper.h
            codecs/CtrPixelConversions.h
            codecs/CtrPixelFormat.cpp
            codecs/CtrPixelFormat.h
            codecs/CtrStringUtilities.cpp
            codecs/CtrStringUtilities.h
            codecs/CtrTextureImage.cpp
            codecs/CtrTextureImage.h
            dependencies/cmdLine/CmdLine.h
            dependencies/imgui/droidsans.ttf.h
            dependencies/imgui/imconfig.h
            dependencies/imgui/imgui.cpp
            dependencies/imgui/imgui.h
            dependencies/imgui/stb_image.c
            dependencies/imgui/stb_rect_pack.h
            dependencies/imgui/stb_textedit.h
            dependencies/imgui/stb_truetype.h
            dependencies/MurmerHash/MurmurHash.h
            dependencies/MurmerHash/MurmurHash.cpp
            dependencies/nanovg/fontstash.h
            dependencies/nanovg/nanovg.h
            dependencies/nanovg/nanovg.c
            dependencies/pugixml/src/pugiconfig.hpp
            dependencies/pugixml/src/pugixml.cpp
            dependencies/pugixml/src/pugixml.hpp
            dependencies/pugixml/src/pugixpath.cpp
            input/CtrCameraManager.cpp
            input/CtrCameraManager.h
            input/CtrDampenedInput.cpp
            input/CtrDampenedInput.h
            input/CtrFocusedInput.h
            input/CtrFocusedInput.cpp
            input/CtrFocusedMovement.cpp
            input/CtrFocusedMovement.h
            input/CtrInput.cpp
            input/CtrInput.h
            input/CtrInputManager.cpp
            input/CtrInputManager.h
            input/CtrInputState.cpp
            input/CtrInputState.h
            input/CtrX360Controller.cpp
            input/CtrX360Controller.h
            math/CtrColor.h
            math/CtrLimits.h
            math/CtrMatrix44.h
            math/CtrMatrixAlgo.h
            math/CtrQuaternion.h
            math/CtrRegion.h
            math/CtrVector2.h
            math/CtrVector3.h
            math/CtrVector4.h
            newui/Ctrimgui.cpp
            newui/Ctrimgui.h
            newui/Ctrnanovg.cpp
            newui/CtrUIRenderer.cpp
            newui/CtrUIRenderer.h
            newui/ocornut_imgui.cpp
            newui/ocornut_imgui.h
            nodes/CtrBrdf.cpp
            nodes/CtrBrdf.h
            nodes/CtrCamera.cpp
            nodes/CtrCamera.h
            nodes/CtrEntity.cpp
            nodes/CtrEntity.h
            nodes/CtrIndexedMesh.cpp
            nodes/CtrIndexedMesh.h
            nodes/CtrMesh.cpp
            nodes/CtrMesh.h
            nodes/CtrNode.cpp
            nodes/CtrNode.h
            nodes/CtrProjectionProperty.cpp
            nodes/CtrProjectionProperty.h
            nodes/CtrProperty.cpp
            nodes/CtrProperty.h
            nodes/CtrRenderNode.cpp
            nodes/CtrRenderNode.h
            nodes/CtrRenderTargetQuad.cpp
            nodes/CtrRenderTargetQuad.h
            nodes/CtrRenderTextureProperty.cpp
            nodes/CtrRenderTextureProperty.h
            nodes/CtrScene.cpp
            nodes/CtrScene.h
            nodes/CtrStreamedMesh.cpp
            nodes/CtrStreamedMesh.h
            nodes/CtrTransformNode.cpp
            nodes/CtrTransformNode.h
            nodes/CtrTransformProperty.cpp
            nodes/CtrTransformProperty.h
            nodes/CtrTypedProperty.h
            nodes/CtrViewportProperty.cpp
            nodes/CtrViewportProperty.h
            nodes/CtrViewProperty.cpp
            nodes/CtrViewProperty.h
            renderAPI/CtrAssetManager.cpp
            renderAPI/CtrAssetManager.h
            renderAPI/CtrColorPass.cpp
            renderAPI/CtrColorPass.h
            renderAPI/CtrColorResolve.cpp
            renderAPI/CtrColorResolve.h
            renderAPI/CtrDepthResolve.cpp
            renderAPI/CtrDepthResolve.h
            renderAPI/CtrFileChangeWatcher.cpp
            renderAPI/CtrFileChangeWatcher.h
            renderAPI/CtrFilterCubemap.h
            renderAPI/CtrFrameBuffer.cpp
            renderAPI/CtrFrameBuffer.h
            renderAPI/CtrGpuConstantBuffer.cpp
            renderAPI/CtrGpuConstantBuffer.h
            renderAPI/CtrGpuTechnique.cpp
            renderAPI/CtrGpuTechnique.h
            renderAPI/CtrGpuVariable.cpp
            renderAPI/CtrGpuVariable.h
            renderAPI/CtrHDRPresentationPolicy.cpp
            renderAPI/CtrHDRPresentationPolicy.h
            renderAPI/CtrIBLProbe.cpp
            renderAPI/CtrIBLProbe.h
            renderAPI/CtrIBLRenderPass.cpp
            renderAPI/CtrIBLRenderPass.h
            renderAPI/CtrIComputeShader.cpp
            renderAPI/CtrIComputeShader.h
            renderAPI/CtrIDepthSurface.cpp
            renderAPI/CtrIDepthSurface.h
            renderAPI/CtrIDevice.cpp
            renderAPI/CtrIDevice.h
            renderAPI/CtrIEffect.cpp
            renderAPI/CtrIEffect.h
            renderAPI/CtrIGpuBuffer.cpp
            renderAPI/CtrIGpuBuffer.h
            renderAPI/CtrIIndexBuffer.cpp
            renderAPI/CtrIIndexBuffer.h
            renderAPI/CtrIRenderResource.cpp
            renderAPI/CtrIRenderResource.h
            renderAPI/CtrIRenderResourceParameters.cpp
            renderAPI/CtrIRenderResourceParameters.h
            renderAPI/CtrIShader.cpp
            renderAPI/CtrIShader.h
            renderAPI/CtrISurface.cpp
            renderAPI/CtrISurface.h
            renderAPI/CtrITexture.cpp
            renderAPI/CtrITexture.h
            renderAPI/CtrIVertexBuffer.cpp
            renderAPI/CtrIVertexBuffer.h
            renderAPI/CtrIVertexDeclaration.cpp
            renderAPI/CtrIVertexDeclaration.h
            renderAPI/CtrMaterial.cpp
            renderAPI/CtrMaterial.h
            renderAPI/CtrPostEffect.cpp
            renderAPI/CtrPostEffect.h
            renderAPI/CtrPostEffectsMgr.cpp
            renderAPI/CtrPostEffectsMgr.h
            renderAPI/CtrPresentationPolicy.cpp
            renderAPI/CtrPresentationPolicy.h
            renderAPI/CtrRenderEnums.h
            renderAPI/CtrRenderPass.cpp
            renderAPI/CtrRenderPass.h
            renderAPI/CtrRenderRequest.cpp
            renderAPI/CtrRenderRequest.h
            renderAPI/CtrScreenOrientedQuad.cpp
            renderAPI/CtrScreenOrientedQuad.h
            renderAPI/CtrShaderMgr.cpp
            renderAPI/CtrShaderMgr.h
            renderAPI/CtrShaderParameterValue.cpp
            renderAPI/CtrShaderParameterValue.h
            renderAPI/CtrShaderParameterValueFactory.cpp
            renderAPI/CtrShaderParameterValueFactory.h
            renderAPI/CtrTextureMgr.cpp
            renderAPI/CtrTextureMgr.h
            renderAPI/CtrVertexDeclarationMgr.cpp
            renderAPI/CtrVertexDeclarationMgr.h
            renderAPI/CtrVertexElement.cpp
            renderAPI/CtrVertexElement.h
            renderAPI/CtrVertexStream.cpp
            renderAPI/CtrVertexStream.h
            renderAPI/CtrViewport.cpp
            renderAPI/CtrViewport.h
            rendererD3D11/effectsD3D11/d3dxGlobal.cpp
            rendererD3D11/effectsD3D11/Effect.h
            rendererD3D11/effectsD3D11/EffectAPI.cpp
            rendererD3D11/effectsD3D11/EffectLoad.cpp
            rendererD3D11/effectsD3D11/EffectLoad.h
            rendererD3D11/effectsD3D11/EffectNonRuntime.cpp
            rendererD3D11/effectsD3D11/EffectReflection.cpp
            rendererD3D11/effectsD3D11/EffectRuntime.cpp
            rendererD3D11/effectsD3D11/EffectVariable.inl
            rendererD3D11/effectsD3D11/Binary/EffectBinaryFormat.h
            rendererD3D11/effectsD3D11/Binary/EffectStateBase11.h
            rendererD3D11/effectsD3D11/Binary/EffectStates11.h
            rendererD3D11/effectsD3D11/Binary/SOParser.h
            rendererD3D11/effectsD3D11/Inc/d3dx11dbg.h
            rendererD3D11/effectsD3D11/Inc/d3dx11effect.h
            rendererD3D11/effectsD3D11/Inc/d3dxGlobal.h
            rendererD3D11/errorD3D11/dxerr.cpp
            rendererD3D11/errorD3D11/DxErr.h
            rendererD3D11/CtrBackbufferSurfaceD3D11.cpp
            rendererD3D11/CtrBackbufferSurfaceD3D11.h
            rendererD3D11/CtrBufferD3D11.cpp
            rendererD3D11/CtrBufferD3D11.h
            rendererD3D11/CtrComputeShaderD3D11.cpp
            rendererD3D11/CtrComputeShaderD3D11.h
            rendererD3D11/CtrDepthSurfaceD3D11.cpp
            rendererD3D11/CtrDepthSurfaceD3D11.h
            rendererD3D11/CtrEffectD3D11.cpp
            rendererD3D11/CtrEffectD3D11.h
            rendererD3D11/CtrFormatConversionD3D11.cpp
            rendererD3D11/CtrFormatConversionD3D11.h
            rendererD3D11/CtrGpuConstantBufferD3D11.cpp
            rendererD3D11/CtrGpuConstantBufferD3D11.h
            rendererD3D11/CtrGpuTechniqueD3D11.cpp
            rendererD3D11/CtrGpuTechniqueD3D11.h
            rendererD3D11/CtrGpuVariableD3D11.cpp
            rendererD3D11/CtrGpuVariableD3D11.h
            rendererD3D11/CtrIndexBufferD3D11.cpp
            rendererD3D11/CtrIndexBufferD3D11.h
            rendererD3D11/CtrInputLayoutCacheD3D11.cpp
            rendererD3D11/CtrInputLayoutCacheD3D11.h
            rendererD3D11/CtrRenderDeviceD3D11.cpp
            rendererD3D11/CtrRenderDeviceD3D11.h
            rendererD3D11/CtrRenderTargetTextureD3D11.cpp
            rendererD3D11/CtrRenderTargetTextureD3D11.h
            rendererD3D11/CtrRenderWindow.cpp
            rendererD3D11/CtrRenderWindow.h
            rendererD3D11/CtrRenderWindowD3D11.cpp
            rendererD3D11/CtrRenderWindowD3D11.h
            rendererD3D11/CtrShaderD3D11.cpp
            rendererD3D11/CtrShaderD3D11.h
            rendererD3D11/CtrSurfaceD3D11.cpp
            rendererD3D11/CtrSurfaceD3D11.h
            rendererD3D11/CtrTextureD3D11.cpp
            rendererD3D11/CtrTextureD3D11.h
            rendererD3D11/CtrTexture2DD3D11.cpp
            rendererD3D11/CtrTexture2DD3D11.h
            rendererD3D11/CtrVertexBufferD3D11.cpp
            rendererD3D11/CtrVertexBufferD3D11.h
            rendererD3D11/CtrVertexDeclarationD3D11.cpp
            rendererD3D11/CtrVertexDeclarationD3D11.h
            resources/critter.rc
            resources/critter.res
            resources/resource.h
            resources/small.ico
            resources/windowIcon.ico
            swizzling/CtrImageConversion.h
            swizzling/CtrImageFunctionNode.h
            ui/CtrImageWidget.cpp
            ui/CtrImageWidget.h
            ui/CtrRenderHud.cpp
            ui/CtrRenderHud.h
            )
set_target_properties(Critter PROPERTIES FOLDER "Application")
set_target_properties(Critter PROPERTIES COMPILE_DEFINITIONS "IBL_USE_ASS_IMP_AND_FREEIMAGE=1;DIRECTINPUT_VERSION=0x0800;_SCL_SECURE_NO_WARNINGS=1;_CRT_SECURE_NO_WARNINGS=1")

if (WIN32)
  # Quench some warnings on MSVC
  if (MSVC)
    set_source_files_properties(ext/rply/rply.c PROPERTIES COMPILE_FLAGS "/wd4127")
  endif()
endif()

