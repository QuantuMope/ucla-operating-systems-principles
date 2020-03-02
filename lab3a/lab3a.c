#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "ext2_fs.h"

/*
    Assumption from spec: Group size is 1
*/

#define SB_OFFSET 1024

int fs_fd;
struct ext2_super_block sb;
struct ext2_group_desc gd;
int bsize;
int num_groups;


void scan_block_bitmap(int num_blocks) {
    // Since we know there is only one group, we know that the blockmap will
    // be right after the group descriptor.
    int offset = gd.bg_block_bitmap * bsize;
    int bitmap_size = sizeof(uint8_t) * (num_blocks/8);
    uint8_t* block_bitmap = malloc(bitmap_size);
    if (pread(fs_fd, block_bitmap, bitmap_size, offset) < 0) {
        fprintf(stderr, "Failed to read free block bitmap: %s\n", strerror(errno));
        exit(1);
    }
    for (int i = 0; i < (num_blocks/8); i++) {
        for (int j = 0; j < 8; j++) {
            if ((block_bitmap[i] & (0x01 << j)) == 0)
                printf("BFREE,%d\n", i*8+j+1);
        }
    }
    free(block_bitmap);
}

void print_directory_entries(int inode_no, int offset) {
    struct ext2_dir_entry dir_entry;
    int bytes = 0;
    while (bytes < bsize) {
        if (pread(fs_fd, &dir_entry, sizeof(dir_entry), offset + bytes) < 0) {
            fprintf(stderr, "Failed to read directory entry: %s\n", strerror(errno));
            exit(1);
        }
        if (dir_entry.inode != 0)
            printf("DIRENT,%d,%d,%d,%d,%d,\'%s\'\n", inode_no,
                                                     bytes,
                                                     dir_entry.inode,
                                                     dir_entry.rec_len,
                                                     dir_entry.name_len,
                                                     dir_entry.name);
        bytes += dir_entry.rec_len;
    }
}

void scan_direct_directories(int inode_no, int offset) {
    int block_no;
    for (int i = 0; i < EXT2_NDIR_BLOCKS; i++) {
        if (pread(fs_fd, &block_no, sizeof(block_no), offset + (i*sizeof(block_no))) < 0) {
            fprintf(stderr, "Failed to read block pointer: %s\n", strerror(errno));
            exit(1);
        }
        if (block_no != 0) {
            print_directory_entries(inode_no, block_no*bsize);
        }
    }
}

void scan_indirect_ptrs(int inode_no, int level, int prev_block_no, int log_offset, char file_type) {
    if (level == 0) {
        if (file_type == 'c')
            print_directory_entries(inode_no, prev_block_no * bsize);   
        return;
    }
    int block_no;
    int block_offset;
    int blocks_per_1 = bsize / sizeof(block_no);
    int blocks_per_2 = blocks_per_1 * blocks_per_1;
    for (int i = 0; i < bsize; i+=sizeof(block_no)) {
        if (pread(fs_fd, &block_no, sizeof(block_no), prev_block_no*bsize + i) < 0) {
            fprintf(stderr, "Failed to read level %d pointer: %s\n", level, strerror(errno));
            exit(1);
        }
        if (block_no != 0) {
            block_offset = i/4;
            switch (level) {
                case 1:
                    log_offset += block_offset;
                    break;
                case 2:
                    log_offset += block_offset * blocks_per_1;
                    break;
                case 3:
                    log_offset += block_offset * blocks_per_2;
            }
            printf("INDIRECT,%d,%d,%d,%d,%d\n", inode_no,
                                                level,
                                                log_offset,
                                                prev_block_no,
                                                block_no);
            scan_indirect_ptrs(inode_no, level-1, block_no, log_offset, file_type);
        }
    }
}

