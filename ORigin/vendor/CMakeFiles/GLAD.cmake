add_library(GLAD STATIC ${VENDOR_DIR}/glad/src/glad.c)
target_include_directories(GLAD PRIVATE ${VENDOR_DIR}/glad/include)
