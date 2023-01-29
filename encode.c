/*
NAME    : NAYANA B R
DATE    : 03-12-2022
PROJECT : LSB Image Steganograpy
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

//function defination for read and validation
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(!strcmp(strstr(argv[2], "."),".bmp"))
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    if( !strcmp(strstr(argv[3],"."),".txt"))
    {
        encInfo -> secret_fname = argv[3];
        strcpy(encInfo -> extn_secret_file, ".txt");
    }
    else if( !strcmp(strstr(argv[3],"."),".sh"))
    {
        encInfo -> secret_fname = argv[3];
        strcpy(encInfo -> extn_secret_file, ".sh");
    }
    else if( !strcmp(strstr(argv[3],"."),".c"))
    {
        encInfo -> secret_fname = argv[3];
        strcpy(encInfo -> extn_secret_file, ".c");
    }

    if( argv[4] != NULL)
    {
        encInfo -> stego_image_fname = argv[4];
    }
    else
    {
        printf("INFO: Output file not mentioned. Creating steged_1mg.bmb as default\n");
        encInfo -> stego_image_fname = "Steged_image_1mg.bmp";
    }
    return e_success;

}

//enoding starts here
//function defination for encoding screet file
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    if(open_files(encInfo) == e_success)
    {
        printf("INFO: done\n");
        printf("INFO: ## Encoding procedure Started ##\n");
    }
    else
    {
        printf("INFO: File opening failed\n");
        return e_failure;
    }

    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    if(get_file_size(encInfo->fptr_secret)>0 )
    {
        printf("INFO: Done. Not Empty\n");
    }
    else
    {
        printf("INFO: Done. file is empty \n");
        return e_failure;
    }

    if(check_capacity(encInfo) == e_success)
    {
        printf("INFO: done. Found OK\n");
    }
    else
    {
        printf("INFO: check capacity failed\n");
        return e_failure;
    }

    if( copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: done\n");
    }
    else
    {
        printf("INFO: Header copying failed\n");
        return e_failure;
    }

    if( encode_magic_string( MAGIC_STRING,encInfo) == e_success)
    {
        printf("INFO: done\n");
    }
    else
    {
        printf("INFO: magic string encoding failed\n");
        return e_failure;
    }

    if( encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("INFO: secret file extn size encoding failed\n");
        return e_failure;
    }

    if( encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("INFO: done\n");
    }
    else
    {
        printf("INFO: screct file extn encoding failed\n");
        return e_failure;
    }

    if( encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
    {
        printf("INFO: done\n");
    }
    else
    {
        printf("INFO: secret file size encoding failed\n");
        return e_failure;
    }

    if( encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO: done\n");
    }
    else
    {
        printf("INFO: secret file data encoding failed\n");
        return e_failure;
    }

    if( copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success )
    {
        printf("INFO: done\n");
    }
    else
    {
        printf("INFO: copying remaing data failed\n");
        return e_failure;
    }
    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->src_image_fname);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo ->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->stego_image_fname);

    // No failure return e_success
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0L,SEEK_END);
    return ftell(fptr);
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

//checking soruce image capacity to encode screct message
Status check_capacity(EncodeInfo *encInfo )
{
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);

    fseek(encInfo -> fptr_secret,0,SEEK_END);
    encInfo -> size_secret_file = ftell(encInfo -> fptr_secret);

    if(encInfo -> image_capacity > (54 + (strlen(MAGIC_STRING)*8) + 32 + 32 + 32 + ((encInfo->size_secret_file)*8)))
    {
        return e_success;
    }
    else
        return e_failure;
}

//getting image size for bmp
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

//coping  bmp header from source to destination(stego image)
Status copy_bmp_header (FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    rewind(fptr_src_image);
    char str[54];
    fread(str,sizeof(char),54, fptr_src_image);
    fwrite(str,sizeof(char),54, fptr_dest_image);
    return e_success;
}

//encoding magic string to stego image
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

//function for encode charecter to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int i=0;
    for(int j=7;j>=0;j--,i++)
    {
        image_buffer[i] = image_buffer[i] & ~1;
        image_buffer[i] = image_buffer[i] | ((data & (1<<j))>>j);
    }
}

//function for encode data to image
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        fread( image_buffer,8,sizeof(char),fptr_src_image);
        encode_byte_to_lsb(data[i],image_buffer);
        fwrite(image_buffer,8,sizeof(char),fptr_stego_image);
    }
    return e_success;
}

//function for encode integer to lsb
Status encode_int_to_lsb(char data, char *image_buffer)
{
    int i=0;
    for(int j=31;j>=0;j--,i++)
    {
        image_buffer[i] = image_buffer[i] & ~1;
        image_buffer[i] = image_buffer[i] | ((data & (1<<j))>>j);
    }
}

//function for encoding secret file extention size
Status encode_secret_file_extn_size(int size,FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[32];
    fread(arr,sizeof(char),32,fptr_src_image);
    encode_int_to_lsb(size,arr);
    fwrite(arr,sizeof(char),32,fptr_stego_image);
    return e_success;
}

//function defination for encoding secret file extention
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extenstion\n",encInfo->secret_fname);
    encode_data_to_image((char*)file_extn, strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

//function defination for encoding secret file extention size
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Size\n",encInfo->secret_fname);

    char str[32];
    fread(str,sizeof(char),32,encInfo -> fptr_src_image);
    encode_int_to_lsb((int)file_size,str);
    fwrite(str,sizeof(char),32,encInfo->fptr_stego_image);
    return e_success;
}

//function defination for encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Data\n",encInfo->secret_fname);
    fseek(encInfo->fptr_secret,0L,SEEK_SET);
    char buffer[encInfo->size_secret_file];
    fread(buffer,encInfo->size_secret_file,sizeof(char),encInfo->fptr_secret);
    encode_data_to_image(buffer,sizeof(buffer),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

//coping remaining source image data to Stego image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    char ch;
    while(fread(&ch,sizeof(char),sizeof(char),fptr_src) > 0)
    {
        fwrite(&ch,sizeof(char),sizeof(char),fptr_dest);
    }
    return e_success;
}