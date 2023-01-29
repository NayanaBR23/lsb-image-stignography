/*
NAME    : NAYANA B R
DATE    : 03-12-2022
PROJECT : LSB Image Steganograpy
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    /*
       if(argc<=2 )
       {
       printf("INFO :Too less arguments\n");
       printf("USAGE: ./a.out -e source.bmp secrect.txt <destination.bmp>\n");
       printf("USAGE: ./a.out -d Stego.bmp <decoded_data.txt>\n");
       return e_failure;
       }
     */

    // checking for valid number of arguments
    if( (argc<=2) ||(!strcmp(argv[1],"-e") && argc == 3) )
    {
        printf("Too less arguments\n");
        printf("USAGE: ./a.out -e source.bmp secrect.txt <destination.bmp>\n");
        printf("USAGE: ./a.out -d Stego.bmp <decoded_data.txt>\n");
        return e_failure;
    }

    //function defination for checking opertion type
    int option = check_operation_type(argv);
    if(option == e_encode)
    {
        //declaring encode struct
        EncodeInfo encInfo;

        //calling function to read and validate arguments
        if( read_and_validate_encode_args(argv, &encInfo) == e_success)
        {

            //calling function for encode screct data
            if(do_encoding(&encInfo) == e_success)
            {
                printf("INFO: ## Encode done successfully ##\n");
            }
            else
            {
                printf("INFO: ## Encoding failed ##\n");
            }
        }
        else
        {
            printf("INFO: read and validate encode arguments failure\n");
        }
    }
    else if(option == e_decode)
    {
        //declaring decode struct
        DecodeInfo decInfo;

        printf("INFO: ## Decoding procedure started ##\n");
        //calling function to read and validate arguments
        if( read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            //calling function for decode screct data
            if(do_decoding(argv,&decInfo) == e_success)
            {
                printf("INFO: ## Decode done successfully ##\n");
            }
            else
            {
                printf("INFO: ## Decoding failed ##\n");
            }
        }
        else
        {
            printf("INFO: read and validate decode arguments failure\n");
        }
    }
    else
    {
        printf("INFO : Unsupported operation type\n");
        printf("USAGE: ./a.out source.bmp secrect.txt <destination.bmp>\n");
        printf("USAGE: ./a.out -d Stego.bmp <decoded_data.txt>\n");
    }
}

//function for checking operation type
OperationType check_operation_type(char *argv[])
{
    if(!strcmp(argv[1] , "-e")  )
    {
        return e_encode;
    }
    else if(!strcmp(argv[1] ,"-d") )
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}