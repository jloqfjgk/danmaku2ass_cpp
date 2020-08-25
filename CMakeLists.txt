cmake_minimum_required(VERSION 2.6)

set(DANMAKU2ASS_SRC
    danmaku2ass.cpp
    CommentParser.cpp
    AssClass.cpp
    )
    
add_library(danmaku2ass ${DANMAKU2ASS_SRC})