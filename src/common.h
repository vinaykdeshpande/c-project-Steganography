//Conditional compilation done

#ifndef COMMON_H
#define COMMON_H

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

/* Setting variable MAX_SECRET_BUF_SIZE to 1 */
#define MAX_SECRET_BUF_SIZE 1

/* Setting variable MAX_IMAGE_BUF_SIZE to 8 */
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
 
/* Setting variable MAX_FILE_SUFFIX to 4 */
#define MAX_FILE_SUFFIX 4

/* Setting file extention size to 32 bytes for encoding and decoding */
#define FILE_EXTN_SIZE 32

/* Setting file size to 32 bytes for encoding and decoding */
#define FILE_SIZE 32

#endif
