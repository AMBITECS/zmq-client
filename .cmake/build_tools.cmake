#--------------------------------------------
# Expand files list '_LIST' by GLOB of '_MASK' argument
#--------------------------------------------
MACRO(MASK_LIST _LIST _MASK)
    if(${_LIST})
        foreach(INC ${${_LIST}})
            if(${INC} STREQUAL ${_MASK})
                list(FIND ${_LIST} ${_MASK} INDEX)
                list(REMOVE_AT ${_LIST} ${INDEX})
                list(GET ${_LIST} ${INDEX} _ITEM)
                if(_ITEM)
                    get_filename_component(source_dir ${_ITEM} DIRECTORY)
                    get_filename_component(source_mask ${_ITEM} NAME)
                    if(IS_DIRECTORY ${source_dir})
                        file(GLOB source_list ${source_dir}/${source_mask})
                        list(REMOVE_AT ${_LIST} ${INDEX})
                        foreach(file ${source_list})
                            list(FIND ${_LIST} ${file} idx)
                            if(idx EQUAL -1)
                                list(APPEND ${_LIST} ${file})
                            endif()
                        endforeach()
                    else()
                        MESSAGE(FATAL_ERROR "- ${source_dir} is not a directory")
                    endif()
                else()
                    MESSAGE(FATAL_ERROR "- file mask are not specified")
                endif()
            endif()
        endforeach()
    endif()
ENDMACRO(MASK_LIST)

#--------------------------------------------
# Expand files list '_LIST' by GLOB of '_FILE' argument
#--------------------------------------------
MACRO(ADD_LIST_FROM_FILE _LIST _FILE)
    file(READ ${_FILE} _FILES)
    STRING(REGEX REPLACE ";" "\\\\;" _FILES "${_FILES}")
    STRING(REGEX REPLACE "\n" ";" _FILES "${_FILES}")
    STRING(REGEX REPLACE "/;$/" "" _FILES "${_FILES}")
    set(${_LIST} ${${_LIST}} ${_FILES})
    message("== _LIST: ${_LIST}")
ENDMACRO(ADD_LIST_FROM_FILE)

MACRO(FILE_LIST _LIST _FILE)
    if(${_LIST})
        foreach(INC ${${_LIST}})
            if(${INC} STREQUAL ${_FILE})
                list(FIND ${_LIST} ${_FILE} INDEX)
                list(REMOVE_AT ${_LIST} ${INDEX})
                list(GET ${_LIST} ${INDEX} _ITEM)
                if(_ITEM)
                    ADD_LIST_FROM_FILE(${_LIST} ${_ITEM})
                endif()
            endif()
        endforeach()
    endif()
ENDMACRO(FILE_LIST)

#--------------------------------------------
# build_target function
#--------------------------------------------
function(build_target TAG_NAME)
    set(options EXE LIB)
    set(oneValueArgs TYPE)
    set(multiValueArgs SRCS PRPS OPTS DEFS INCS LNKS)
    cmake_parse_arguments(TAG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Unmask (expand) files lists
    MASK_LIST(TAG_INCS ":MASK")
    MASK_LIST(TAG_SRCS ":MASK")

    FILE_LIST(TAG_SRCS ":FILE")

    message("== TAG_SRCS: ${TAG_SRCS}")

    if (TAG_LIB)
        add_library(${TAG_NAME} ${TAG_TYPE} ${TAG_SRCS})
    elseif (TAG_EXE)
        add_executable(${TAG_NAME} ${TAG_SRCS})
    else()
        MESSAGE(FATAL_ERROR "Unknown module type")
    endif()

    if (TAG_PRPS)
        set_target_properties      (${TAG_NAME} PROPERTIES ${TAG_PRPS})
    endif()

    if (TAG_OPTS)
        target_compile_options     (${TAG_NAME} PRIVATE ${TAG_OPTS})
    endif()

    if (TAG_DEFS)
        target_compile_definitions (${TAG_NAME} PRIVATE ${TAG_DEFS})
    endif()

    if (TAG_INCS)
        target_include_directories (${TAG_NAME} PRIVATE ${TAG_INCS})
    endif()

    if(TAG_LNKS)
        target_link_libraries(${TAG_NAME} ${TAG_LNKS})
    endif()

endfunction()
