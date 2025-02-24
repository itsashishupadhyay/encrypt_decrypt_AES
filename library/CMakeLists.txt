# Find OpenSSL
find_package(OpenSSL REQUIRED)

set (OPEN_SSL_LOCAL_FOLDER "third-part-lib-openssl")

set(MY_LIBRARY_NAME "encryption_decryption_library")

# Create a library
add_library(${MY_LIBRARY_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source/encrypt_aes.cpp
)

# Include directories
target_include_directories(${MY_LIBRARY_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
)

# Link against OpenSSL libraries
target_link_libraries(${MY_LIBRARY_NAME}  OpenSSL::SSL OpenSSL::Crypto)

