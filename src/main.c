#include <Danmaku2ASS/danmaku2ass.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void help()
{
    const char *str =
        "Usage: danmaku2ass [-h] [-o OUTPUT] [-s WIDTHxHEIGHT] [-fn FONT]\n"
        "                   [-fs SIZE] [-a ALPHA] [-dm SECONDS] [-ds SECONDS]\n"
        "                   FILE\n"
        "\n"
        "positional arguments:\n"
        "  FILE                  Comment file to be processed\n"
        "\n"
        "optional arguments:\n"
        "  -h, --help            show this help message and exit\n"
        "  -o OUTPUT, --output OUTPUT\n"
        "                        Output file\n"
        "  -s WIDTHxHEIGHT, --size WIDTHxHEIGHT\n"
        "                        Stage size in pixels [default: 1280x720]\n"
        "  -fn FONT, --font FONT\n"
        "                        Specify font face [default: sans-serif]\n"
        "  -fs SIZE, --fontsize SIZE\n"
        "                        Default font size [default: 25]\n"
        "  -a ALPHA, --alpha ALPHA\n"
        "                        Text opacity [default: 1]\n"
        "  -dm SECONDS, --duration-marquee SECONDS\n"
        "                        Duration of scrolling comment display [default: 5]\n"
        "  -ds SECONDS, --duration-still SECONDS\n"
        "                        Duration of still comment display [default: 5]\n";
    puts(str);
}

int main(int argc, char **argv)
{
    char *input = NULL;
    char *output = NULL;
    char *font = "sans-serif";
    int width = 1280;
    int height = 720;
    int fontsize = 25;
    int dm = 5;
    int ds = 5;
    double alpha = 1;

    int ret;

    // Read arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            help();
            return EXIT_SUCCESS;
        }
        
        else if (argv[i][0] == '-' && i == argc - 1)
        {
            fprintf(stderr, "ERROR: No value for the parameter \"%s\" is given!\n\n", argv[i]);
            help();
            return EXIT_FAILURE;
        }

        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0)
        {
            output = argv[i + 1];
            i++;
        }

        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0)
        {
            char *w = argv[i + 1];
            char *x = strchr(w, 'x');
            if (x == NULL)
            {
                fprintf(stderr, "ERROR: Wrong video size format: %s (should be WIDTHxHEIGHT).\n\n", w);
                help();
                return EXIT_FAILURE;
            }
            *x = '\0';
            char *h = x + 1;
            width = atoi(w);
            height = atoi(h);
            i++;
        }

        else if (strcmp(argv[i], "-fn") == 0 || strcmp(argv[i], "--font") == 0)
        {
            font = argv[i + 1];
            i++;
        }

        else if (strcmp(argv[i], "-fs") == 0 || strcmp(argv[i], "--fontsize") == 0)
        {
            fontsize = atoi(argv[i + 1]);
            i++;
        }

        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--alpha") == 0)
        {
            alpha = atof(argv[i + 1]);
            i++;
        }

        else if (strcmp(argv[i], "-dm") == 0 || strcmp(argv[i], "--duration-marquee") == 0)
        {
            dm = atoi(argv[i + 1]);
            i++;
        }

        else if (strcmp(argv[i], "-ds") == 0 || strcmp(argv[i], "--duration-still") == 0)
        {
            ds = atoi(argv[i + 1]);
            i++;
        }

        else
        {
            input = argv[i];
        }
    }

    if (input == NULL)
    {
        fprintf(stderr, "ERROR: No input file is given!\n\n");
        help();
        return EXIT_FAILURE;
    }

    if (output == NULL)
    {
        output = (char*) malloc(strlen(input) + 4);
        strcpy(output, input);
        strcat(output, ".ass");
    }

    printf(
        "Configuration:\n"
        "  Input:            %s\n"
        "  Output:           %s\n"
        "  Video size:       %dx%d\n"
        "  Font:             %s %d\n"
        "  Alpha:            %lf\n"
        "  Duration marquee: %d\n"
        "  Duration still:   %d\n\n",
        input,
        output,
        width,
        height,
        font,
        fontsize,
        alpha,
        dm,
        ds);

    ret = danmaku2ass(
        input,
        output,
        width,
        height,
        font,
        fontsize,
        alpha,
        dm,
        ds
    );

    return !ret;
}