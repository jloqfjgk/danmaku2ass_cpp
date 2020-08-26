//
//  AssBuilder.h
//  danmaku2ass_native
//
//  Created by TYPCN on 2015/4/8.
//
//

#ifndef __danmaku2ass_native__AssBuilder__
#define __danmaku2ass_native__AssBuilder__

#include <fstream>
#include <map>
#include <memory>
#include <vector>

namespace Danmaku2ASS
{

    const int DISALLOW_NONE = 0;
    const int DISALLOW_SCROLL = 1;
    const int DISALLOW_TOP = 2;
    const int DISALLOW_BOTTOM = 4;

    class AssBuilder
    {

    public:
        typedef std::shared_ptr<AssBuilder> Ptr;
        
        inline void setDuration(int dm, int ds)
        {
            m_durationMarquee = dm;
            m_durationStill = ds;
        }
        inline void setDisallowMode(int mode) { m_disallowMode = mode; }
        inline void addBlockWord(const std::string &word) { m_blockWords.push_back(word); }

        void writeHead(int width, int height, const std::string& font, int fontsize, double alpha);
        void appendComment(double appear_time, int comment_mode, int font_color, const std::string& content);

        void exportAss(std::ostream& output);
        void exportAssToFile(const std::string& filename);

    private:
        // <playbackTime, <source, result, textWidth>>
        std::map<double, std::tuple<std::string, std::string, int>> m_commentMap;

        std::vector<std::string> m_blockWords;
        std::string m_styleName;
        std::string m_assHead;
        int m_disallowMode = 0;
        int m_durationMarquee = 5;
        int m_durationStill = 5;
        int m_width = 1280;
        int m_height = 720;
        int m_fontSize = 24;
    };

} // namespace Danmaku2ASS

#endif /* defined(__danmaku2ass_native__AssBuilder__) */
