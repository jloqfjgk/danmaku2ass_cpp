#include <Danmaku2ASS/danmaku2ass.h>
#include <Danmaku2ASS/CommentParser.h>
#include <fstream>
#include <sstream>

void danmaku2ass(const char *infile, const char *outfile,int width,int height,const char *font,int fontsize,double alpha,int duration_marquee,int duration_still)
{
    std::ifstream input(infile);
    Danmaku2ASS::CommentParser p(input);
    p.setResolution(width, height);
    p.setFont(font, fontsize);
    p.setDuration(duration_marquee, duration_still);
    p.setAlpha(alpha);

    auto ass = p.convert();
    ass->exportAssToFile(outfile);

    input.close();
}
