#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include "non-ascii.h"

enum encodings_e
{
    CP1251,
    KOI8R,
    ISO_8859_5
};

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
        return 1;
    }

    input = fopen(argv[1], "rb");
    if (input == NULL)
    {
        printf("ERROR: cannot open input file.\n");
        return 2;
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

    if (argc == 4)
    {
        output = fopen(argv[3], "wb");
        if (output == NULL)
        {
            printf("ERROR: cannot create output file.\n");
            goto close_input;
        }
    }
    else
        output = NULL;

    printf("SINOPSIS: Take file %p encoded with %d and save it to the %p\n", (void *)input, encoding, (void *)output);
    run(input, encoding, output);

    if (output)
        fclose(output);
close_input:
    fclose(input);

    return 0;
}

#define MAX_BUF_SIZE 1024

const char *decode_byte(unsigned char c, enum encodings_e encoding)
{
    unsigned char a = c & 0x7f;
    if (a == c)
        return NULL;
    switch (encoding)
    {
    case CP1251:
        return CP1251_TABLE[a];
    case KOI8R:
        return KOI8R_TABLE[a];
    case ISO_8859_5:
        return ISO_8859_5_TABLE[0];
    default:
        return NULL;
    }
}

int run(FILE *input, enum encodings_e encoding, FILE *output)
{
    unsigned char buf[MAX_BUF_SIZE];
    size_t n;
    if (output == NULL)
        output = stdout;
    for (size_t i = 0; i < 128; ++i)
    {
        printf("%s", KOI8R_TABLE[i]);
    }
    printf("\n");
    do
    {
        n = fread(buf, 1, MAX_BUF_SIZE, input);
        for (size_t i = 0; i < n; i++)
        {
            const char *decoded = decode_byte(buf[i], encoding);
            if (decoded)
                fwrite(decoded, 1, strlen(decoded), output);
            else
                fwrite(&buf[i], 1, 1, output);
        }
    } while (n > 0);

    return 0;
}