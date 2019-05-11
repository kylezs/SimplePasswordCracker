from itertools import permutations

lower_alphabet = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z']
upper_alphabet = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z']

output_path = "output_words.txt"

def gen_4_letter_all_perms():
    f= open(output_path, "a+")

    perms = permutations(lower_alphabet, 4)

    for tup in list(perms):
        str =  ''.join(tup)
        f.write("%s\n" % (str))

gen_4_letter_all_perms()