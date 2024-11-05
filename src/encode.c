#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"


/* Defining function to Get File names and validate
 * Input: Beautiful.bmp file, secret.txt and stego.bmp
 * Output: Contents of secret.txt hidden in Stego.bmp image
 * Return Value: e-success or e_failure, on file errors
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //checking if the third argument is .bmp file or not
    if (strcmp(strstr(argv[2], ".bmp"), ".bmp") == 0)
    {
        encInfo -> src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    //checking if the fourth argument is .txt file or not
    if (strcmp(strstr(argv[3], ".txt"), ".txt") == 0)
    {
        encInfo -> secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }

    //checking if the fifth argument is passed or not
    if (argv[4] != NULL)
    {
        encInfo -> stego_image_fname = argv[4];       //assigning the 5th argument already passed to pointer stego_image_fname
    }
    else
    {
        encInfo -> stego_image_fname = "stego.bmp";   //creating stego.bmp file if 5th argument not passed and assigned
    }
    return e_success;
}



/* Defining function : Encoding contents of secret.txt in stego.bmp image
 * Input: Beautiful.bmp image, secret.txt and stego.bmp image
 * Output: Encoded message copied from secret.txt in stego.bmp image
 * Return: e_success or e_failure, on file error
*/
Status do_encoding(EncodeInfo *encInfo)
{
    printf("**********Encoding Procedure Started**********\n");
    
    //To check files opened successfully or not
    if (open_files(encInfo) == e_success)
    {
        printf("Open Files is successful\n");
    
        //To check whether files successfully have the required size or not
        if (check_capacity(encInfo) == e_success)
        {
            printf("Check capacity is success\n");
    
            //To check whether source file header copied successfully to stego image successfully or not
            if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
            {
                printf("Copied bmp header is success\n");
           
                //To check whether magic string encoded successfully or not
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Encoded Magic string is success\n");
           
                    strcpy(encInfo -> extn_secret_file, strstr(encInfo -> secret_fname, "."));  //copying as the extn from secret file name 
                    //To check whether secret file extension size is encoded successfully or not
                    if (encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                    {
                        printf("Encoded secret file extension size is success\n");
                    
                        //To check whether secret file extension is encoded successfully or not
                        if (encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encoded Secret file extension is success\n");
                        
                            //To check whether secret file size is encoded successfully or not
                            if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
                            {
                                printf("Encoded secret file size is success\n");
                            
                                //To check whether secret file data is encoded successfully or not
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoded secret file data is success\n");
                                    if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
                                    {
                                        printf("Copied remaining data is success\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy remaining data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Failed to Encode secret file data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Failed to Encode secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Encoded secret file extension was not success\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to Encode secret file extention size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to encode Magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Copied bmp header was failure\n");
            }
        }
        else
        {
            printf("Check capacity was failure\n");
            return e_failure;
        }
    }
    else
    {
        printf("Open Files was failure\n");
        return e_failure;
    }    
    return e_success;
}



/* Defining function to Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18, and height after that. size is 4 bytes
*/
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;   //declaring variables width and height of datatype unsigned int
   
    //setting file pointer pointing to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}



/* Defining function to open files and doing validation */
Status open_files(EncodeInfo *encInfo)
{
    //Opening Source Image file in read mode
    encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "r");
    
    //validating if error exists while opening file
    if (encInfo -> fptr_src_image == NULL)
    {
        perror("fopen");    
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> src_image_fname);
        return e_failure;
    }

    //Opening secret file in read mode
    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "r");
    
    //validating if error exists while opening file
    if (encInfo -> fptr_secret == NULL)
    {
        perror("fopen");    
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> secret_fname);
        return e_failure;
    }

    //Opening stego.bmp file in write mode
    encInfo->fptr_stego_image = fopen(encInfo -> stego_image_fname, "w");
    
    //validating if error exists while opening file
    if (encInfo -> fptr_stego_image == NULL)
    {
        perror("fopen");    
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo -> stego_image_fname);
        return e_failure;
    }
    return e_success;
}


