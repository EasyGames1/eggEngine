function(CompileShaders TARGET_NAME SHADERS_FOLDER)
    file(GLOB_RECURSE GLSL_SOURCE_FILES
            "${SHADERS_FOLDER}/*.glsl"
            "${SHADERS_FOLDER}/*.vert"
            "${SHADERS_FOLDER}/*.frag"
            "${SHADERS_FOLDER}/*.geom"
            "${SHADERS_FOLDER}/*.tese"
            "${SHADERS_FOLDER}/*.tesc"
            "${SHADERS_FOLDER}/*.comp"
    )

    foreach (GLSL ${GLSL_SOURCE_FILES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        get_filename_component(FILE_PATH ${GLSL} PATH)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" FILE_REL_PATH ${FILE_PATH})
        set(SPIR_V "${CMAKE_CURRENT_BINARY_DIR}/${FILE_REL_PATH}/${FILE_NAME}.spv")
        add_custom_command(
                OUTPUT ${SPIR_V}
                COMMAND Vulkan::glslc ${GLSL} -o ${SPIR_V}
                DEPENDS ${GLSL}
        )
        list(APPEND SPIR_V_BINARY_FILES ${SPIR_V})
    endforeach (GLSL)

    add_custom_target(
            Shaders
            DEPENDS ${SPIR_V_BINARY_FILES}
    )

    add_dependencies(${TARGET_NAME} Shaders)
endfunction(CompileShaders)