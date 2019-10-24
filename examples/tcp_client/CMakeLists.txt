cmake_minimum_required (VERSION 2.6)
project (tcp_client)

# 包含头文件
include_directories(./include)

# 包含库

# 包含源文件
# set(CFILE_SRC src/main.c)
# set(CFILE_SRC ${CFILE_SRC} main.c)

aux_source_directory(./src CFILE_SRC)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -g)

add_executable(tcp_client ${CFILE_SRC})
