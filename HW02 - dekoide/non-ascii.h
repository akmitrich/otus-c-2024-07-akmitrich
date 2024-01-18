#ifndef H_NON_ASCII
#define H_NON_ASCII

enum encodings_e
{
    CP1251,
    KOI8R,
    ISO_8859_5
};

const char *decode_byte(unsigned char c, enum encodings_e encoding);

#endif