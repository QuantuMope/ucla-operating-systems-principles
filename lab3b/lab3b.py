import sys
import csv
from math import ceil


def separate_lists(entries):
    block_free_list = []
    inodes = []
    inode_free_list = []
    indirects = []
    directories = []
    super_block = None
    group_descr = None
    for row in entries:
        entry_name = row[0]
        if entry_name == 'BFREE':
            block_free_list.append(int(row[1]))
        elif entry_name == 'INODE':
            inodes.append(row)
        elif entry_name == 'IFREE':
            inode_free_list.append(int(row[1]))
        elif entry_name == 'INDIRECT':
            indirects.append(row)
        elif entry_name == 'DIRENT':
            directories.append(row)
        elif entry_name == 'SUPERBLOCK':
            super_block = row
        elif entry_name == 'GROUP':
            group_descr = row
    return block_free_list, inodes, inode_free_list, indirects, directories, super_block, group_descr


def get_meta_data_blocks(super_block, group):
    """
        Must take into consideration the super block, group descriptors,
        data block bitmap, inode bitmap, and inode table.
        We can assume that there is only one group and therefore only one
        group descriptor.
    """
    block_size = int(super_block[3])
    num_inodes = int(super_block[2])
    inode_table_size = ceil((num_inodes/8)/block_size)
    meta_data_blocks = [1, 2, int(group[6]), int(group[7])]
    inode_table_block_no = int(group[8])
    # In case the inode block table is longer than one block (which it shouldn't be)
    for i in range(inode_table_block_no, inode_table_block_no + inode_table_size):
        meta_data_blocks.append(i)
    return meta_data_blocks


def check_block_validity(inodes, indirects, free_list, total_blocks, meta_data_blocks):
    exit_code = 0
    block_reference_count = [0 for _ in range(total_blocks)]
    for meta_block in meta_data_blocks:
        block_reference_count[meta_block-1] += 1

    for inode in inodes:
        if len(inode) != 27: continue  # for symbolic links with size less than 60
        for i in range(12, 27):
            block_no = int(inode[i])
            block_string = "BLOCK {} IN INODE {} AT OFFSET {}".format(block_no, inode[1], i - 12)
            if 0 < block_no < total_blocks:
                block_reference_count[block_no - 1] += 1
                if block_reference_count[block_no - 1] > 1:
                    print("DUPLICATE " + block_string)
                    exit_code += 1
                if block_no in free_list:
                    print("ALLOCATED BLOCK {} ON FREELIST".format(block_no))
                    exit_code += 1

            if block_no < 0 or block_no > total_blocks:
                print("INVALID " + block_string)
                exit_code += 1
            elif block_no in meta_data_blocks:
                print("RESERVED " + block_string)
                exit_code += 1

    for indirect in indirects:
        block_no = int(indirect[5])
        level = ""
        if indirect[2] == '3':
            level = "TRIPLE"
        elif indirect[2] == '2':
            level = "DOUBLE"
        block_string = "{}INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(level, block_no, indirect[1], indirect[3])
        if 0 < block_no < total_blocks:
            block_reference_count[block_no - 1] += 1
            if block_reference_count[block_no - 1] > 1:
                print("DUPLICATE " + block_string)
                exit_code += 1
            if block_no in free_list:
                print("ALLOCATED BLOCK {} ON FREELIST".format(block_no))
                exit_code += 1

        if block_no < 0 or block_no > total_blocks:
            print("INVALID " + block_string)
            exit_code += 1
        elif block_no in meta_data_blocks:
            print("RESERVED " + block_string)
            exit_code += 1

    for block_no, count in enumerate(block_reference_count):
        if count == 0 and block_no+1 not in free_list:
            print("UNREFERENCED BLOCK {}".format(block_no+1))
            exit_code += 1

    return exit_code


def check_inode_validity(inodes, inode_free_list, total_inodes):
    exit_code = 0
    inode_reference_count = [0 for _ in range(total_inodes)]
    for inode in inodes:
        inode_no = int(inode[1])
        inode_reference_count[inode_no-1] += 1
        if inode_no in inode_free_list:
            print("ALLOCATED INODE {} ON FREELIST".format(inode_no))
            exit_code += 1
    for inode_no, count in enumerate(inode_reference_count):
        if count == 0 and inode_no+1 not in inode_free_list:
            print("UNALLOCATED INODE {} NOT ON FREELIST".format(inode_no+1))
            exit_code += 1

    return exit_code


def check_directory_validity(inodes, total_inodes, directories, inode_free_list):
    exit_code = 0
    link_counts = [0 for _ in range(total_inodes)]
    for directory in directories:
        link_counts[int(directory[1])-1] += 1
    for inode in inodes:
        inode_no = int(inode[1])
        l_count = int(inode[6])
        links = link_counts[inode_no-1]
        if inode[2] == 'd' and l_count != links:
            print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(inode_no, links, l_count))
            exit_code += 1

    for directory in directories:
        parent_no = int(directory[1])
        inode_no = int(directory[3])
        if inode_no < 1 or inode_no > total_inodes:
            print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(parent_no, directory[6], inode_no))
            exit_code += 1
        elif inode_no in inode_free_list:
            print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(parent_no, directory[6], inode_no))
            exit_code += 1

    for directory in directories:
        parent_no = int(directory[1])
        inode_no = int(directory[3])
        name = directory[6]
        if name == "'.'" and parent_no != inode_no:
            print("DIRECTORY INODE {} NAME {} LINK TO INODE {} SHOULD BE {}".format(inode_no, name, parent_no, inode_no))
            exit_code += 1
        elif name == "'..'" and inode_no == 2 and parent_no != 2:  # special case due to root
            print("DIRECTORY INODE 2 NAME '..' LINK TO INODE {} SHOULD BE 2".format(parent_no))
            exit_code += 1

    return exit_code


def main():
    if len(sys.argv) != 2:
        print("Program expects exactly one argument but {} were given.".
              format(len(sys.argv)-1), file=sys.stderr)
        sys.exit(1)

    fs_name = sys.argv[1]
    entries = []

    try:
        file = open(fs_name)
    except FileNotFoundError:
        print("File {} could not be found".format(fs_name), file=sys.stderr)
        sys.exit(1)
    except IOError:
        print("File {} could not be opened/read".format(fs_name), file=sys.stderr)
        sys.exit(1)

    file_reader = csv.reader(file, delimiter=',')
    for entry in file_reader:
        entries.append(entry)
    file.close()

    # Since we know that there is only one group
    block_free_list, inodes, inode_free_list, indirects, directories, super_block, group_descr = separate_lists(entries)
    meta_data_blocks = get_meta_data_blocks(super_block, group_descr)

    total_blocks = int(super_block[1])
    total_inodes = int(super_block[2])

    exit_codes = [check_block_validity(inodes, indirects, block_free_list, total_blocks, meta_data_blocks),
                  check_inode_validity(inodes, inode_free_list, total_inodes),
                  check_directory_validity(inodes, total_inodes, directories, inode_free_list)]

    for code in exit_codes:
        if code != 0:
            sys.exit(2)

    sys.exit(0)


if __name__ == '__main__':
    main()