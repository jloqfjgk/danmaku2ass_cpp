//
//  AssClass.cpp
//  danmaku2ass_native
//
//  Created by TYPCN on 2015/4/8.
//
//

#include "AssClass.h"
#include <string.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <time.h>

using namespace Danmaku2ASS;

// Useful functions
static void stripStr(std::string& in)
{
    std::replace(in.begin(), in.end(), '\r', '\n');
    std::replace(in.begin(), in.end(), '\n', ' ');
}

static inline std::string replaceAll(std::string str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

static int roundInt(double r)
{
    return static_cast<int>((r > 0.0) ? (r + 0.5) : (r - 0.5));
}

template <typename _Iterator1, typename _Iterator2>
static inline size_t incUtf8StringIterator(_Iterator1 &it, const _Iterator2 &last)
{
    if (it == last)
        return 0;
    unsigned char c;
    size_t res = 1;
    for (++it; last != it; ++it, ++res)
    {
        c = *it;
        if (!(c & 0x80) || ((c & 0xC0) == 0xC0))
            break;
    }

    return res;
}

static inline int utf8StringSize(const std::string &str)
{
    int res = 0;
    std::string::const_iterator it = str.begin();
    for (; it != str.end(); incUtf8StringIterator(it, str.end()))
        res++;
    return res;
}

static inline std::string ts2t(double timestamp)
{

    int ts = (int)timestamp * 100;
    int hour, minute, second, centsecond;
    hour = ts / 360000;
    minute = ts % 360000;
    second = minute % 6000;
    minute = minute / 6000;
    centsecond = second % 100;
    second = second / 100;
    std::stringstream ss;
    ss << hour;
    ss << ':' << std::setfill('0') << std::setw(2) << minute;
    ss << ':' << std::setfill('0') << std::setw(2) << second;
    ss << '.' << std::setfill('0') << std::setw(2) << centsecond;
    return ss.str();
}



Ass::Ass(const char *filename){
    std::remove(filename);
    m_outStream.open(filename);
}

void Ass::setDuration(int dm, int ds){
    m_durationMarquee = dm;
    m_durationStill = ds;
}

void Ass::writeHead(int width, int height, const char *font, int fontsize, double alpha){
    
    srand((int)time(0));
    
    m_fontSize = fontsize;
    m_height = height;
    m_width = width;
    
    // Write a♂ss head info
    m_outStream << "[Script Info]\nScript Updated By: Danmaku2ASS_native (https://github.com/typcn/danmaku2ass_native)\nScriptType: v4.00+" << std::endl;
    m_outStream << "PlayResX: " << width << std::endl;
    m_outStream << "PlayResY: " << height << std::endl;
    m_outStream << "Aspect Ratio: " << width << ":" << height << std::endl;
    m_outStream << "Collisions: Normal" << std::endl;
    m_outStream << "WrapStyle: 2" << std::endl;
    m_outStream << "ScaledBorderAndShadow: yes" << std::endl;
    m_outStream << "YCbCr Matrix: TV.601" << std::endl << std::endl;

    std::stringstream hex_builder;
    hex_builder << std::hex << std::setfill('0') << std::setw(2) << (255 - roundInt(alpha * 255));
    std::string alpha_hex = hex_builder.str();
    
    // Write ass styles , maybe disorder , See https://en.wikipedia.org/wiki/SubStation_Alpha
    m_outStream << "[V4+ Styles]" << std::endl;
    m_outStream << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding" << std::endl;

    // Get Style name
    std::stringstream ss;
    ss << "TYCM_" << rand() % (int)(9999 + 1);
    m_styleName = ss.str();
    
    m_outStream << "Style: " <<
        m_styleName <<  "," << // Style name
        font << ", " << // Font name
        fontsize << ", " << // Font size
        "&H"<< alpha_hex <<"FFFFFF, " << // Primary Color
        "&H"<< alpha_hex <<"FFFFFF, " << // Secondary Color
        "&H"<< alpha_hex <<"000000, " << // Outline Color
        "&H"<< alpha_hex <<"000000, " << // Back Color
        "0, 0, 0, 0, 100, 100, 0.00, 0.00, 1, 1, 0, 7, 0, 0, 0, 0" << std::endl << std::endl;

    m_outStream << "[Events]" << std::endl;
    m_outStream << "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text" << std::endl;
}

void Ass::appendComment(double appear_time,int comment_mode,int font_color,const char *content){

    std::string str = content;
    stripStr(str);
    
    int strLength = utf8StringSize(str);
    if(strLength > 100){
        return;
    }
    
    int contentFontLen = strLength * m_fontSize;

    std::stringstream effect;
    int duration;
    if(comment_mode < 4){
        effect << "\\move(" << m_width << ", [MROW], " << -contentFontLen << ", [MROW])";
        duration = m_durationMarquee;
    }else if(comment_mode == 4){
        effect << "\\an2\\pos(" << (m_width / 2) << ",[BottomROW])";
        duration = m_durationStill;
    }else if(comment_mode == 5){
        effect << "\\an8\\pos(" << (m_width / 2) << ",[TopROW])";
        duration = m_durationStill;
    }else{
        return;
    }

    std::string color = "";
    if(font_color != 16777215){
        if(font_color == 0x000000){
            color = "\\c&H000000&";
        }else if(font_color == 0xffffff){
            color = "\\c&HFFFFFF&";
        }else{
            int R = (font_color >> 16) & 0xff;
            int G = (font_color >> 8) & 0xff;
            int B = font_color & 0xff;
            std::stringstream hex_builder;
            hex_builder << std::hex << std::setfill('0') << std::setw(2) << B << G << R;
            color = "\\c&H" + hex_builder.str() + "&";
        }
    }

    std::stringstream ss;
    ss << "Dialogue: 2," << ts2t(appear_time) << "," << ts2t(appear_time + duration) << "," << m_styleName << ",,0000,0000,0000,,{" << effect.str() << color << "}" << str;

    std::pair<int, std::string> contentPair = make_pair(contentFontLen, ss.str());
    m_commentMap.insert(std::pair<double, std::pair<int,std::string>>(appear_time, contentPair));
    
}

void Ass::writeToDisk(int disallowMode){
    
    int All_Rows = 0;
    int Dropped_Rows = 0;
    
    double TopTime = 0;
    double BottomTime = 0;
    
    int TopROW = -1;
    int BottomROW = -1;
    
    int line = m_height / m_fontSize;

    std::vector<double> rows_dismiss_time(line, 0); // The time of scroll comment dismiss
    std::vector<double> rows_visible_time(line, 0); // The time of last char visible on screen
    
    for(auto iterator = m_commentMap.cbegin(); iterator != m_commentMap.cend(); iterator++) {
        
        All_Rows++;

        std::string r = iterator->second.second;

        double playbackTime = iterator->first;
        double TextWidth = iterator->second.first + 2.0; // Add some space between texts
        double act_time = TextWidth / (((double) m_width + TextWidth)/ (double) m_durationMarquee); // duration of last char visible on screen
        
        if(r.find("[MROW]") != std::string::npos) {
            if(disallowMode & DISALLOW_SCROLL) {
                continue;
            }
            bool Replaced = false;
            for(int i=0;i < line;i++) {
                double Time_Arrive_Border = (playbackTime + (double) m_durationMarquee) - act_time; // The time of first char reach left border of video
                if(Time_Arrive_Border > rows_dismiss_time[i] && playbackTime > rows_visible_time[i]){
                    rows_dismiss_time[i] = playbackTime + (double) m_durationMarquee;
                    rows_visible_time[i] = playbackTime + act_time;
                    r = replaceAll(r,"[MROW]", std::to_string(i * m_fontSize));
                    Replaced = true;
                    break;
                }
            }
            if(!Replaced){
                r = "";
                Dropped_Rows++;
            }
        }else if(r.find("[TopROW]") != std::string::npos) {
            if(disallowMode & DISALLOW_TOP) {
                continue;
            }
            double timeago =  iterator->first - TopTime;
            if(timeago > m_durationStill) {
                TopROW = 0;
                TopTime = iterator->first;
            } else {
                TopROW++;
            }
            r = replaceAll(r,"[TopROW]",std::to_string(TopROW * m_fontSize));
        } else if(r.find("[BottomROW]") != std::string::npos) {
            if(disallowMode & DISALLOW_BOTTOM){
                continue;
            }else{
                double timeago =  iterator->first - BottomTime;
                if(timeago > m_durationStill){
                    BottomROW = 0;
                    BottomTime = iterator->first;
                }else{
                    BottomROW++;
                }
                r = replaceAll(r, "[BottomROW]", std::to_string(m_height - BottomROW * m_fontSize));
            }
        } else {
            continue;
        }
        
        if(r.length() < 10) {
            Dropped_Rows++;
            continue;
        }

        m_outStream << r << std::endl;
    }

    std::cout << "Comments:" << All_Rows << " Dropped:" << Dropped_Rows << std::endl;
}
