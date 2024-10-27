cmake_minimum_required(VERSION 3.20)

set (CMAKE_CXX_STANDARD 20)

set (BOOST_ROOT "D:/Netology/Libs/boost/boost_1_86_0/boost_1_86_0") # Путь к библиотеке Boost
set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)


set(SOURCE_FILES "boost_beast_async_client_http.cpp" )
#set(SOURCE_FILES "crawl.cpp" )


project(Async_test)


find_package (Boost REQUIRED)

if (Boost_FOUND)
    include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
endif ()

add_executable(Async_test ${SOURCE_FILES} )

target_compile_features(Async_test PRIVATE cxx_std_17)

target_link_libraries(Async_test PRIVATE ${Boost_LIBRARIES})


