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
        
        AssBuilder(const std::string& filename);
        void setDuration(int dm, int ds);
        void writeHead(int width, int height, const std::string& font, int fontsize, double alpha);
        void appendComment(double appear_time, int comment_mode, int font_color, const std::string& content);
        void writeToDisk(int disallowMode);

    private:
        std::ofstream m_outStream;
        std::map<double, std::pair<int, std::string>> m_commentMap;
        std::string m_styleName;
        int m_durationMarquee;
        int m_durationStill;
        int m_width;
        int m_height;
        int m_fontSize;
    };

} // namespace Danmaku2ASS

#endif /* defined(__danmaku2ass_native__AssBuilder__) */
