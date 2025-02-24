cmake_minimum_required(VERSION 3.10)

# Set the project name
project(encrypt_decrypt_AES)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(encrypt_decrypt_AES main.cpp)

add_subdirectory(library)
target_link_libraries(${PROJECT_NAME}${EXECUTABLE_SUFFIX} PUBLIC encryption_decryption_library)

# Link libraries (if any)
# target_link_libraries(encrypt_decrypt_AES <library_name>)