/* Defining the function to set the size/capacity to an image */
Status check_capacity(EncodeInfo *encInfo)
{
    //getting image capacity by calling the function 'get_image_size_for_bmp'
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);  
    
    //getting size of secret file by calling the function 'get_image_size_for_bmp'
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

    //checking if the required image capacity obtained or not
    if (encInfo -> image_capacity > (16 + 32 + 32 + 32 + (8 * encInfo -> size_secret_file)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


/* Defining function to get the file size */
uint get_file_size(FILE *fptr)     
{
    //Settinhg file pointer to the end position
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);    //returning the end position value which is the size of file
}


/* Defining function to copy header of source bmp file to destination bmp file */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char str[54];    //declarng an array 'str' of datatype char with size 54
    //setting file pointer to 1st byte
    fseek(fptr_src_image, 0, SEEK_SET);
    //reading 54 bytes header from source bmp image and writing it into destination bmp image
    fread(str, sizeof(char), 54, fptr_src_image);
    fwrite(str, sizeof(char), 54, fptr_dest_image);
    return e_success;
}



/* Defining the function to encode the magic string */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //each element of magic string encoded one after other by calling function 'encode_data_to_image'
    encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}



/* Defining as generic function to encode magic string */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
        //Each character read from source image and encoded with 8-bytes and written into stego image bmp file
        fread(encInfo -> image_data, sizeof(char), 8, encInfo -> fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo -> image_data);
        fwrite(encInfo -> image_data, sizeof(char), 8, encInfo -> fptr_stego_image);
    }
    return e_success;
}


/* Defining as generic function to encode byte to lsb to encode bytes to lsb */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    uint mask = 1 << 7;           //mask of unsigned int type is declared and assigned with shift operation(1<<7)

    //each byte of data requires 8 bytes each to make it as lsb
    for (int i = 0; i < 8; i++)
    {
        //each byte is encoded with 8 bytes and below operation is performed to encode into lsb
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i)); 
        mask = mask >> 1;           
    }
}


/* Defining function to encode secret file extension size */
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_dest_image) 
{
    char str[FILE_SIZE];     //declarng an array 'str' of datatype char with size 32 bytes

    //reading 32 bytes for once from source bmp image and encoded to lsb and  writing encoded secret file extension size into destination bmp image
    fread(str, FILE_SIZE, sizeof(char), fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, FILE_SIZE, sizeof(char), fptr_dest_image);
    return e_success;
}



/* Defining as generic function to encode size(int) int lsb */
Status encode_size_to_lsb(char *buffer, int size)
{
    uint mask = 1 << 31;          //mask of unsigned int type is declared and assigned with shift operation(1<<31)
    
    //each size(int) requires 32 bytes each to make it as lsb
    for (int i = 0; i < 32; i++)
    {
        //each integer size is encoded with 32 bytes and below operation is performed to encode into lsb
        buffer[i] = (buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;          
    }
}


/* Defining function to encode secret file extension */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    //each byte of secret file extension encoded one after other by calling function 'encode_data_to_image'
    encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}



/* Defining function to encode secret file size */
Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
    char str[FILE_SIZE];     //declarng an array 'str' of datatype char with size 32 bytes

    //reading 32 bytes for once from source bmp image and encoded to lsb and  writing encoded secret file extension size into destination bmp image
    fread(str, FILE_SIZE, sizeof(char), encInfo -> fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, FILE_SIZE, sizeof(char), encInfo -> fptr_stego_image);
    return e_success;
}



/* Defining function to encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //Setting file pointer to 1st byte of the secret file
    fseek(encInfo -> fptr_secret, 0, SEEK_SET);     
    char ch;   //declaring variable 'ch' of char datatype
    
    //for loop runs secret file size no of times
    for (int i = 0; i < encInfo -> size_secret_file; i++)  
    {
        fread(encInfo -> image_data, 8, 1, encInfo -> fptr_src_image);    //reading 8 bytes once from fptr_src_image & stored in address of image_data
        fread(&ch, 1, 1, encInfo -> fptr_secret);                         //reading 1 character once from the secret file & stores in address of ch
        //encoding each byte into lsb
        encode_byte_to_lsb(ch, encInfo -> image_data);
        fwrite(encInfo -> image_data, 8, 1, encInfo -> fptr_stego_image); //writting 8-bytes once to fptr_stego_image from the address of image_data
    }
    return e_success;
}


/* Defining function to copy remaining data of source bmp image to stego bmp image to make stego bmp image to look similar to source bmp image */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char ch;   //declaring variable 'ch' of char datatype

    //Each byte read on after other from source image and sent to destination image till it reaches EOF 
    while (fread(&ch, 1, 1, fptr_src_image) > 0)    
    {
        fwrite(&ch, 1, 1, fptr_dest_image);         
    }
    return e_success;
}


