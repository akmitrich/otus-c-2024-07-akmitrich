#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int process_file(FILE *input);

int main(int argc, const char *argv[])
{
    const char *program = argv[0];
    printf("%s -- find JPEG and ZIP in a FILE", program);
    if (argc < 2)
    {
        printf("\nUsage: %s FILE\n", program);
        printf("FILE -- input binary file\n");
        printf("All other args are ignored\n");
        return 1;
    }
    const char *file_path = argv[1];
    printf(" %s.\n", file_path);

    FILE *input = fopen(file_path, "rb");
    if (input == NULL)
    {
        printf("ERROR: could not open your file %s.\n", file_path);
        return 2;
    }

    int r = process_file(input);

    fclose(input);

    return r;
}

int roll_buffer(unsigned char *buf, size_t buf_size, size_t need, size_t *marker, size_t *len, void *src)
{
    size_t remain = *len - *marker;
    if (remain < need)
    {
        for (size_t i = 0; i < remain; ++i)
        {
            buf[i] = buf[*marker + i];
        }
        *len = fread(buf + remain, 1, buf_size - remain, src);
        *len += remain;
        *marker = 0;
        if (*len - *marker < need)
        {
            return -1;
        }
    }
    return 0;
}

#define BUF_SIZE_MAX 1024

int find_jpeg(FILE *input)
{
    int end_of_jpeg = -1;
    unsigned char buf[BUF_SIZE_MAX];
    size_t len = fread(buf, 1, 2, input);
    if (len < 2)
    {
        printf("ERROR: could not read start of JPEG.\n");
        return -1;
    }

    // start of JPEG signature 'FF D8'
    if (buf[0] != 0xff || buf[1] != 0xd8)
    {
        return -1;
    }

    size_t marker = 0;
    buf[0] = 0;
    end_of_jpeg = 2;

    // end of JPEG signature 'FF D9'
    while (buf[marker] != 0xff || buf[marker + 1] != 0xd9)
    {
        ++marker;
        ++end_of_jpeg;
        if (len - marker < 2)
        {
            buf[0] = buf[len - 1];
            len = fread(buf + 1, 1, BUF_SIZE_MAX - 1, input);
            if (len == 0)
            {
                printf("ERROR: unexpected end of JPEG.\n");
                return -1;
            }
            len += 1;
            marker = 0;
        }
    }

    return end_of_jpeg;
}

int list_files_zip(FILE *input)
{
    unsigned char buf[BUF_SIZE_MAX];
    size_t len = 0;
    size_t marker = 0;
    if (roll_buffer(buf, BUF_SIZE_MAX, 4, &marker, &len, input) < 0)
    {
        printf("No ZIP found.\n");
        return -1;
    }
    // ZIP always starts with 'PK' signature bytes
    if (buf[marker] != 0x50 || buf[marker + 1] != 0x4b)
    {
        printf("No ZIP found.\n");
        return -1;
    }
    size_t found = 0;
    while (len > 0)
    {
        if (roll_buffer(buf, BUF_SIZE_MAX, 4, &marker, &len, input) < 0 && len > 3)
        {
            printf("ERROR: unexpected EOF while looking for central directory file header signature. %lu\n", len);
            return -1;
        }
        // Central directory file header signature '50 4b 01 02'
        if (buf[marker] == 0x50 && buf[marker + 1] == 0x4b && buf[marker + 2] == 1 && buf[marker + 3] == 2)
        {
            // Central directory file header 46 bytes long
            if (roll_buffer(buf, BUF_SIZE_MAX, 46, &marker, &len, input) < 0)
            {
                printf("ERROR: unexpected end of ZIP central directory.\n");
                return -1;
            }
            size_t filename_len = buf[marker + 28] + (buf[marker + 29] << 8);
            marker += 46;
            if (roll_buffer(buf, BUF_SIZE_MAX, filename_len, &marker, &len, input) < 0)
            {
                printf("ERROR: could not read filename when it must be there.\n");
                return -1;
            }
            char *filename = malloc(filename_len + 1);
            strncpy(filename, (const char *)&buf[marker], filename_len);
            filename[filename_len] = '\0';
            printf("%lu. '%s'\n", ++found, filename);
            free(filename);
        }
        marker += 1;
    }
    if (found == 0)
    {
        printf("No files found in the ZIP.\n");
    }
    return 0;
}

int process_file(FILE *input)
{
    int pos = find_jpeg(input);
    if (pos > 0)
    {
        printf("File is JPEG, ends at %d\n", pos);
        fseek(input, pos, SEEK_SET);
        list_files_zip(input);
    }
    else
    {
        printf("The file is not a JPEG.\n");
        return -1;
    }
    return 0;
}