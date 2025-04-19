// decode header file
#ifndef DECODE_H
#define DECODE_H
#include "types.h" // Contains user defined types
#define SIZE_OF_MAGIC_STRING 6
#define SIZE_OF_DEST_FILE 30
#define SIZE_OF_DEST_DATA 100
typedef struct DecodeInfo{
    // information of src file
    char *src_image_file_name; 
    FILE * src_fptr;

    // magic string
    int lenth_of_magic_string;
    char magic_string_input[SIZE_OF_MAGIC_STRING];
    char magic_string_from_src[SIZE_OF_MAGIC_STRING];

    // extension information
    int length_of_secret_file_extn;
    char secret_file_extn_string[SIZE_OF_MAGIC_STRING];
    char secret_file_extn_from_user[SIZE_OF_MAGIC_STRING];

    // secrete data;
    char dest_file_name[SIZE_OF_DEST_FILE];
    FILE *dest_fptr;
    int len_of_serete_file;
    
}DecodeInfo;
/*Decode function prototype*/
Status do_decoding(DecodeInfo *decInfo, int argc, char* argv[]);
Status read_and_validate_decode_argv(DecodeInfo *decInfo, char* argv[]);
Status open_src_file(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_extn(DecodeInfo *decInfo);
Status match_extn_and_open_file(DecodeInfo*decInfo, char*argv[]);
Status decoding_secrete_file_data(DecodeInfo *decInfo);
Status decode_string(int len, char* ptr, DecodeInfo *decInfo);
int decode_length(DecodeInfo *decInfo);
#endif