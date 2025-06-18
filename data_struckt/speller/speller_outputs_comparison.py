def find_missing_words():
    # Read both files
    with open('my_speller_output.txt', 'r') as f:
        my_words = [line.strip() for line in f if line.strip()]

    with open('cs50_speller_output.txt', 'r') as f:
        cs50_words = [line.strip() for line in f if line.strip()]

    print(f"My output has {len(my_words)} words")
    print(f"CS50 output has {len(cs50_words)} words")
    print()

    # Find differences
    my_set = set(my_words)
    cs50_set = set(cs50_words)

    # Words in CS50 but not in mine (missing from my output)
    missing_from_mine = cs50_set - my_set

    # Words in mine but not in CS50 (extra in my output)
    extra_in_mine = my_set - cs50_set

    if missing_from_mine:
        print("Words missing from my output:")
        for word in sorted(missing_from_mine):
            print(f"  {word}")
        print()

    if extra_in_mine:
        print("Extra words in my output:")
        for word in sorted(extra_in_mine):
            print(f"  {word}")
        print()

    # Find position differences
    print("Position-by-position comparison:")
    max_len = max(len(my_words), len(cs50_words))

    for i in range(max_len):
        my_word = my_words[i] if i < len(my_words) else "[END]"
        cs50_word = cs50_words[i] if i < len(cs50_words) else "[END]"

        if my_word != cs50_word:
            print(f"Position {i+1}:")
            print(f"  My output:  {my_word}")
            print(f"  CS50 output: {cs50_word}")
            print()
            # Only show first few differences to avoid spam
            if i > 10:
                print("... (stopping after first several differences)")
                break

if __name__ == "__main__":
    find_missing_words()