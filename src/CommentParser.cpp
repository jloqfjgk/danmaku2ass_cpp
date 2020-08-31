#include <Danmaku2ASS/CommentParser.h>
#include <iostream>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

using namespace Danmaku2ASS;

/*
 Get comment type
 */
static CommentType getCommentType(std::string headline)
{
    if (headline.find("\"commentList\":[") != std::string::npos)
    {
        return COMMENT_TYPE_ACFUN;
    }
    else if (headline.find("xml version=\"1.0\" encoding=\"UTF-8\"?><i") != std::string::npos ||
             headline.find("xml version=\"1.0\" encoding=\"utf-8\"?><i") != std::string::npos ||
             headline.find("xml version=\"1.0\" encoding=\"Utf-8\"?>\n") != std::string::npos)
    {
        return COMMENT_TYPE_BILIBILI;
    }
    else if (headline.find("xml version=\"1.0\" encoding=\"UTF-8\"?><p") != std::string::npos ||
             headline.find("!-- BoonSutazioData=") != std::string::npos)
    {
        return COMMENT_TYPE_NICONICO;
    }
    else
    {
        return COMMENT_TYPE_UNKNOWN;
    }
}

CommentParser::CommentParser(std::istream &source) : m_inStream(source)
{
}

AssBuilder::Ptr CommentParser::convert()
{
    std::string headline;
    getline(m_inStream, headline);
    CommentType type = getCommentType(headline);
    m_inStream.seekg(0);

    switch (type)
    {
    case COMMENT_TYPE_BILIBILI:
    {
        std::cout << "Bilibili format detected ! Converting..." << std::endl;
        AssBuilder::Ptr ass = _convertBilibili();
        if (ass != nullptr)
        {
            std::cout << "Convert succeed" << std::endl;
        }
        else
        {
            std::cerr << "Convert failed" << std::endl;
        }
        return ass;
    }

    case COMMENT_TYPE_ACFUN:
    case COMMENT_TYPE_NICONICO:
        std::cerr << "Sorry , The format is not supported" << std::endl;
        return nullptr;

    default:
        std::cerr << "ERROR: Unable to get comment type" << std::endl;
        return nullptr;
    }
}

AssBuilder::Ptr CommentParser::_convertBilibili()
{
    auto ass = std::make_shared<AssBuilder>(m_width, m_height, m_font, m_fontSize, m_alpha, m_durationMarquee, m_durationStill);

    rapidxml::file<> xmlFile(m_inStream);
    if (xmlFile.size() < 1)
    {
        return nullptr;
    }

    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *node;
    try
    {
        doc.parse<0>(xmlFile.data());
        node = doc.first_node("i"); // Get comment main node
    }
    catch (const rapidxml::parse_error &e)
    {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return nullptr;
    }
    if (!node)
    {
        return nullptr;
    }
    if (!node->first_node("d"))
    {
        return nullptr;
    }
    for (auto child = node->first_node("d"); child; child = child->next_sibling()) // Each comment
    {
        if (!child)
        {
            continue;
        }

        /* Arg1 : Appear time
         The time of comment appear.
         */
        std::string p = child->first_attribute("p")->value();
        size_t p_start = 0;
        size_t p_end = p.find(',');
        if (p_end == std::string::npos)
        {
            continue;
        }
        double appear_time = stof(p.substr(p_start, p_end - p_start));

        /* Arg2 : Comment mode
         123 - Scroll comment
         4 - Bottom comment
         5 - Top comment
         6 - Reverse comment
         7 - Positioned comment
         8 - Javascript comment ( not convert )
         */
        p_start = p_end + 1;
        p_end = p.find(',', p_start);
        if (p_end == std::string::npos)
        {
            continue;
        }
        int comment_mode = stoi(p.substr(p_start, p_end));

        /* Arg3 : Font size ( not needed )*/
        p_start = p_end + 1;
        p_end = p.find(',', p_start);
        if (p_end == std::string::npos)
        {
            continue;
        }

        /* Arg4 : Font color */
        p_start = p_end + 1;
        p_end = p.find(',', p_start);
        if (p_end == std::string::npos)
        {
            continue;
        }
        int font_color = stoi(p.substr(p_start, p_end));

        /* Arg5 : Unix timestamp ( not needed ) */
        /* Arg6 : comment pool ( not needed ) */
        /* Arg7 : sender uid ( not needed ) */
        /* Arg8 : database rowID ( not needed ) */

        ass->appendComment(appear_time, comment_mode, font_color, child->value());
    }
    return ass;
}