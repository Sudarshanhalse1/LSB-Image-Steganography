#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char* argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    // checking the type of operation
    if(!(argc < 2 || argc > 5))
    {
        switch (check_operation_type(argv))
        {
            case e_encode:
            if(argc>6 || argc < 4)
            {
                printf("sb_steg: Encoding: lsb_steg -e <.bmp file> <.txt file> [output file]\n");
                return e_failure;
            }
                if(do_encoding(&encInfo, argc, argv) == e_success)
                {
                    printf("INFO: ## Encoding Done Successfully ##\n");
                    return e_success;
                }
                break;
            case e_decode:
                if(argc>4 || argc<3)
                {
                    printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
                    return e_failure;
                }
                if(do_decoding(&decInfo, argc, argv) == e_success)
                {
                    printf("INFO: ## Decoding Done Successfully ##\n");
                    return e_success;
                }
                else
                {
                    printf("INFO: ## Decoding Failed ##\n");
                    return e_failure;
                }
                break;
            default:
                printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
                printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
                return e_failure;
        }
    }
    else
    {
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp_file> <.text_file> [output file]\n");
        printf("./lsb_steg: Decoding: ./lsb_steg -d <.bmp_file> [output file]\n");
        return e_failure;
    }
    return 0;
}

