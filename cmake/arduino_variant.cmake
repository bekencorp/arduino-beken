function(arduino_beken_collect_variants variants_root target_soc out_var)
    file(
        GLOB variant_entries
        RELATIVE "${variants_root}"
        "${variants_root}/${target_soc}"
        "${variants_root}/${target_soc}_*"
    )

    set(generic_variants)
    set(other_variants)
    foreach(entry IN LISTS variant_entries)
        if(IS_DIRECTORY "${variants_root}/${entry}")
            if(entry MATCHES "_generic$")
                list(APPEND generic_variants "${entry}")
            else()
                list(APPEND other_variants "${entry}")
            endif()
        endif()
    endforeach()

    list(SORT generic_variants)
    list(SORT other_variants)
    set(${out_var} ${generic_variants} ${other_variants} PARENT_SCOPE)
endfunction()


function(arduino_beken_resolve_variant variants_root out_target out_variant out_dir)
    set(arduino_target "${ARMINO_SOC}")
    if(arduino_target STREQUAL "")
        set(arduino_target "$ENV{ARMINO_SOC}")
    endif()
    if(arduino_target STREQUAL "")
        message(FATAL_ERROR "ARMINO_SOC is not set. Build arduino-beken through 'make TARGET=<soc>' so the correct variant can be selected.")
    endif()

    arduino_beken_collect_variants("${variants_root}" "${arduino_target}" arduino_variant_candidates)
    if(NOT arduino_variant_candidates)
        message(FATAL_ERROR "No variant directory found for TARGET=${arduino_target} under ${variants_root}.")
    endif()

    set(arduino_variant "$ENV{ARDUINO_VARIANT}")
    if(arduino_variant STREQUAL "")
        list(GET arduino_variant_candidates 0 arduino_variant)
    endif()

    set(arduino_variant_dir "${variants_root}/${arduino_variant}")
    if(NOT IS_DIRECTORY "${arduino_variant_dir}")
        message(
            FATAL_ERROR
            "Requested Arduino variant '${arduino_variant}' does not exist under ${variants_root}."
        )
    endif()

    set(${out_target} "${arduino_target}" PARENT_SCOPE)
    set(${out_variant} "${arduino_variant}" PARENT_SCOPE)
    set(${out_dir} "${arduino_variant_dir}" PARENT_SCOPE)
endfunction()
