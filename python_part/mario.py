while True:
    try:
        number = int(input("What height? -> "))
    except Exception:
        print("height must be an Integer!")
    else:
        break

print(f"You inputed the number {number}")

for i in range(1, number+1):
    one_half = " "*(number-i) + "#"*i
    print(one_half, '  ', one_half[::-1], sep='')