#ifndef DECODE_H
#define DECODE_H

#include "types.h"   //User defined types.h data type
#include "common.h"  //User defined common.h data type

/* Typedef done to Structure to store information required for decoding stego image to new file Info about output and intermediate data is also stored */
typedef struct  _DecodeInfo
{
    /* Encoded stego image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    uint image_data_size;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Decoded output file info */
    char *decoded_fname;
    FILE *fptr_decoded_file;
    char extn_decoded_file[MAX_FILE_SUFFIX];
    char decode_data[MAX_SECRET_BUF_SIZE];
}	DecodeInfo;


/* Decoding function prototypes */

/* Read and validation Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointer for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Performs Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get the magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(uint extn_size, DecodeInfo *decInfo);

/* Decode secret file extention size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decoding secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode byte from lsb */
Status decode_byte_from_lsb(char *decode_data, char *image_data);

/* Decode size from LSB of the image data */
Status decode_size_from_lsb(char *buffer, DecodeInfo *decInfo);

#endif
