cmake_minimum_required(VERSION 2.6)

include_directories(include)

set(DANMAKU2ASS_SRC
    src/AssBuilder.cpp
    src/CommentParser.cpp
    src/danmaku2ass.cpp
    )
    
add_library(Danmaku2AssCPP ${DANMAKU2ASS_SRC})

add_executable(danmaku2ass src/main.c)
target_link_libraries(danmaku2ass Danmaku2AssCPP)