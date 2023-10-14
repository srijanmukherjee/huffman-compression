import sys
import random
import string

def main():
    if len(sys.argv) != 3:
        print(f"usage: python {sys.argv[0]} OUTPUT_FILE_PATH FILE_SIZE_IN_BYTES", file=sys.stderr)
        exit(1)

    output_path = sys.argv[1]
    file_size = int(sys.argv[2])

    characters = string.ascii_lowercase + string.ascii_lowercase + string.digits + ' \t\n'

    with open(output_path, 'w') as fp:
        bytes_left = file_size

        while bytes_left > 0:
            length = min(bytes_left, 100 * 1024 * 1024) # write 100MB at a time
            fp.write(''.join(random.choices(characters, k=length)))
            bytes_left -= length

if __name__ == '__main__':
    main()
