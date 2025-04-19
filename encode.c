#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    printf("INFO:\n");
    uint width, height; short int bits;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);
    
    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("\twidth = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("\theight = %u\n", height);
    rewind(fptr_image);
    fseek(fptr_image, 28, SEEK_SET);
    fread(&bits, sizeof(short int), 1, fptr_image);
    printf("\tbits = %d\n", bits/8);

    // Return image capacity
    return (width * height * (bits/8))+54+1;
}

// gettingn file size
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return(ftell(fptr));
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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo, int argc, char* argv[])
{
    printf("INFO: ## Encoding Procedure Started ##\n");
    // read and validate the input
    if(read_and_validate_encode_args(argv, encInfo) == e_success)
    {
        printf("Validation successful!\n");
    }
    else
    {
        printf("Validation is unsuccessful!\n");
        return e_failure;
    }

    // opening the files
    if (open_files(encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return e_failure;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // reading magic string
    char magic_string[10];
    printf("Enter the magic string: ");
    scanf(" %s", magic_string);

    // calculating the size of secret file
    if(check_capacity(encInfo, strlen(magic_string)) == e_failure)
    {
        printf("ERROR: \"SkeletonCode/beautiful.bmp\" doesn't have the capacity to encode \"secret.txt\"\n");
        return e_failure;
    }
    else
    {
        printf("INFO: Done. Found OK\n");
    }

    // coping the header file
    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        printf("INFO: Done\n");
    else
        printf("header file coping failed!\n");

    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string((const char*)magic_string, encInfo) == e_success)
        printf("INFO: Done\n");
    else
        printf("Encoding of magic string Failed!\n");

    printf("INFO: Encoding %s File Extenstion\n", encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo) == e_success)
        printf("INFO: Done\n");
    else
        printf("Enconding secret file extension Failed!\n");

    
    if(encode_secret_file_data(encInfo) == e_success)
        printf("INFO: Done\n");
    else
        printf("Secrete file data encoding Fsiled!\n");

    printf("INFO: Copying Left Over Data\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo) == e_success)
        printf("INFO: Done\n");
    else
        printf("Coping RGB data failed!\n");

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char str[54];
    if(fread(str, sizeof(char),  54, fptr_src_image)!= 54)
    {
        perror("fread header failed");
        return e_failure;
    }

    if(fwrite(str, sizeof(char),  54, fptr_dest_image) != 54)
    {
        perror("fwrite header failed");
        return e_failure;
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len = (int)strlen(magic_string);
    encode_int(&len, encInfo);
    encode_string((const char*)magic_string, encInfo);
    return e_success;
}

Status encode_int(void* len, EncodeInfo *encInfo)
{
    unsigned int mask;
    //char* ptr = (char*)len;
    char str[32]; 
    if(fread(str, sizeof(char), 32, encInfo->fptr_src_image) !=32)
    {
        printf("Reading int is failed\n");
        return e_failure;
    }
    for(int i=0; i<32; i++)
    {
        str[i] &= -2;
        mask = (1<<i)&(*((int*)len));
        mask = mask>>i;
        str[i] = str[i]|mask;
    }
    if(fwrite(str, sizeof(char), 32, encInfo->fptr_stego_image)!=32)
    {
        printf("ERROR:: int fwrite is failed\n");
        return e_failure;
    }
}

Status encode_string(const char* magic_string, EncodeInfo* encInfo)
{
    unsigned len = strlen(magic_string), mask;
    unsigned char str[8]; 
    // enconding string
    for(int i=0; i<len; i++)
    {
        if(fread(str, sizeof(char), 8, encInfo->fptr_src_image) != 8)
        {
            printf("Fread in encode string failed\n");
            return e_failure;
        }
        for(int j=0; j<8; j++)
        {
            str[j] = str[j] & (char)-2;
            mask = (1<<j)&magic_string[i];
            mask = mask>>j;
            str[j] = str[j]|mask;
        }
        if(fwrite(str, sizeof(char), 8, encInfo->fptr_stego_image)!=8)
        {
            printf("fwrite in string encode is failed\n");
            return e_failure;
        }
    }
}
OperationType check_operation_type(char *argv[])
{
    printf("INFO: Checking Operation Type..\n");
    if(!strcmp(argv[1], "-e"))
        return e_encode;
    else if(!strcmp(argv[1], "-d"))
        return e_decode;
    else
        return e_unsupported;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char* temp;
    if((temp = strstr(argv[2], ".bmp"))&&strcmp(temp, ".bmp") == 0)
    {
        if(strchr(argv[3], '.')!=NULL)
        {
            if(argv[4] != NULL)
            {
                if((temp = strstr(argv[4], ".bmp"))&&strcmp(temp, ".bmp") == 0)
                {
                    encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    printf("sb_steg: Encoding: lsb_steg -e <.bmp file> <.txt file> [output file]\n");
                    return e_failure;
                }
            }
            else
            {
                printf("INFO: Output File not mentioned. Creating 24028A.bmp as default\n");
                encInfo->stego_image_fname = "24028A.bmp";
                
            }
            encInfo->secret_fname = argv[3];
        }
        else
        {
            printf("sb_steg: Encoding: lsb_steg -e <.bmp file> <.txt file> [output file]\n");
            return e_failure;
        }
        encInfo->src_image_fname = argv[2];
        return e_success;
    }
    else
    {
        printf("sb_steg: Encoding: lsb_steg -e <.bmp file> <.txt file> [output file]\n");
        return e_failure;
    }
}
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    char* ptr = strchr(encInfo->secret_fname, '.');
    int len = strlen(ptr);
    for(int i=0; i<len; i++)
        encInfo->extn_secret_file[i]=ptr[i];
    encode_int(&len, encInfo);
    encode_string(encInfo->extn_secret_file, encInfo);
    return e_success;
}

// encoding the lenght of secret file
Status encode_secret_file_size(EncodeInfo *encInfo)
{
    int len = (int)encInfo->size_secret_file;
    //printf("\n\n\nlenght from encode: %d\n\n", len);
    encode_int(&len, encInfo);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    
    printf("INFO: Encoding %s File Size", encInfo->secret_fname);
    if(encode_secret_file_size(encInfo)== e_success)
        printf("INFO: Done\n");
    else
        printf("Encoding secrete file size Failed!\n");
    printf("INFO: Encoding secret.txt File Data\n");

    char str[1024];
    size_t bytesRead;
    while ((bytesRead = fread(str, 1, sizeof(str) - 1, encInfo->fptr_secret)) > 0) 
    {
        str[bytesRead] = '\0';  
        encode_string(str, encInfo);
    }

    return e_success;
  
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo* encInfo)
{
    char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        if (fwrite(buffer, 1, bytesRead, fptr_dest) != bytesRead)
        {
            printf("ERROR: Failed to write data to the output file!\n");
            return e_failure;
        }
    }

    return e_success;
}
    
Status check_capacity(EncodeInfo *encInfo, int len_of_magic)
{
    uint img_size;
    printf("INFO: Checking for secret.txt size\n");
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->size_secret_file != 0)
    {
        printf("INFO: Done. Not Empty\n");
    }
    rewind(encInfo->fptr_secret);
    printf("INFO: Checking for SkeletonCode/beautiful.bmp capacity to handle secret.txt\n");
    img_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
    char* ptr = strchr(encInfo->secret_fname, '.');
    int len_of_exten = strlen(ptr);

    if((encInfo->size_secret_file + len_of_magic + len_of_exten + 12)*8 > img_size)
        return e_failure;
    else
        return e_success;
}

