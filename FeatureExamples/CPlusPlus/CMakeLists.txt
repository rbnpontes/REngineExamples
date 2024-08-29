

# Define source files
file (GLOB SOURCE_FILES Source/*.cpp Source/*.h)

add_subdirectory(Source/SamplePlugin)

if (WIN32)
    add_definitions(-DENGINE_WIN32_CONSOLE)
endif()

# iOS we must pack assets
if(IOS)
  set (OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Artifacts")
  create_package("${ENGINE_SOURCE_DIR}/Resources/CoreData" "${OUTPUT_DIR}/CoreData.pak")
  create_package("${ENGINE_SOURCE_DIR}/Resources/PlayerData" "${OUTPUT_DIR}/PlayerData.pak")
  create_package("${ENGINE_SOURCE_DIR}/Submodules/EngineExamples/FeatureExamples/CPlusPlus/Data" "${OUTPUT_DIR}/Data.pak")
endif()

if (IOS)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "16.6" CACHE STRING "Minimum OS X deployment version")
  set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET})
  # build ios
  add_executable(FeatureExamples MACOSX_BUNDLE ${SOURCE_FILES})
  set_target_properties(FeatureExamples
      PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME \${PRODUCT_NAME}
                  MACOSX_BUNDLE_GUI_IDENTIFIER com.rengine.\${PRODUCT_NAME:rfc1034identifier:lower}
  )
  # embed engine resources
  set (RESOURCE_LIST 
    "${OUTPUT_DIR}/CoreData.pak"
    "${OUTPUT_DIR}/PlayerData.pak"
    "${OUTPUT_DIR}/Data.pak")
  target_sources(FeatureExamples PRIVATE ${RESOURCE_LIST})
  set_source_files_properties(${RESOURCE_LIST} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
elseif(ANDROID)
  add_library(FeatureExamples SHARED ${SOURCE_FILES})
else()
  add_executable(FeatureExamples ${SOURCE_FILES})
endif()

add_dependencies(FeatureExamples SamplePlugin)
target_link_libraries(FeatureExamples ${ENGINE_CORE_LIB_TARGET})