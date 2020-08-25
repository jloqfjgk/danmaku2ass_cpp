#include <stdio.h>
#include <string.h>
#include <time.h>
#include <map>
#include <iostream>
#include <fstream>
#include <iso646.h>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "danmaku2ass.h"

using namespace Danmaku2ASS;

/*
 Get comment type
 
 headline: the first line of comment file
 
 Return:
 1 - Acfun
 2 - Bilibili
 3 - Niconico
 */

static int GetCommentType(std::string headline){
    if(headline.find("\"commentList\":[") != std::string::npos){
        return 1;
    }else if(headline.find("xml version=\"1.0\" encoding=\"UTF-8\"?><i") != std::string::npos or
             headline.find("xml version=\"1.0\" encoding=\"utf-8\"?><i") != std::string::npos or
             headline.find("xml version=\"1.0\" encoding=\"Utf-8\"?>\n") != std::string::npos){
        return 2;
    }else if(headline.find("xml version=\"1.0\" encoding=\"UTF-8\"?><p") != std::string::npos or
             headline.find("!-- BoonSutazioData=") != std::string::npos){
        return 3;
    }else{
        return 0;
    }
    return 0;
}


bool CommentParser::convert(int type) {
    if(!type){
        std::ifstream input(m_inFile);
        std::string headline;
        getline(input, headline);
        type = GetCommentType(headline);
        input.close();
    }
    if(type == 2){
        return _convertBilibili();
    }else{
        return false;
    }
}

bool CommentParser::_convertBilibili(){
    Ass ass;
    
    ass.init(m_outFile);
    ass.setDuration(m_durationMarquee, m_durationStill);
    ass.writeHead(m_width, m_height, m_font, m_fontSize, m_alpha);

    rapidxml::file<> xmlFile(m_inFile);
    if(xmlFile.size() < 1){
        return false;
    }
    
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *node;
    try {
        doc.parse<0>(xmlFile.data());
        node = doc.first_node("i"); // Get comment main node
    }catch(const rapidxml::parse_error& e){
        std::cerr << "Parse error: " << e.what() << std::endl;
        return false;
    }
    if(!node){
        return false;
    }
    if(!node->first_node("d")){
        return false;
    }
    for (auto child = node->first_node("d"); child; child = child->next_sibling()) // Each comment
    {
        if(!child){
            continue;
        }
        std::string v = child->value();
        bool isBlocked = false;
        for (auto i = m_blockWords.begin();i != m_blockWords.end(); i++ ){
            if(v.find(*i) != std::string::npos){
                isBlocked = true;
            }
        }
        if(isBlocked){
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
        
        ass.appendComment(appear_time, comment_mode, font_color, child->value());
    }
    
    
    ass.writeToDisk(m_disallowMode);
    
    return true;
}

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
void Danmaku2ASS::run(const char *infile,const char *outfile,int width,int height,const char *font,int fontsize,double alpha,int duration_marquee,int duration_still){
    std::ifstream input(infile);
    std::string headline;
    getline(input, headline);
    int type = GetCommentType(headline);
    CommentParser p;
    p.setFile(infile, outfile);
    p.setResolution(width, height);
    p.setFont(font, fontsize);
    p.setDuration(duration_marquee, duration_still);
    p.setAlpha(alpha);
    if(type == 1){
        //cout << "Avfun format detected ! Converting..." << endl;
        std::cerr << "Sorry , The format is not supported" << std::endl;
    }else if(type == 2){
        std::cout << "Bilibili format detected ! Converting..." << std::endl;
        bool result = p.convert(type);
        if(result){
            std::cout << "Convert succeed" << std::endl;
        }else{
            std::cerr << "Convert failed" << std::endl;
        }
    }else if(type == 3){
        //cout << "Niconico format detected ! Converting..." << endl;
        std::cerr << "Sorry , The format is not supported" << std::endl;
    }else{
        std::cerr << "ERROR: Unable to get comment type" << std::endl;
    }
    input.close();
}

