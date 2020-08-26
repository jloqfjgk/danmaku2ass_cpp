cmake_minimum_required(VERSION 2.6)

include_directories(include)

set(DANMAKU2ASS_SRC
    src/AssBuilder.cpp
    src/CommentParser.cpp
    src/danmaku2ass.cpp
    )
    
add_library(danmaku2ass ${DANMAKU2ASS_SRC})