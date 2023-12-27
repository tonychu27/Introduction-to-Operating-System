import random

n = int(input("How many random number you want to generate ? "))

random_numbers = [random.randint(-10000, 10000) for _ in range(n)]

with open('input.txt', 'w') as file:
    file.write(str(n) + ' ' + '\n' )
    for number in random_numbers:
        file.write(str(number) + ' ')