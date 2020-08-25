#ifndef __danmaku2ass_native__CommentParser__
#define __danmaku2ass_native__CommentParser__

#include <istream>
#include <vector>
#include "AssClass.h"

namespace Danmaku2ASS
{

    enum CommentType
    {
        COMMENT_TYPE_UNKNOWN = 0,
        COMMENT_TYPE_ACFUN = 1,
        COMMENT_TYPE_BILIBILI = 2,
        COMMENT_TYPE_NICONICO = 3
    };


    class CommentParser
    {
    private:
        std::vector<const char *> m_blockWords;
        std::istream &m_inStream;
        const char *m_outFile;
        const char *m_font;
        int m_width = 1280;
        int m_height = 720;
        int m_fontSize = 20;
        int m_durationMarquee = 5;
        int m_durationStill = 5;
        int m_disallowMode = DISALLOW_NONE;
        double m_alpha = 0.8;

        bool _convertBilibili();

    public:
        CommentParser(std::istream &source);
        inline void setOutputFile(const char *outFile) { m_outFile = outFile; }
        inline void setResolution(int width, int height)
        {
            m_width = width;
            m_height = height;
        }
        inline void setFont(const char *font, int size)
        {
            m_font = font;
            m_fontSize = size;
        }
        inline void setAlpha(double alpha) { m_alpha = alpha; }
        inline void setDuration(int scroll, int still)
        {
            m_durationMarquee = scroll;
            m_durationStill = still;
        }
        inline void setDisallowMode(int mode) { m_disallowMode = mode; }
        inline void addBlockWord(const char *word) { m_blockWords.push_back(word); }
        bool convert();
    };

} // namespace Danmaku2ASS
#endif