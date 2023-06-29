cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (AUTH_SERVICE_BINARY "auth-service")
SET (USER_SERVICE_BINARY "user-service")
SET (BLOG_SERVICE_BINARY "blog-service")
SET (MESSANGER_SERVICE_BINARY "messanger-service")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

add_executable(${AUTH_SERVICE_BINARY}
    services/auth-service/main.cpp 
    services/auth-service/auth-config/auth-config.cpp
    database/database-config/database-config.cpp
    database/database.cpp
    database/user-auth.cpp
)

target_include_directories(${AUTH_SERVICE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${AUTH_SERVICE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${AUTH_SERVICE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${AUTH_SERVICE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${AUTH_SERVICE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

add_executable(${USER_SERVICE_BINARY}
    services/user-service/main.cpp 
    services/user-service/user-config/user-config.cpp
    database/database-config/database-config.cpp
    database/database.cpp
    database/user.cpp
)

target_include_directories(${USER_SERVICE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${USER_SERVICE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${USER_SERVICE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${USER_SERVICE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${USER_SERVICE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

add_executable(${BLOG_SERVICE_BINARY}
    services/blog-service/main.cpp 
    services/blog-service/blog-config/blog-config.cpp
    database/database-config/database-config.cpp
    database/database.cpp
    database/blog-post.cpp
)

target_include_directories(${BLOG_SERVICE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${BLOG_SERVICE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${BLOG_SERVICE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${BLOG_SERVICE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${BLOG_SERVICE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

add_executable(${MESSANGER_SERVICE_BINARY}
    services/messanger-service/main.cpp 
    services/messanger-service/messanger-config/messanger-config.cpp
    database/database-config/database-config.cpp
    database/database.cpp
    database/chat-message.cpp
)

target_include_directories(${MESSANGER_SERVICE_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${MESSANGER_SERVICE_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${MESSANGER_SERVICE_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${MESSANGER_SERVICE_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${MESSANGER_SERVICE_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)
