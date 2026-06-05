armino_component_register(
    SRCS "src/overlay_anchor.c"
    REQUIRES bk_init
)

armino_build_get_property(python PYTHON)

set(linker_script "${CMAKE_BINARY_DIR}/armino/${ARDUINO_BEKEN_TARGET}/${ARDUINO_BEKEN_TARGET}_out.ld")
set(linker_patch_stamp "${CMAKE_CURRENT_BINARY_DIR}/arduino_beken_linker_patch.stamp")

add_custom_command(
    OUTPUT "${linker_patch_stamp}"
    COMMAND ${python} "${CMAKE_CURRENT_LIST_DIR}/../../tools/patch_linker_script.py" --script "${linker_script}"
    COMMAND ${CMAKE_COMMAND} -E touch "${linker_patch_stamp}"
    DEPENDS "${linker_script}" "${CMAKE_CURRENT_LIST_DIR}/../../tools/patch_linker_script.py"
    COMMENT "Patching ${ARDUINO_BEKEN_TARGET}_out.ld for C++ constructor tables"
    VERBATIM
)

add_custom_target(arduino_beken_linker_patch DEPENDS "${linker_patch_stamp}")
add_dependencies(${COMPONENT_LIB} arduino_beken_linker_patch)