void scan_inode(int inode_no) {
    struct ext2_inode inode;
    int offset = (gd.bg_inode_table * bsize) + (sizeof(inode) * (inode_no-1));
    if (pread(fs_fd, &inode, sizeof(inode), offset) < 0) {
        fprintf(stderr, "Failed to read inode entry: %s\n", strerror(errno));
        exit(1);
    }

    if (inode.i_mode == 0 || inode.i_links_count == 0)
        return;

    // Get the file type
    char file_type;
    int mode_mask = inode.i_mode & 0xF000;
    if (mode_mask == 0x8000)
        file_type = 'f';
    else if (mode_mask == 0x4000)
        file_type = 'd';
    else if (mode_mask == 0xA000)
        file_type = 's';
    else
        file_type = '?';

    // Get the inode times translated
    char times[3][40] = {0};
    struct tm ts;
    time_t raw_times[3] = {inode.i_ctime, inode.i_mtime, inode.i_atime};
    for (int i = 0; i < 3; i++) {
        ts = *gmtime(raw_times+i);
        strftime(times[i], sizeof(times[i]), "%m/%d/%g %T", &ts);
    }

    printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d", inode_no,
                                                     file_type,
                                                     (inode.i_mode & 0xFFF),
                                                     inode.i_uid,
                                                     inode.i_gid,
                                                     inode.i_links_count,
                                                     times[0],
                                                     times[1],
                                                     times[2],
                                                     inode.i_size,
                                                     inode.i_blocks);
    // File type specific outputs
    switch (file_type) {
        case 'f':
        case 'd':
            for (int i = 0; i < EXT2_N_BLOCKS; i++)
                printf(",%d", inode.i_block[i]);
            break;
        case 's':
            if (inode.i_size < 60)
                break;
            for (int i = 0; i < EXT2_N_BLOCKS; i++)
                printf(",%d", inode.i_block[i]);
    }
    printf("\n");

    // Print the directories from direct ptrs
    if (file_type == 'd')
        scan_direct_directories(inode_no, offset+40); 
    // Recurse through all indirect pointers for files and directories
    // Print directories found linked to indirect pointers
    int ptrs_per_block = bsize / sizeof(int);
    int offset_2 = EXT2_NDIR_BLOCKS + ptrs_per_block;
    int offset_3 = offset_2 + ptrs_per_block * ptrs_per_block;
    int log_offsets[3] = {EXT2_NDIR_BLOCKS, offset_2, offset_3};
    switch (file_type) {
        case 'd':
        case 'f':
            for (int i = 0; i < 3; i++)
                scan_indirect_ptrs(inode_no, i+1, inode.i_block[i+12], log_offsets[i], file_type);
    }
}


void scan_inode_bitmap(int num_inodes) {
    int offset = gd.bg_inode_bitmap * bsize;
    int bitmap_size = sizeof(uint8_t) * (num_inodes/8);
    uint8_t* inode_bitmap = malloc(bitmap_size);
    if (pread(fs_fd, inode_bitmap, bitmap_size, offset) < 0) {
        fprintf(stderr, "Failed to read free inode bitmap: %s\n", strerror(errno));
        exit(1);
    }
    for (int i = 0; i < (num_inodes/8); i++) {
        for (int j = 0; j < 8; j++) {
            if ((inode_bitmap[i] & (0x01 << j)) == 0)
                printf("IFREE,%d\n", i*8+j+1);
            else
                scan_inode(i*8+j+1);
        }
    }
    free(inode_bitmap);
}



void scan_groups() {
    int num_blocks;
    int num_inodes;
    int group_offset = SB_OFFSET + sizeof(sb);
    // num_groups is always 1 but this code should handle file systems with more than one group
    for (int i = 0; i < num_groups; i++) {
        if (pread(fs_fd, &gd, sizeof(gd), group_offset + (i*sizeof(gd))) < 0) {
            fprintf(stderr, "Failed to read group descriptor data: %s\n", strerror(errno));
            exit(1);
        }
        num_blocks = ((i + 1 == num_groups) && (sb.s_blocks_count % sb.s_blocks_per_group != 0)) ? \
                     sb.s_blocks_count % sb.s_blocks_per_group : sb.s_blocks_per_group;

        num_inodes = ((i + 1 == num_groups) && (sb.s_inodes_count % sb.s_inodes_per_group != 0)) ? \
                     sb.s_inodes_count % sb.s_inodes_per_group : sb.s_inodes_per_group;

        printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", i,
                                                  num_blocks,
                                                  sb.s_inodes_per_group,
                                                  gd.bg_free_blocks_count,
                                                  gd.bg_free_inodes_count,
                                                  gd.bg_block_bitmap,
                                                  gd.bg_inode_bitmap,
                                                  gd.bg_inode_table);
        scan_block_bitmap(num_blocks);
        scan_inode_bitmap(num_inodes);
    }
}

void scan_file_system() {
    // Start with scanning the superblock.
    if (pread(fs_fd, &sb, sizeof(sb), SB_OFFSET) < 0) {
        fprintf(stderr, "Failed to read super block data: %s\n", strerror(errno));
        exit(1);
    }
    if (sb.s_magic != EXT2_SUPER_MAGIC) {
        fprintf(stderr, "Incorrectly scanned super block\n");
        exit(1);
    }
    bsize = EXT2_MIN_BLOCK_SIZE << sb.s_log_block_size;
    // num_groups will always be 1 for this assignment.
    num_groups = ceil((double)sb.s_blocks_count / sb.s_blocks_per_group);
    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", sb.s_blocks_count,
                                                sb.s_inodes_count,
                                                bsize,
                                                sb.s_inode_size,
                                                sb.s_blocks_per_group,
                                                sb.s_inodes_per_group,
                                                sb.s_first_ino);
    // Scan group descriptors.
    scan_groups();
}



int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Program expects exactly one input but %d were given\n", argc-1);
        exit(1);
    }

    if ((fs_fd = open(argv[1], O_RDONLY)) < 0)  {
        fprintf(stderr, "Failed to open the file system.\n");
        exit(1);
    }

    // Scan the file system. Starts a series of recursive function calls
    // that scans the whole file system.
    scan_file_system();

    exit(0);

}