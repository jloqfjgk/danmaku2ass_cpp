//
//  danmaku2ass.hpp
//  bilibili
//
//  Created by TYPCN on 2015/6/7.
//  Copyright (c) 2016 TYPCN. All rights reserved.
//

#ifndef bilibili_danmaku2ass_h
#define bilibili_danmaku2ass_h

#include <string>
#include <vector>
#include "AssClass.h"

namespace Danmaku2ASS {

/*
 Convert comments to .ass subtitle
  infile: comment file path
  outfile: output file path
  width: video width
  height: video height
  font: font name
  alpha: comment alpha
  duration_marquee:Duration of scrolling comment
  duration_still:Duration of still comment
 */
void run(const char *infile,
         const char *outfile,
         int width,
         int height,
         const char *font,
         int fontsize,
         double alpha,
         int duration_marquee,
         int duration_still);


enum CommentType
{
    COMMENT_TYPE_UNKNOWN = 0,
    COMMENT_TYPE_ACFUN = 1,
    COMMENT_TYPE_BILIBILI = 2,
    COMMENT_TYPE_NICONICO = 3
};


class CommentParser {
private:
    std::vector<const char *> m_blockWords;
    const char *m_inFile;
    const char *m_outFile;
    const char *m_font = "Heiti";
    int m_width = 1280;
    int m_height = 720;
    int m_fontSize = 20;
    int m_durationMarquee = 5;
    int m_durationStill = 5;
    int m_disallowMode = DISALLOW_NONE;
    double m_alpha = 0.8;

    bool _convertBilibili();

public:
    inline void setFile(const char *inFile, const char *outFile) { m_inFile = inFile; m_outFile = outFile; }
    inline void setResolution(int width, int height) { m_width = width; m_height = height; }
    inline void setFont(const char *font, int size) { m_font = font; m_fontSize = size; }
    inline void setAlpha(double alpha) { m_alpha = alpha; }
    inline void setDuration(int scroll,int still) { m_durationMarquee = scroll; m_durationStill = still; }
    inline void setDisallowMode(int mode) { m_disallowMode = mode; }
    inline void addBlockWord(const char *word) { m_blockWords.push_back(word); }
    bool convert(int type);
};

}

#endif
