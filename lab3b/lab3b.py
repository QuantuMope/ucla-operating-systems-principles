import sys
import csv


def main():
    if len(sys.argv) != 2:
        print("Program expects exactly one argument but {} were given.".
              format(len(sys.argv)-1), file=sys.stderr)
        sys.exit(1)

    fs_name = sys.argv[1]

    with open(fs_name) as file:
        file_reader = csv.reader(file, delimiter=',')
        for row in file_reader:
            for entry in row:
                print(entry)


if __name__ == '__main__':
    main()