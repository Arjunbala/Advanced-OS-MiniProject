def generate_big_random_letters(filename,size):
    """
    generate big random letters/alphabets to a file
    :param filename: the filename
    :param size: the size in bytes
    :return: void
    """
    import random
    import string

    chars = ''.join([random.choice(string.letters) for i in range(size)]) #1


    with open(filename, 'w') as f:
        f.write(chars)
    pass

if __name__ == '__main__':
    generate_big_random_letters("temp_big_letters_direct.txt",1024*1024*10)
