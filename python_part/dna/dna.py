import csv
import sys


def main():

    # TODO: Check for command-line usage
    while True:
        if len(sys.argv) != 3:
            print("Wrong number of command line args!")
            quit(1)
            continue
        else:
            file_path_database = sys.argv[1]
            file_path_measurement = sys.argv[2]
            break

    # TODO: Read database file into a variable
    rows = []
    with open(file_path_database, 'r') as file_database:
        reader = csv.DictReader(file_database)
        for row in reader:
            rows.append(row)

    # TODO: Read DNA sequence file into a variable
    dna_sample = None
    with open(file_path_measurement, 'r') as file_measurement:
        reader = csv.reader(file_measurement)
        dna_sample = reader.__next__()[0]

    # TODO: Find longest match of each STR in DNA sequence
    STRs = list(rows[0].keys())
    STRs.pop(0)
    print(STRs)

    longest_matches = {}

    for STR in STRs:
        longest_matches[STR] = longest_match(dna_sample, STR)

    # TODO: Check database for matching profiles
    name = None
    for row in rows:
        name = row['name']
        for key in longest_matches.keys():
            if longest_matches[key] != int(row[key]):
                name = None
        if name is not None:
            break

    if name is None:
        print("No match")
    else:
        print(name)

    return 0


def longest_match(sequence : str, subsequence : str):
    """Returns length of longest run of subsequence in sequence."""

    # Initialize variables
    longest_run = 0
    subsequence_length = len(subsequence)
    sequence_length = len(sequence)

    # Check each character in sequence for most consecutive runs of subsequence
    for i in range(sequence_length):

        # Initialize count of consecutive runs
        count = 0

        # Check for a subsequence match in a "substring" (a subset of characters) within sequence
        # If a match, move substring to next potential match in sequence
        # Continue moving substring and checking for matches until out of consecutive matches
        while True:

            # Adjust substring start and end
            start = i + count * subsequence_length
            end = start + subsequence_length

            # If there is a match in the substring
            if sequence[start:end] == subsequence:
                count += 1

            # If there is no match in the substring
            else:
                break

        # Update most consecutive matches found
        longest_run = max(longest_run, count)

    # After checking for runs at each character in seqeuence, return longest run found
    return longest_run


main()
