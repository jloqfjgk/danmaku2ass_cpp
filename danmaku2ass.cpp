#include <stdio.h>
#include <string.h>
#include <time.h>
#include <map>
#include <iostream>
#include <fstream>
#include <iso646.h>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "AssClass.hpp"
#include "danmaku2ass.h"
#include "danmaku2ass.hpp"

using namespace std;
using namespace rapidxml;

/*
 Get comment type
 
 headline: the first line of comment file
 
 Return:
 1 - Acfun
 2 - Bilibili
 3 - Niconico
 */

int GetCommentType(string headline){
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


bool CommentParser::Convert(int type){
    if(!type){
        std::ifstream input(in);
        string headline;
        getline(input,headline);
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
    
    ass.init(out);
    ass.SetDuration(duration_marquee,duration_still);
    ass.WriteHead(width, height, font, fontsize,alpha);

    rapidxml::file<> xmlFile(in);
    if(xmlFile.size() < 1){
        return false;
    }
    rapidxml::xml_document<> doc;
    xml_node<> *node;
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
    for (xml_node<> *child = node->first_node("d"); child; child = child->next_sibling()) // Each comment
    {
        if(!child){
            continue;
        }
        std::string v = child->value();
        bool isBlocked = false;
        for (auto i = blockWords.begin();i != blockWords.end(); i++ ){
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
        string p = child->first_attribute("p")->value();
        size_t p_start = 0;
        size_t p_end = p.find(',');
        if (p_end == string::npos)
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
        if (p_end == string::npos)
        {
            continue;
        }
        int comment_mode = stoi(p.substr(p_start, p_end));

        /* Arg3 : Font size ( not needed )*/
        p_start = p_end + 1;
        p_end = p.find(',', p_start);
        if (p_end == string::npos)
        {
            continue;
        }

        /* Arg4 : Font color */
        p_start = p_end + 1;
        p_end = p.find(',', p_start);
        if (p_end == string::npos)
        {
            continue;
        }
        int font_color = stoi(p.substr(p_start, p_end));
        
        /* Arg5 : Unix timestamp ( not needed ) */
        /* Arg6 : comment pool ( not needed ) */
        /* Arg7 : sender uid ( not needed ) */
        /* Arg8 : database rowID ( not needed ) */
        
        ass.AppendComment(appear_time, comment_mode, font_color, child->value());
    }
    
    
    ass.WriteToDisk(disallowModes);
    
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

void danmaku2ass(const char *infile,const char *outfile,int width,int height,const char *font,int fontsize,double alpha,int duration_marquee,int duration_still){
    std::ifstream input(infile);
    string headline;
    getline(input,headline);
    int type = GetCommentType(headline);
    CommentParser p;
    p.SetFile(infile, outfile);
    p.SetRes(width, height);
    p.SetFont(font, fontsize);
    p.SetDuration(duration_marquee, duration_still);
    p.SetAlpha(alpha);
    if(type == 1){
        //cout << "Avfun format detected ! Converting..." << endl;
        cout << "Sorry , The format is not supported" << endl;
    }else if(type == 2){
        cout << "Bilibili format detected ! Converting..." << endl;
        bool result = p.Convert(type);
        if(result){
            cout << "Convert succeed" << endl;
        }else{
            cout << "Convert failed" << endl;
        }
    }else if(type == 3){
        //cout << "Niconico format detected ! Converting..." << endl;
        cout << "Sorry , The format is not supported" << endl;
    }else{
        cout << "ERROR: Unable to get comment type" << endl;
    }
    input.close();
}

