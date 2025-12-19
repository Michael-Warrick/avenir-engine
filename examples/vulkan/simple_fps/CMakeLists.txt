add_executable(simple_fps main.cpp FPSController.cpp)

target_link_libraries(simple_fps PRIVATE avenir)

function(add_slang_shader_target_simple_fps TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})

    # Quick check to ensure we actually have shaders to compile
    if (NOT SHADER_SOURCES)
        message(FATAL_ERROR "add_slang_shader_target: no shader sources provided!")
    endif ()

    # Find slang compiler
    set(SLANGC_EXECUTABLE "$ENV{VULKAN_SDK}/bin/slangc")
    message(STATUS "SLANGC_EXECUTABLE = ${SLANGC_EXECUTABLE}")

    # Dealing only with vertex and fragment pipelines
    set(ENTRY_POINTS -entry vertMain -entry fragMain)

    set(OUTPUTS)

    foreach (SHADER_SOURCE_REL IN LISTS SHADER_SOURCES)
        # Get absolute path to source file
        set(SHADER_SOURCE ${CMAKE_CURRENT_LIST_DIR}/${SHADER_SOURCE_REL})

        # Retain subdirectory structure (i.e., "resources/shaders") and extract shader name
        get_filename_component(SHADER_SUB_DIR ${SHADER_SOURCE_REL} DIRECTORY)
        get_filename_component(SHADER_NAME ${SHADER_SOURCE_REL} NAME_WE)

        # Set output directory to be in source-mirrored binary directory
        set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/${SHADER_SUB_DIR})
        set(SHADER_OUTPUT ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv)

        add_custom_command(
                OUTPUT ${SHADER_OUTPUT}
                COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
                COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCE} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o ${SHADER_OUTPUT}
                DEPENDS ${SHADER_SOURCE}
                COMMENT "Compiling Slang Shaders"
                VERBATIM
        )

        list(APPEND OUTPUTS ${SHADER_OUTPUT})
    endforeach ()

    add_custom_target(${TARGET} DEPENDS ${OUTPUTS})
endfunction()

add_slang_shader_target_simple_fps(simple_fps_shader SOURCES resources/shaders/shader.slang)
add_dependencies(simple_fps simple_fps_shader)