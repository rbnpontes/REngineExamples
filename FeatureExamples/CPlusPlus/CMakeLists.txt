

# Define source files
file (GLOB SOURCE_FILES Source/*.cpp Source/*.h)

if (WIN32)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif()

if (IOS)
  set (OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Artifacts")
  create_package("${ATOMIC_SOURCE_DIR}/Resources/CoreData" "${OUTPUT_DIR}/CoreData.pak")
  create_package("${ATOMIC_SOURCE_DIR}/Resources/PlayerData" "${OUTPUT_DIR}/PlayerData.pak")
  create_package("${ATOMIC_SOURCE_DIR}/Submodules/EngineExamples/FeatureExamples/CPlusPlus/Data" "${OUTPUT_DIR}/Data.pak")

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
else()
  add_executable(FeatureExamples ${SOURCE_FILES})
endif()

target_link_libraries(FeatureExamples Atomic)

if(NOT IOS OR NOT ANDROID)
  # Copy AtomicTool to Artifacts
  add_custom_command( TARGET FeatureExamples POST_BUILD
                      COMMAND "${CMAKE_COMMAND}"
                      ARGS -E make_directory \"${ATOMIC_SOURCE_DIR}/Artifacts/Examples/CPlusPlus\"
                      COMMAND "${CMAKE_COMMAND}"
                      ARGS -E copy_if_different \"$<TARGET_FILE:FeatureExamples>\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Examples/CPlusPlus/\"
                      COMMENT "Copying FeatureExamples to Build Artifacts" )
endif ()
if (WIN32)

  # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
  add_custom_command (TARGET FeatureExamples POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:FeatureExamples>/D3DCompiler_47.dll\"
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Examples/CPlusPlus/D3DCompiler_47.dll\" )

endif(WIN32)
