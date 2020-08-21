cmake_minimum_required(VERSION 2.6)
project(danmaku2ass_native)

set(SOURCE_FILES
    danmaku2ass.cpp
    danmaku2ass.h
    rapidxml/rapidxml_iterators.hpp
    rapidxml/rapidxml_print.hpp
    rapidxml/rapidxml_utils.hpp
    rapidxml/rapidxml.hpp
    AssClass.cpp
    AssClass.hpp
    )
    
add_library(danmaku2ass ${SOURCE_FILES})