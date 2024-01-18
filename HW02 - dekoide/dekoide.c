#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#include "non-ascii.h"

const char *encoding_str[] = {"CP-1251", "KOI-8r", "ISO-8859-5"};

static int run(FILE *input, enum encodings_e encoding, FILE *output);

int main(int argc, const char *argv[])
{
    FILE *input;
    enum encodings_e encoding = CP1251;
    FILE *output;

    const char *program = argv[0];
    printf("%s -- converts a FILE from one of the three encoding into UTF-8\n", program);
    if (argc < 2)
    {
        printf("Usage: %s FILE [ENC] [OUTPUT]\n", program);
        printf("FILE -- input file encoded in\n");
        printf("ENC -- one of three encodings: CP1251 | KOI8-R | ISO-8859-5. Default is CP1251\n");
        printf("OUTPUT -- path to save decoded file, default is stdout\n");
        return EXIT_FAILURE;
    }

    const char *input_name = argv[1];
    input = fopen(input_name, "rb");
    if (input == NULL)
    {
        printf("ERROR: cannot open input file.\n");
        return EXIT_FAILURE;
    }

    if (argc > 2)
    {
        if (strncasecmp(argv[2], "cp", 2) == 0)
            encoding = CP1251;
        else if (strncasecmp(argv[2], "koi", 3) == 0)
            encoding = KOI8R;
        else if (strncasecmp(argv[2], "iso", 3) == 0)
            encoding = ISO_8859_5;
        else
            printf("WARNING: cannot find your encoding, try to decode from CP-1251.\n");
    }
    else
        encoding = CP1251;

    const char *output_name;
    int r;
    if (argc == 4)
    {
        output_name = argv[3];
        output = fopen(output_name, "wb");
        if (output == NULL)
        {
            printf("ERROR: cannot create output file.\n");
            r = EXIT_FAILURE;
            goto close_input;
        }
    }
    else
    {
        output = NULL;
        output_name = "'stdout'";
    }

    printf("SINOPSIS: Take file %s encoded with %s and save it to the %s\n", input_name, encoding_str[encoding], output_name);
    r = run(input, encoding, output);

    if (output)
        fclose(output);
close_input:
    fclose(input);

    return r;
}

#define MAX_BUF_SIZE 4096

int run(FILE *input, enum encodings_e encoding, FILE *output)
{
    unsigned char buf[MAX_BUF_SIZE];
    size_t n;
    if (output == NULL)
        output = stdout;

    printf("\n");
    do
    {
        n = fread(buf, 1, MAX_BUF_SIZE, input);
        for (size_t i = 0; i < n; i++)
        {
            const char *decoded = decode_byte(buf[i], encoding);
            if (decoded)
            {
                if (!fwrite(decoded, 1, strlen(decoded), output))
                    return EXIT_FAILURE;
            }
            else if (!fwrite(&buf[i], 1, 1, output))
                return EXIT_FAILURE;
        }
    } while (n > 0);

    return EXIT_SUCCESS;
}