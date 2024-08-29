# Define source files
file (GLOB SOURCE_FILES *.cpp *.h)

add_library(SamplePlugin SHARED ${SOURCE_FILES})
target_link_libraries(SamplePlugin PRIVATE ${ENGINE_CORE_LIB_TARGET})

#set(LIB_OUT_DIR "${ENGINE_SOURCE_DIR}/Artifacts/Data")

# add_custom_command(TARGET SamplePlugin POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SamplePlugin> ${LIB_OUT_DIR}
#     COMMENT "Copying Sample Plugin to ${LIB_OUT_DIR}"
# )