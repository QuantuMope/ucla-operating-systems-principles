import sys
import csv


def check_block_bounds(file_reader, total_blocks, meta_data_blocks):
    for row in file_reader[2:]:
        if row[0] is 'INDIRECT':
            block_no = row[5]
            level = ""
            if   row[2] is '3': level = "TRIPLE"
            elif row[2] is '2': level = "DOUBLE"
            if block_no < 0 or block_no > total_blocks:
                print("INVALID {}INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(level, block_no, row[1], row[3]))
            elif block_no in meta_data_blocks:
                print("RESERVED {}INDIRECT BLOCK {} IN INODE {} AT OFFSET {}".format(level, block_no, row[1], row[3]))


def get_meta_data_blocks(group):
    # SUPERBLOCK, GROUP DESCRIPTOR, FREE BLOCK LIST, FREE INODE LIST
    return [1, 2, int(group[6]), int(group[7])]


def check_off_block(block_check, block_no):
    if isinstance(block_no, list):
        for no in block_no:
            block_check[no - 1] = 1
    else:
        block_check[block_no - 1] = 1


def main():
    if len(sys.argv) != 2:
        print("Program expects exactly one argument but {} were given.".
              format(len(sys.argv)-1), file=sys.stderr)
        sys.exit(1)

    fs_name = sys.argv[1]
    entries = []

    with open(fs_name) as file:
        file_reader = csv.reader(file, delimiter=',')
        for entry in file_reader:
            entries.append(entry)

    # Since we know that there is only one group
    total_blocks = entries[0][1]
    block_check = [0 for _ in range(int(total_blocks))]

    meta_data_blocks = get_meta_data_blocks(entries[1])

    check_block_bounds(entries, total_blocks, meta_data_blocks)





if __name__ == '__main__':
    main()