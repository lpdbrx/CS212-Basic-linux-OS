#include <stdio.h>
#include <openssl/sha.h>
#include "mount.h"
#include "filev6.h"
#include <string.h>

#define MAX_LENGTH_OF_FILE (SECTOR_SIZE * (ADDR_SMALL_LENGTH-1) * ADDRESSES_PER_SECTOR)

/**
 * week05
 * 
 * @brief print the sha of the content
 * @param content the content of which we want to print the sha
 * @param length the length of the content
 */
void print_sha_from_content(const unsigned char *content, size_t length){
	unsigned char sha[SHA256_DIGEST_LENGTH];
	SHA256(content, length, sha);
	if(sha == NULL) return;
	for (size_t i = 0; i < SHA256_DIGEST_LENGTH ; i++){
		printf("%02x" ,sha[i]);
	}
}


/**
 * week05
 * 
 * @brief print the sha of the content of an inode
 * @param u the filesystem
 * @param inode the inode of which we want to print the content
 * @param inr the inode number
 */
void print_sha_inode(struct unix_filesystem *u, struct inode inod, int inr) {
    //check if the inode is valid
    if (inod.i_mode & IALLOC) {
        printf("SHA inode %d: ", inr);
        if (inod.i_mode & IFDIR) {
            printf("no SHA for directories. \n");
        } else {

            //when the inode represent a file
            struct filev6 filv6 = {u, (uint16_t) inr, inod, 0};
            char content[MAX_LENGTH_OF_FILE + 1];

            //store the content (data) of one sector in content
            int rem = filev6_readblock(&filv6, content);
            content[MAX_LENGTH_OF_FILE] = '\0';

            //while the whole content of the inode is not read, read a sector and store in currContent
            //since the content of an inode (->file or directory) might be larger than one sector
            while (rem == SECTOR_SIZE) {
                char currContent[SECTOR_SIZE + 1];
                rem = filev6_readblock(&filv6, currContent);
                currContent[SECTOR_SIZE] = '\0';
                strcat(content, currContent);
            }
            print_sha_from_content((unsigned const char *) content, strlen(content));
            memset(content, 0, sizeof(content));
            printf("\n");
        }
    }
}