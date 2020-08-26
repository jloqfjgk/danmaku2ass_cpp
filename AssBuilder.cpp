//
//  AssClass.cpp
//  danmaku2ass_native
//
//  Created by TYPCN on 2015/4/8.
//
//

#include "AssBuilder.h"
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
    for (auto it = str.cbegin(); it != str.cend(); incUtf8StringIterator(it, str.cend()))
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



void AssBuilder::writeHead(int width, int height, const std::string& font, int fontsize, double alpha)
{
    srand((int)time(0));
    
    m_fontSize = fontsize;
    m_height = height;
    m_width = width;
    
    // Write a♂ss head info
    std::stringstream head_builder;
    head_builder << "[Script Info]\nScript Updated By: Danmaku2ASS_native (https://github.com/typcn/danmaku2ass_native)\nScriptType: v4.00+" << std::endl;
    head_builder << "PlayResX: " << width << std::endl;
    head_builder << "PlayResY: " << height << std::endl;
    head_builder << "Aspect Ratio: " << width << ":" << height << std::endl;
    head_builder << "Collisions: Normal" << std::endl;
    head_builder << "WrapStyle: 2" << std::endl;
    head_builder << "ScaledBorderAndShadow: yes" << std::endl;
    head_builder << "YCbCr Matrix: TV.601" << std::endl << std::endl;

    std::stringstream hex_builder;
    hex_builder << std::hex << std::setfill('0') << std::setw(2) << (255 - roundInt(alpha * 255));
    std::string alpha_hex = hex_builder.str();
    
    // Write ass styles , maybe disorder , See https://en.wikipedia.org/wiki/SubStation_Alpha
    head_builder << "[V4+ Styles]" << std::endl;
    head_builder << "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding" << std::endl;

    // Get Style name
    std::stringstream ss;
    ss << "TYCM_" << rand() % (int)(9999 + 1);
    m_styleName = ss.str();
    
    head_builder << "Style: " <<
        m_styleName <<  "," << // Style name
        font << ", " << // Font name
        fontsize << ", " << // Font size
        "&H"<< alpha_hex <<"FFFFFF, " << // Primary Color
        "&H"<< alpha_hex <<"FFFFFF, " << // Secondary Color
        "&H"<< alpha_hex <<"000000, " << // Outline Color
        "&H"<< alpha_hex <<"000000, " << // Back Color
        "0, 0, 0, 0, 100, 100, 0.00, 0.00, 1, 1, 0, 7, 0, 0, 0, 0" << std::endl << std::endl;

    head_builder << "[Events]" << std::endl;
    head_builder << "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text" << std::endl;

    m_assHead = head_builder.str();
}

void AssBuilder::appendComment(double appear_time, int comment_mode, int font_color, const std::string& content)
{
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

    m_commentMap[appear_time] = std::make_tuple(str, ss.str(), contentFontLen);
}

void AssBuilder::exportAss(std::ostream& output)
{
    // Write head
    output << m_assHead;
    
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

        double playbackTime = iterator->first;
        std::string source = std::get<0>(iterator->second);
        std::string result = std::get<1>(iterator->second);
        double TextWidth = std::get<2>(iterator->second) + 2.0; // Add some space between texts

        // Skip block words
        for (const auto &word : m_blockWords)
        {
            if (source.find(word) != std::string::npos)
            {
                continue;
            }
        }

        double act_time = TextWidth / (((double) m_width + TextWidth)/ (double) m_durationMarquee); // duration of last char visible on screen
        
        if(result.find("[MROW]") != std::string::npos) {
            if(m_disallowMode & DISALLOW_SCROLL) {
                continue;
            }
            bool Replaced = false;
            for(int i=0;i < line;i++) {
                double Time_Arrive_Border = (playbackTime + (double) m_durationMarquee) - act_time; // The time of first char reach left border of video
                if(Time_Arrive_Border > rows_dismiss_time[i] && playbackTime > rows_visible_time[i]){
                    rows_dismiss_time[i] = playbackTime + (double) m_durationMarquee;
                    rows_visible_time[i] = playbackTime + act_time;
                    result = replaceAll(result, "[MROW]", std::to_string(i * m_fontSize));
                    Replaced = true;
                    break;
                }
            }
            if(!Replaced){
                result = "";
                Dropped_Rows++;
            }
        }else if(result.find("[TopROW]") != std::string::npos) {
            if(m_disallowMode & DISALLOW_TOP) {
                continue;
            }
            double timeago =  iterator->first - TopTime;
            if(timeago > m_durationStill) {
                TopROW = 0;
                TopTime = iterator->first;
            } else {
                TopROW++;
            }
            result = replaceAll(result, "[TopROW]", std::to_string(TopROW * m_fontSize));
        } else if(result.find("[BottomROW]") != std::string::npos) {
            if(m_disallowMode & DISALLOW_BOTTOM){
                continue;
            }else{
                double timeago =  iterator->first - BottomTime;
                if(timeago > m_durationStill){
                    BottomROW = 0;
                    BottomTime = iterator->first;
                }else{
                    BottomROW++;
                }
                result = replaceAll(result, "[BottomROW]", std::to_string(m_height - BottomROW * m_fontSize));
            }
        } else {
            continue;
        }
        
        if(result.length() < 10) {
            Dropped_Rows++;
            continue;
        }

        output << result << std::endl;
    }

    std::cout << "Comments:" << All_Rows << " Dropped:" << Dropped_Rows << std::endl;
}

void AssBuilder::exportAssToFile(const std::string& filename)
{
    std::remove(filename.c_str());
    std::ofstream output(filename);
    exportAss(output);
}