

# Define source files
file (GLOB SOURCE_FILES Source/*.cpp Source/*.h)

if (WIN32)
    add_definitions(-DATOMIC_WIN32_CONSOLE)
endif()

if (IOS)
  add_executable(FeatureExamples MACOSX_BUNDLE ${SOURCE_FILES})
  set_target_properties(FeatureExamples
      PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME \${PRODUCT_NAME}
                  MACOSX_BUNDLE_GUI_IDENTIFIER com.rengine.\${PRODUCT_NAME:rfc1034identifier:lower}
  )
else()
  add_executable(FeatureExamples ${SOURCE_FILES})
endif()

target_link_libraries(FeatureExamples Atomic)

# Copy AtomicTool to Artifacts
add_custom_command( TARGET FeatureExamples POST_BUILD
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E make_directory \"${ATOMIC_SOURCE_DIR}/Artifacts/Examples/CPlusPlus\"
                    COMMAND "${CMAKE_COMMAND}"
                    ARGS -E copy_if_different \"$<TARGET_FILE:FeatureExamples>\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Examples/CPlusPlus/\"
                    COMMENT "Copying FeatureExamples to Build Artifacts" )

if (WIN32)

  # pre-Windows 8 can't count on D3DCompiler_47.dll being on system
  add_custom_command (TARGET FeatureExamples POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"$<TARGET_FILE_DIR:FeatureExamples>/D3DCompiler_47.dll\"
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_if_different \"${D3DCOMPILER_47_DLL}\" \"${ATOMIC_SOURCE_DIR}/Artifacts/Examples/CPlusPlus/D3DCompiler_47.dll\" )

endif(WIN32)
