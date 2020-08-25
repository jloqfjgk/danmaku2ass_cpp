//
//  AssClass.h
//  danmaku2ass_native
//
//  Created by TYPCN on 2015/4/8.
//
//

#ifndef __danmaku2ass_native__AssClass__
#define __danmaku2ass_native__AssClass__

#include <stdio.h>
#include <fstream>
#include <vector>
#include <map>

namespace Danmaku2ASS {

const int DISALLOW_NONE = 0;
const int DISALLOW_SCROLL = 1;
const int DISALLOW_TOP = 2;
const int DISALLOW_BOTTOM = 4;

class Ass{
private:
    std::ofstream m_outStream;
    std::map <double, std::pair<int, std::string>> m_commentMap;
    std::string m_styleName;
    int m_durationMarquee;
    int m_durationStill;
    int m_width;
    int m_height;
    int m_fontSize;
    
public:
    void init(const char *filename);
    void setDuration(int dm, int ds);
    void writeHead(int width, int height, const char *font, int fontsize, double alpha);
    void appendComment(double appear_time, int comment_mode, int font_color, const char *content);
    void writeToDisk(int disallowMode);
};

}

#endif /* defined(__danmaku2ass_native__AssClass__) */
