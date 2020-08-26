# Danmaku2ass (C++ Version)

## Features

Danmaku2ASS converts comments from Niconico/Acfun/Bilibili to ASS format so that you can play it with any media player supporting ASS subtitle.

Written in C++, it can convert 10,000 comments in just 0.05 seconds. It doesn't depend on any third-party libraries, so it is easy to embed.

This library is based on the works of [m13253/danmaku2ass](https://github.com/m13253/danmaku2ass) and [typcn/danmaku2ass_native](https://github.com/typcn/danmaku2ass_native). Much respect to the authors.

## Supported Website

Currently only Bilibili is supported. Other websites will be added in the future.

## Usage

### As executable

Todo.

### As library

Download the code and place it into your project, then import it in your CMakeLists.txt:

```cmake
add_subdirectory(danmaku2ass)
include_directories(${PROJECT_SOURCE_DIR}/3rdparty/danmaku2ass/include)
......
target_link_libraries(${TARGET_NAME} danmaku2ass)
```

C-compatible API:

```c
#include <Danmaku2ASS/danmaku2ass.h>

void danmaku2ass(const char *infile,    // Input file
                 const char *outfile,   // Output file
                 int width,             // Video width
                 int height,            // Video height
                 const char *font,      // Font name
                 int fontsize,          // Font size
                 double alpha,          // Transparency [0-1]
                 int duration_marquee,  // Display time of scrolling comments
                 int duration_still);   // Display time of still comments
```

Use in C++:

```cpp
#include <Danmaku2ASS/CommentParser.h>
#include <Danmaku2ASS/AssBuilder.h>
#include <fstream>

void danmaku2ass_example()
{
    // Open source file
    std::ifstream input("danmaku.xml");
    Danmaku2ASS::CommentParser parser(input);

    // Set parameters
    parser.setResolution(1280, 720); // Video size
    parser.setFont("Heiti", 32);     // Font name & size
    parser.setDuration(5, 5);        // Display time of scrolling and still comments
    parser.setAlpha(alpha);          // Transparency [0-1]

    // Convert to ASS
    Danmaku2ASS::AssBuilder::Ptr ass = parser.convert();

    if (ass != nullptr)
    {
        // Block scolling and top comments
        ass->setDisallowMode(Danmaku2ASS::DISALLOW_SCROLL | Danmaku2ASS::DISALLOW_TOP);

        // Block word
        ass->addBlockWord("114514");

        // Save to file
        std::ofstream outfile("output.ass");
        ass->exportAss(outfile);
        // Or: ass->exportAssToFile("output.ass");
    }
}
```

## Render danmaku into videos

Use `ffmpeg`:

```shell
ffmpeg -i foo.flv -vf ass=foo.ass -vcodec libx264 -acodec copy foo-with-danmaku.flv
```

## Licence

Do What The F*ck You Want To
