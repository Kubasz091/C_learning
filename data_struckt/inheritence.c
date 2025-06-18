#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct person {
    char *name;
    char *blood_type_alleles; // e.g., "AB", "OO", etc.
    struct person *parents[2];
} person;

char* get_random_name(int male);
char* get_random_blood_type();
void print_family(person *p, int generation);
person* create_family(int generations);
void free_family(person *p);
void print_family(person *p, int generation);


// possible blood types: OO, OA, OB, AO, AA, AB, BO, BA, and BB
// alleles: A, B and O

char *blood_types[] = {
    "OO", "OA", "OB", "AO", "AA", "AB", "BO", "BA", "BB"
};

char *names_male[] = {
    "John", "Michael", "David", "James", "Robert",
    "William", "Joseph", "Charles", "Thomas", "Daniel"
};
char *names_female[] = {
    "Mary", "Jennifer", "Linda", "Patricia", "Elizabeth",
    "Barbara", "Susan", "Jessica", "Sarah", "Karen"
};


int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc != 2) {
        printf("Usage: %s <generations>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int generations = atoi(argv[1]);
    if (generations <= 0) {
        printf("Error: Generations must be a positive integer.\n");
        return EXIT_FAILURE;
    }
    person *family_tree = create_family(generations);
    if (!family_tree) {
        printf("Error: Failed to create family tree.\n");
        return EXIT_FAILURE;
    }
    printf("Family tree for %d generations:\n", generations);
    print_family(family_tree, 0);
    free_family(family_tree);
    printf("Family tree freed successfully.\n");
    printf("Program completed successfully.\n");

    return EXIT_SUCCESS;
}

person* create_family(int generations) {
    if (generations <= 0) {
        return NULL;
    }

    person *p = malloc(sizeof(person));
    if (!p) {
        return NULL;
    }

    p->name = get_random_name(rand() % 2);
    p->parents[0] = create_family(generations - 1);
    p->parents[1] = create_family(generations - 1);

    if (!p->parents[0] && !p->parents[1]) {
        p->blood_type_alleles = get_random_blood_type();
    }
    else if (p->parents[0] && p->parents[1]) {
        char *allele1 = &p->parents[0]->blood_type_alleles[rand() % 2];
        char *allele2 = &p->parents[1]->blood_type_alleles[rand() % 2];

        p->blood_type_alleles = malloc(3 * sizeof(char));
        if (!p->blood_type_alleles) {
            return NULL;
        }
        p->blood_type_alleles[0] = *allele1;
        p->blood_type_alleles[1] = *allele2;
        p->blood_type_alleles[2] = '\0';
    }
    else {
        printf("Error: family structure corroded (unfull generations)\n");
    }

    return p;
}

void free_family(person *p) {
    if (!p) return;

    if (p->parents[0] && p->parents[1]) {
        free(p->blood_type_alleles);
    }

    free_family(p->parents[0]);
    free_family(p->parents[1]);

    free(p);
}

void print_family(person *p, int generation) {
    if (!p) return;

    for (int i = 0; i < generation; i++) {
        printf("      ");
    }
    printf("Name: %s, Generation: %i, Blood Type: %s\n", p->name, generation, p->blood_type_alleles);

    print_family(p->parents[0], generation + 1);
    print_family(p->parents[1], generation + 1);
}


char* get_random_blood_type() {
    return blood_types[rand() % (sizeof(blood_types) / sizeof(blood_types[0]))];
}

char* get_random_name(int male) {
    if (male) {
        return names_male[rand() % (sizeof(names_male) / sizeof(names_male[0]))];
    }
    else {
        return names_female[rand() % (sizeof(names_female) / sizeof(names_female[0]))];
    }
}