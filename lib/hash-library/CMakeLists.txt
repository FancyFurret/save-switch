set(TARGET_NAME hash-library)

file(GLOB SRC_FILES include/hash-library/*.cpp)
add_library(${TARGET_NAME} ${SRC_FILES} include/endian.h)
