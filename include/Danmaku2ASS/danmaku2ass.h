//
//  danmaku2ass.hpp
//  bilibili
//
//  Created by TYPCN on 2015/6/7.
//  Copyright (c) 2016 TYPCN. All rights reserved.
//

#ifndef bilibili_danmaku2ass_h
#define bilibili_danmaku2ass_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 Convert comments to .ass subtitle
  infile: comment file path
  outfile: output file path
  width: video width
  height: video height
  font: font name
  alpha: comment alpha
  duration_marquee: Duration of scrolling comment
  duration_still: Duration of still comment

  Return: 1 if successful; otherwise 0
 */
int danmaku2ass(const char *infile,
                const char *outfile,
                int width,
                int height,
                const char *font,
                int fontsize,
                double alpha,
                int duration_marquee,
                int duration_still);

#ifdef __cplusplus
}
#endif
#endif
