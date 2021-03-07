project(flv)


# Add static and shared library target
add_library(flv_static STATIC ${COMMON_SRC} ${VCODEC_SRC} ${ACODEC_SRC} ${FLV_SRC})
add_library(flv_shared SHARED ${COMMON_SRC} ${VCODEC_SRC} ${ACODEC_SRC} ${FLV_SRC})

# Set library output name
set_target_properties(flv_static PROPERTIES OUTPUT_NAME flv)
set_target_properties(flv_shared PROPERTIES OUTPUT_NAME flv)

set_target_properties(flv_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(flv_shared PROPERTIES CLEAN_DIRECT_OUTPUT 1)





