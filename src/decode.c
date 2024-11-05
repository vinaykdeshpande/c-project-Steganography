/*
name:vinay kumar k deshpande 
date:10-08-2022
description:Project_1: Steganography.
*/
#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include "common.h"


/* Defining function to Get File names and validate
 * Input: stego.bmp file
 * Output: Data hidden in Stego.bmp image is decoded in decoded.txt file
 * Return Value: e-success or e_failure, on file errors
*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //checking if the third argument is .bmp file or not
    if (strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
    {
        decInfo -> stego_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    //checking if the fourt argument is passed or not
    if (argv[3] != NULL)
    {
        decInfo -> decoded_fname = argv[3];        //assigning the 4th argument already passed to pointer decoded_fname
    }
    else
    {
        decInfo -> decoded_fname = "decoded.txt";  //creating decoded.txt file if 4th argument not passed and assigned
    }
    return e_success;
}


/* Defining function : Decoding contents of stego.bmp image in decoded.txt file
 * Input: stego.bmp image
 * Output: Decoded message copied from stego.bmp image to decoded.txt file
 * Return: e_success or e_failure, on file error
*/
Status do_decoding(DecodeInfo *decInfo)
{
    printf("##Decoding Procedure Started##\n");

    //To check files opened successfully or not
    if (open_decode_files(decInfo) == e_success)
    {
        printf("INFO: Opening  required files\n");
       
        //making the file pointer to point to 54th byte where magic string is stored
        fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
        //To check whether magic string decoded successfully or not
        if (decode_magic_string(MAGIC_STRING, decInfo) == e_success)
        {
            printf("INFO: Deccoding magic string procedure\n");
                    
            //To check whether decoded file extension size is decoded successfully or not
            if (decode_secret_file_extn_size(decInfo) == e_success)
            {
                printf("INFO: Decoding output file extension\n");
            
                //To check whether decoded file extension is decoded successfully or not
                if(decode_secret_file_extn(decInfo -> image_data_size, decInfo) == e_success)
                {
                    printf("Decoded secret file extension is success\n");
                
                    //To check whether decoded file size is decoded successfully or not
                    if (decode_secret_file_size(decInfo) == e_success)
                    {
                        printf("Decoding File size\n");

                        //To check whether decoded data to file  successful or not
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("Decoding File data\n");
                            return e_success;
                        }
                        else
                        {
                            printf("Decoded the secret data is failure\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Decoded file size is failure\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Decoded secret file extension is failure\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Decoded secret file extension size is failure\n");
                return e_failure;
            }
        }
        else
        {
            printf("Decoded magic string is failure\n");
            return e_failure;
        }
    }
    else
    {
        printf("failure in Opening the files\n");
        return e_failure;
    }
    return e_success;
}


/* Defining function to open files and doing validation */
Status open_decode_files(DecodeInfo *decInfo)
{
    //Opening Stego Image file in read mode
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");
    
    //validating if error exists while opening file
    if (decInfo -> fptr_stego_image == NULL)
    {
        perror("fopen");    //perror is a function used to print just a message as a normal printf
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> stego_image_fname);

        return e_failure;
    }

    //Opening decoded.txt file in write mode
    decInfo->fptr_decoded_file = fopen(decInfo -> decoded_fname, "w");
    
    //validating if error exists while opening file
    if (decInfo -> fptr_decoded_file == NULL)
    {
        perror("fopen");    //perror is a function used to print just a message as a normal printf
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo -> decoded_fname);

        return e_failure;
    }
    return e_success;
}


/* Defining the function to decode the magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    //reading 8 bytes from 54th byte of stego image to decode 1st character '#' of magic string 
    fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
    
    //checking if decoding of lsb to byte is successfull
    if (decode_byte_from_lsb(decInfo->decode_data, decInfo->image_data) == e_success)
    {
        //then checking if the character of magic string # matched with decoded data
        if (decInfo->decode_data[0] == '#')
        {
            //reading next 8 bytes from 62nd byte of stego image to decode 2nd character '#' of magic string 
            fread(decInfo->image_data, sizeof(char), 8, decInfo->fptr_stego_image);
            
            //checking if decoding of lsb to byte is successfull
            if (decode_byte_from_lsb(decInfo->decode_data, decInfo->image_data) == e_success)
            {
                //then checking if the next character of magic string * matched with decoded data
                if (decInfo->decode_data[0] == '*');
                else
                    return e_failure;
            }
            printf("Decoded data of magic string matched\n");
            return e_success;
        }
        else
        {
            fprintf(stderr,"Error: Decoded data of magic string not matched\n");
            return e_failure;
        }
    }
    else
        return e_failure;
}


/* Defining function to decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{ 
    char str[FILE_EXTN_SIZE];     //declarng an array 'str' of datatype char with size 32 bytes
    
    //reading 32 bytes for once from stego bmp image and decoded to size fom lsb
    fread(str, sizeof(char), FILE_EXTN_SIZE, decInfo->fptr_stego_image);
    decode_size_from_lsb(str, decInfo);
    return e_success;
}


/* Defining function to decode size from lsb */
Status decode_size_from_lsb(char *buffer, DecodeInfo *decInfo)
{
    //Decoding size by running for loop for file size 32 times
    for (int i = 0; i < FILE_EXTN_SIZE; i++)
    {
        //Size is decoded for lsb using following operation
        decInfo->image_data_size <<= 1;
        decInfo->image_data_size |= (buffer[i] & 0x01);
    }
    return e_success;
}


/* Defining function to decode secret file extension */
Status decode_secret_file_extn(uint extn_size, DecodeInfo *decInfo)
{
    //Decoding extension by running for loop for image_data_size 4 times
    for (int i = 0; i < extn_size; i++)
    {
        //One byte read 8 times from stego image and stored in address of image_data
        fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
        
        //checking if decoding lsb to byte is success or failure
        if (decode_byte_from_lsb(decInfo->decode_data, decInfo->image_data) == e_success)
        {
            decInfo->extn_decoded_file[i] = decInfo->decode_data[0];      //decoding the extension of decode file
        }
        else
            return e_failure;
    }
    printf("Decoded secret file extension matched\n");
    return e_success;
}


/* Defining function to decode file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char str[FILE_SIZE];     //declarng an array 'str' of datatype char with size 32 bytes
    
    //reading 32 bytes for once from stego bmp image and decoded to size fom lsb
    fread(str, sizeof(char), FILE_SIZE, decInfo->fptr_stego_image);
    decode_size_from_lsb(str, decInfo);
    return e_success;
}


/* Defining function to decode secret file data into file */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    //for loop runs for image_dat_size times
    for (int i = 0; i < decInfo->image_data_size; i++)
    {
        //read one byte 8 times from stego image and stored in address of image_data
        fread(decInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, decInfo->fptr_stego_image);
        
        //checking if decoding lsb to byte is success or failure
        if(decode_byte_from_lsb(decInfo->decode_data, decInfo->image_data) == e_success)
        {
            fwrite(decInfo->decode_data, sizeof(char), 1, decInfo->fptr_decoded_file);    //decoded data written in decoded.txt file
        }
        else
            return e_failure;
	}
	return e_success;
}


/* Defining function to decode byte from lsb */
Status decode_byte_from_lsb(char *decode_data, char *image_data)
{
    //for loop runs for 8 bytes
    for (int i = 0; i < MAX_IMAGE_BUF_SIZE; i++)
	{
        //byte decoded from each 8 bytes with lsb data 
		decode_data[0] <<= 1;
		decode_data[0] |=  (image_data[i] & 0x01);
	}
	return e_success;
}
