def calc_Coleman_Liau_index(input : str):
    sentences = input.replace('?', '.').replace('!', '.').split('.')
    words = input.replace('?', '.').replace('!', '.').replace('.', '').split()

    while "" in sentences:
        sentences.remove("")

    no_of_words = len(words)

    no_of_letters = sum([len(word.replace('\'', '').replace(',', '')) for word in words])

    L = no_of_letters / (no_of_words/100)

    S = len(sentences) / (no_of_words/100)

    result = 0.0588 * L - 0.296 * S - 15.8

    rounded = round(result)

    if rounded < 1:
        return "Before Grade 1"
    elif rounded >= 16:
        return "Grade 16+"
    else:
        return f"Grade {rounded}"

while True:
    input_text = input("Input -> ")
    if any(char.isdigit() for char in input_text):
        print("input must me a text, withot any numbers!!!")
    else:
        break

print(calc_Coleman_Liau_index(input_text))