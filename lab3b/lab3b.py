# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339


import sys
import csv
from math import ceil


def separate_lists(entries):
    block_free_list = []
    inodes = []
    inode_free_list = []
    indirects = []
    direct_entries = []
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
            direct_entries.append(row)
        elif entry_name == 'SUPERBLOCK':
            super_block = row
        elif entry_name == 'GROUP':
            group_descr = row
    return block_free_list, inodes, inode_free_list, indirects, direct_entries, super_block, group_descr


def get_meta_data_blocks(super_block, group):
    """
        Must take into consideration the super block, group descriptors,
        data block bitmap, inode bitmap, and inode table.
        We can assume that there is only one group and therefore only one
        group descriptor.
    """
    block_size = int(super_block[3])
    num_inodes = int(super_block[2])
    inode_size = int(super_block[4])
    inode_table_size = ceil((num_inodes * inode_size)/block_size)
    meta_data_blocks = [0, 1, 2, int(group[6]), int(group[7])]
    inode_table_block_no = int(group[8])
    # In case the inode block table is longer than one block (which it shouldn't be)
    for i in range(inode_table_block_no, inode_table_block_no + inode_table_size):
        meta_data_blocks.append(i)
    return meta_data_blocks


def check_block_validity(inodes, indirects, free_list, total_blocks, meta_data_blocks):
    exit_code = 0
    block_reference_count = {i: [] for i in range(total_blocks)}

    for inode in inodes:
        if len(inode) != 27: continue  # for symbolic links with size less than 60
        for i in range(12, 27):
            block_no = int(inode[i])
            ptr_type = ""
            offset = i - 12
            if i == 24:
                ptr_type = "INDIRECT "
                offset = 12
            elif i == 25:
                ptr_type = "DOUBLE INDIRECT "
                offset = 268
            elif i == 26:
                ptr_type = "TRIPLE INDIRECT "
                offset = 65804
            block_string = "{}BLOCK {} IN INODE {} AT OFFSET {}".format(ptr_type, block_no, inode[1], offset)
            if 0 < block_no < total_blocks:
                block_reference_count[block_no].append(block_string)
                if len(block_reference_count[block_no]) > 1:
                    if len(block_reference_count[block_no]) == 2:
                        print("DUPLICATE " + block_reference_count[block_no][0])
                    print("DUPLICATE " + block_string)
                    exit_code += 1
                if block_no in free_list:
                    print("ALLOCATED BLOCK {} ON FREELIST".format(block_no))
                    exit_code += 1

            if block_no < 0 or block_no > total_blocks:
                print("INVALID " + block_string)
                exit_code += 1
            elif block_no in meta_data_blocks[1:]:
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
            block_reference_count[block_no].append(block_string)
            if len(block_reference_count[block_no]) > 1:
                if len(block_reference_count[block_no]) == 2:
                    print("DUPLICATE " + block_reference_count[block_no][0])
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

    first_block = len(meta_data_blocks)
    for block_no in range(first_block, total_blocks):
        count = len(block_reference_count[block_no])
        if count == 0 and block_no not in free_list:
            print("UNREFERENCED BLOCK {}".format(block_no))
            exit_code += 1

    return exit_code


def check_inode_validity(inodes, inode_free_list, total_inodes, first_inode):
    exit_code = 0
    inode_reference_count = [0 for _ in range(total_inodes)]
    for inode in inodes:
        inode_no = int(inode[1])
        inode_reference_count[inode_no-1] += 1
        if inode_no in inode_free_list:
            print("ALLOCATED INODE {} ON FREELIST".format(inode_no))
            inode_free_list.remove(inode_no)
            exit_code += 1
    for inode_no in range(first_inode, total_inodes):
        count = inode_reference_count[inode_no-1]
        if count == 0 and inode_no not in inode_free_list:
            print("UNALLOCATED INODE {} NOT ON FREELIST".format(inode_no))
            exit_code += 1

    return exit_code, inode_reference_count


def check_directory_validity(inodes, total_inodes, direct_entries, inode_free_list):
    exit_code = 0
    link_counts = [0 for _ in range(total_inodes)]

    for direct_entry in direct_entries:
        direct_ino = int(direct_entry[1])
        file_ino = int(direct_entry[3])
        name = direct_entry[6]
        if file_ino < 1 or file_ino > total_inodes:
            print("DIRECTORY INODE {} NAME {} INVALID INODE {}".format(direct_ino, name, file_ino))
            exit_code += 1
        elif file_ino in inode_free_list:
            print("DIRECTORY INODE {} NAME {} UNALLOCATED INODE {}".format(direct_ino, name, file_ino))
            exit_code += 1

        if name == "'.'" and file_ino != direct_ino:
            print("DIRECTORY INODE {} NAME {} LINK TO INODE {} SHOULD BE {}".format(direct_ino, name, file_ino, direct_ino))
            exit_code += 1
        elif name == "'..'" and direct_ino == 2 and file_ino != 2:  # special case due to root
            print("DIRECTORY INODE 2 NAME '..' LINK TO INODE {} SHOULD BE 2".format(file_ino))
            exit_code += 1

        if 0 < file_ino < total_inodes:
            link_counts[file_ino-1] += 1

    for inode in inodes:
        inode_no = int(inode[1])
        l_count = int(inode[6])
        links = link_counts[inode_no-1]
        if l_count != links:
            print("INODE {} HAS {} LINKS BUT LINKCOUNT IS {}".format(inode_no, links, l_count))
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
    block_free_list, inodes, inode_free_list, indirects, direct_entries, super_block, group_descr = separate_lists(entries)
    meta_data_blocks = get_meta_data_blocks(super_block, group_descr)

    total_blocks = int(super_block[1])
    total_inodes = int(super_block[2])
    first_inode  = int(super_block[7])

    exit_codes = [check_block_validity(inodes, indirects, block_free_list, total_blocks, meta_data_blocks),
                  check_inode_validity(inodes, inode_free_list, total_inodes, first_inode),
                  check_directory_validity(inodes, total_inodes, direct_entries, inode_free_list)]

    for code in exit_codes:
        if code != 0:
            sys.exit(2)

    sys.exit(0)


if __name__ == '__main__':
    main()