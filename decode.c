#include <stdio.h>
#include <string.h> 
#include "decode.h"
#include <stdlib.h>
Status open_src_file(DecodeInfo *decInfo)
{
    // opening the src file
    decInfo->src_fptr = fopen(decInfo->src_image_file_name, "rb");
    if(decInfo->src_fptr == NULL)
    {
        return e_failure;       
    }
    return e_success;
}
Status do_decoding(DecodeInfo* decInfo, int argc, char* argv[])
{
    printf("INFO: ## Decoding Procedure Started ##\n");

    // reading and validating the arg
    if(read_and_validate_decode_argv(decInfo, argv) == e_success)
        printf("INFO: Opening required files\n");
    // Opening the files
    if(open_src_file(decInfo) == e_success)
    {
        printf("INFO: Opened %s\n", decInfo->src_image_file_name);
    }
    else
    {
        printf("Failed opening file\n");
        return e_failure;
    }

    // reading the magic string from user
    printf("Enter the magic string Signature: ");
    scanf(" %s", decInfo->magic_string_input);

    // decoding the magic string from the src file
    printf("INFO: Decoding Magic String Signature\n");
    if(decode_magic_string(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("ERROR: Wrong input file\n");
        return e_failure;
    }
    if(strcmp(decInfo->magic_string_input, decInfo->magic_string_from_src) !=0)
    {
        printf("INFO: Authentication Failed!!!\n");
        return e_failure;
    }
    printf("INFO: Decoding Output File Extenstion\n");
    if(decode_extn(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    if(match_extn_and_open_file(decInfo, argv) == e_success)
    {
        printf("INFO: Done. Opened all required files\n");
    }
    if(decoding_secrete_file_data(decInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    fclose(decInfo->dest_fptr);
    fclose(decInfo->src_fptr);
    return e_success;
    

}
Status read_and_validate_decode_argv(DecodeInfo * decInfo, char* argv[])
{
    // checking src file is having the .bmp 
    if(strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->src_image_file_name = argv[2];
        return e_success;
    }

    /*// checking the dest file is present or not
    if(argv[3] == NULL)
    {
        printf("INFO: Output File not mentioned. Creating decoded.txt as default\n");
        decInfo->dest_image_file_name = "output";
    }*/
}
Status decode_magic_string(DecodeInfo* decInfo)
{
    /*skipping the 54 bytes */
    fseek(decInfo->src_fptr, 54, SEEK_SET);
    decInfo->lenth_of_magic_string = decode_length(decInfo);
    if(decInfo->lenth_of_magic_string >6 || decInfo->lenth_of_magic_string<0)
    {
        return e_failure;
    }
    decode_string(decInfo->lenth_of_magic_string, decInfo->magic_string_from_src, decInfo);
    return e_success;
}
Status decode_extn(DecodeInfo *decInfo)
{
    decInfo->length_of_secret_file_extn = decode_length(decInfo);
    decode_string(decInfo->length_of_secret_file_extn, decInfo->secret_file_extn_string, decInfo);   

}
Status match_extn_and_open_file(DecodeInfo*decInfo, char*argv[])
{
    if(argv[3] == NULL)
    {
        strcpy(decInfo->dest_file_name, "decoded");
        printf("INFO: Output File not mentioned. Creating %s as default\n", decInfo->dest_file_name);
        strcat(decInfo->dest_file_name, decInfo->secret_file_extn_string);
        decInfo->dest_fptr = fopen(decInfo->dest_file_name, "wb");
        printf("INFO: Opened %s\n", decInfo->dest_file_name);
        return e_success;
    }
    else
    {
        strcpy(decInfo->dest_file_name, argv[3]);
        char tempFileName[SIZE_OF_DEST_FILE];
        strcpy(tempFileName, decInfo->dest_file_name);
        char *temp = strchr(tempFileName, '.');
        if(temp != NULL)
        {
            *temp = '\0';
        }
        strcpy(decInfo->dest_file_name, tempFileName);
        strcat(decInfo->dest_file_name, decInfo->secret_file_extn_string);
        decInfo->dest_fptr = fopen(decInfo->dest_file_name, "wb");
        printf("File extension with name = %s\n", decInfo->dest_file_name);
        return e_success;
    }
}
Status decoding_secrete_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding File Size\n");
    decInfo->len_of_serete_file=decode_length(decInfo);
    printf("INFO: Done\n");
    printf("INFO: Decoding File Data\n");
    unsigned char *secret_file_data = (char *)malloc(decInfo->len_of_serete_file + 1);
    if (secret_file_data == NULL) 
    {
        printf("ERROR: Memory allocation failed!\n");
        return e_failure;
    }

    decode_string(decInfo->len_of_serete_file, secret_file_data, decInfo);
    printf("INFO: Done\n");
    secret_file_data[decInfo->len_of_serete_file] = '\0';
    fprintf(decInfo->dest_fptr, "%s", secret_file_data);
    // char ch;
    // int i=0;
    // while(decInfo->secret_file_data[i])
    // {
    //     fputc(ch, decInfo->dest_fptr);
    //     i++;
    // }
    return e_success;
}

int decode_length(DecodeInfo *decInfo)
{
    char buffer[32];
    int len = 0;
    fread(buffer, sizeof(char), 32, decInfo->src_fptr);
    for(int i=0; i<32; i++)
    {
        len |= ((buffer[i] &1) << i);
    }
    //printf("%d\n", len);
    /*for(int i = 0; i <4; i++)
    {
        // Correct order: element size then number of elements
        fread(buffer, sizeof(char), 8, decInfo->src_fptr);
        for(int j = 0; j < 8; j++)
        {
            
            length[i] |= ((buffer[j] & 1) << j);
            printf("%hhx  ", length[i]);
        }
    }*/
    //sscanf(length, "%d", &len);
    // for(int i=0; i<4; i++)
    // printf("\nLen = %hhx", length[i]);
    // putchar('\n');
    return len;
}
Status decode_string(int len, char* ptr, DecodeInfo *decInfo)
{
    unsigned char buffer[8];
    for(int i=0; i< len; i++)
    {
        ptr[i] = 0;
        if(fread(buffer, sizeof(char), 8, decInfo->src_fptr) != 8)
        {
            printf("string decoding fail\n");
        }
        for(int j=0; j<8; j++)
        {
            ptr[i] |= ((buffer[j] & 1)<<j);
        }

    }
    return e_success;
}
