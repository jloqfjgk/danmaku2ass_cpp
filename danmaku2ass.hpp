//
//  danmaku2ass.hpp
//  bilibili
//
//  Created by TYPCN on 2015/6/7.
//  Copyright (c) 2016 TYPCN. All rights reserved.
//

#ifndef bilibili_danmaku2ass_hpp
#define bilibili_danmaku2ass_hpp

#include "danmaku2ass.h"
#include <string>
#include <vector>

int GetCommentType(std::string headline);

class CommentParser{
private:
    std::vector<const char *> blockWords;
    std::vector<int> disallowModes;
    const char *in;
    const char *out;
    int width = 1280;
    int height = 720;
    const char *font = "Heiti";
    int fontsize = 20;
    double alpha = 0.8;
    int duration_marquee = 5;
    int duration_still = 5;
    
    bool _convertBilibili();
public:
    void SetFile(const char *infile,const char *outfile){ in = infile; out = outfile; };
    void SetRes(int w,int h){ width = w; height = h; };
    void SetFont(const char *name,int size){ font = name; fontsize = size; };
    void SetAlpha(double a){ alpha = a; };
    void SetDuration(int scroll,int still){ duration_marquee = scroll; duration_still = still; };
    void SetBlockWord(const char *word){ blockWords.push_back(word); };
    void AddDisallowMode(int mode){ disallowModes.push_back(mode); }; // 1 scroll 2 top 3 bottom
    bool Convert(int type);
};

#endif
