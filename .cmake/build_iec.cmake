set(no_dev_warnings_backup "$CACHE{CMAKE_SUPPRESS_DEVELOPER_WARNINGS}")
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON CACHE INTERNAL "" FORCE)
#------------------------------------------------------------------------------
file(READ ${PROJ_IEC_DIR}/iec-sources IEC_FILES)

STRING(REGEX REPLACE ";" "\\\\;" IEC_FILES "${IEC_FILES}")
STRING(REGEX REPLACE "\n" ";" IEC_FILES "${IEC_FILES}")
STRING(REGEX REPLACE "/;$/" "" IEC_FILES "${IEC_FILES}")
list(REMOVE_ITEM IEC_FILES POUS.c)

message("== IEC COMPILE FILES: ${IEC_FILES}")

execute_process(
    WORKING_DIRECTORY ${PROJ_IEC_DIR}
    COMMAND ${CMAKE_CXX_COMPILER} -I ${PROJ_LIB_DIR} -c ${IEC_FILES} -w
)

#------------------------------------------------------------------------------
set(no_dev_warnings_backup $CACHE{CMAKE_SUPPRESS_DEVELOPER_WARNINGS})