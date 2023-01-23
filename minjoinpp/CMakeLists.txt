cmake_minimum_required(VERSION 3.10)
project(minsimjoin)

#set(CMAKE_CXX_STANDARD 23)
#add_compile_options(-O2 -Wall -Wextra -pedantic -Werror -std=c++17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wpedantic -O3 -lpthread")

add_executable(minsimjoin src/main.cpp)
