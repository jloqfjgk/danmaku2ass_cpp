#include "danmaku2ass.h"
#include <fstream>
#include <sstream>
#include "CommentParser.h"

void Danmaku2ASS::parseFile(const char *infile, const char *outfile,int width,int height,const char *font,int fontsize,double alpha,int duration_marquee,int duration_still)
{
    std::ifstream input(infile);
    CommentParser p(input);
    p.setResolution(width, height);
    p.setFont(font, fontsize);
    p.setDuration(duration_marquee, duration_still);
    p.setAlpha(alpha);

    AssBuilder::Ptr ass = p.convert();
    ass->exportAssToFile(outfile);

    input.close();
}

void Danmaku2ASS::parseString(const char *instr, const char *outfile, int width, int height, const char *font, int fontsize, double alpha, int duration_marquee, int duration_still)
{
    std::stringstream input(instr);
    CommentParser p(input);
    p.setResolution(width, height);
    p.setFont(font, fontsize);
    p.setDuration(duration_marquee, duration_still);
    p.setAlpha(alpha);

    AssBuilder::Ptr ass = p.convert();
    ass->exportAssToFile(outfile);
}
