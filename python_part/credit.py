card_nums_mappings = {
    34 : "AMEX",
    37 : "AMEX",
    51 : "MasterCard",
    52 : "MasterCard",
    53 : "MasterCard",
    54 : "MasterCard",
    55 : "MasterCard",
    40 : "VISA",
    41 : "VISA",
    42 : "VISA",
    43 : "VISA",
    44 : "VISA",
    45 : "VISA",
    46 : "VISA",
    47 : "VISA",
    48 : "VISA",
    49 : "VISA"
}


def check_valid(number : list):
    doubled_nums = [num*2 for num in number[-2::-2]]
    not_doubled_nums = [num for num in number[-1::-2]]

    doubled_nums = [num // 10 + num % 10 for num in doubled_nums]
    not_doubled_nums = [num // 10 + num % 10 for num in not_doubled_nums]

    sum1 = sum(doubled_nums)
    sum2 = sum(not_doubled_nums)
    return (sum1 + sum2) % 10 == 0

def what_provider(number : list):
    print("The number is {}".format("valid" if check_valid(number) else "invalid"))
    print("And is from: {}".format(card_nums_mappings[number[0]*10 + number[1]]))


while True:
    card_number = input("Card number: ")
    if card_number.isdecimal() and card_number.isdigit() and len(card_number) in [13, 15, 16]:
        break
    else:
        print("INVALID")

what_provider([int(char) for char in card_number.strip()])