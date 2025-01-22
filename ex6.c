#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str) {
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0) {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int) strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src) {
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *) malloc(len + 1);
    if (!dest) {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt) {
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success) {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0) {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int) strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0') {
            printf("Invalid input.\n");
        } else {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type) {
    switch (type) {
        case GRASS:
            return "GRASS";
        case FIRE:
            return "FIRE";
        case WATER:
            return "WATER";
        case BUG:
            return "BUG";
        case NORMAL:
            return "NORMAL";
        case POISON:
            return "POISON";
        case ELECTRIC:
            return "ELECTRIC";
        case GROUND:
            return "GROUND";
        case FAIRY:
            return "FAIRY";
        case FIGHTING:
            return "FIGHTING";
        case PSYCHIC:
            return "PSYCHIC";
        case ROCK:
            return "ROCK";
        case GHOST:
            return "GHOST";
        case DRAGON:
            return "DRAGON";
        case ICE:
            return "ICE";
        default:
            return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput() {
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *) malloc(capacity);
    if (!input) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (size + 1 >= capacity) {
            capacity *= 2;
            char *temp = (char *) realloc(input, capacity);
            if (!temp) {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char) c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node) {
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

int BFSTreeHeight(PokemonNode *root) {
    // Base case: if the tree is empty, its height is 0
    if (root == NULL)
        return 0;

    // Recursively find the height of the left subtree
    int left = BFSTreeHeight(root->left);

    // Recursively find the height of the right subtree
    int right = BFSTreeHeight(root->right);

    // The height of the tree is the greater of the two subtree heights plus one for the root
    if (left > right)
        return left + 1;
    return right + 1;
}

void visitPokemonByLevel(PokemonNode *root, int level, VisitNodeFunc visit) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // If the current level is 1, visit the node
    if (level == 1) {
        visit(root);
    } else {
        // Recursively visit nodes at the next level down
        visitPokemonByLevel(root->left, level - 1, visit);
        visitPokemonByLevel(root->right, level - 1, visit);
    }
}

void BFSGeneric(PokemonNode *root, VisitNodeFunc visit) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Iterate through each level and visit nodes
    for (int i = 1, height = BFSTreeHeight(root); i <= height; i++) {
        visitPokemonByLevel(root, i, visit);
    }
}

void displayBFS(PokemonNode *root) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Perform BFS traversal and print each node
    BFSGeneric(root, printPokemonNode);
}


void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Visit the current node
    visit(root);
    // Recursively visit the left subtree
    preOrderGeneric(root->left, visit);
    // Recursively visit the right subtree
    preOrderGeneric(root->right, visit);
}

void preOrderTraversal(PokemonNode *root) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Perform pre-order traversal and print each node
    preOrderGeneric(root, printPokemonNode);
}

void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Recursively visit the left subtree
    inOrderGeneric(root->left, visit);
    // Visit the current node
    visit(root);
    // Recursively visit the right subtree
    inOrderGeneric(root->right, visit);
}

void inOrderTraversal(PokemonNode *root) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Perform in-order traversal and print each node
    inOrderGeneric(root, printPokemonNode);}


void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Recursively visit the left subtree
    postOrderGeneric(root->left, visit);
    // Recursively visit the right subtree
    postOrderGeneric(root->right, visit);
    // Visit the current node
    visit(root);
}

void postOrderTraversal(PokemonNode *root) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Perform post-order traversal and print each node
    postOrderGeneric(root, printPokemonNode);
}

void initNodeArray(NodeArray *na, int cap) {
    // Allocate memory for the array of PokemonNode pointers
    na->nodes = malloc(cap * sizeof(PokemonNode *));
    if (na->nodes == NULL) {
        // If memory allocation fails, print an error message and return
        printf("Memory allocation failed.\n");
        return;
    }
    // Initialize the size of the array to 0
    na->size = 0;
    // Set the capacity of the array to the given capacity
    na->capacity = cap;
}

void collectAll(PokemonNode *root, NodeArray *na) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Check if the array needs to be resized
    if (na->size >= na->capacity) {
        na->capacity *= 2;
        na->nodes = realloc(na->nodes, na->capacity * sizeof(PokemonNode *));
        if (na->nodes == NULL) {
            // If memory reallocation fails, print an error message and return
            printf("Memory reallocation failed.\n");
            return;
        }
    }

    // Add the current node to the array
    na->nodes[na->size++] = root;

    // Recursively collect nodes from the left subtree
    collectAll(root->left, na);

    // Recursively collect nodes from the right subtree
    collectAll(root->right, na);
}

// Function to compare two PokemonNode pointers by the name of the Pokemon
int compareByNameNode(const void *a, const void *b) {
    return strcmp((*(PokemonNode **)a)->data->name, (*(PokemonNode **)b)->data->name);
}

// Function to compare two OwnerNode pointers by the owner's name
int compareOwnerNames(const void *a, const void *b) {
    return strcmp((*(OwnerNode **)a)->ownerName, (*(OwnerNode **)b)->ownerName);
}

void displayAlphabetical(PokemonNode *root) {
    // Base case: if the tree is empty, return
    if (!root) return;

    // Allocate memory for NodeArray
    NodeArray *array = (NodeArray *) malloc(sizeof(NodeArray));
    if (array == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Initialize NodeArray and collect all nodes
    initNodeArray(array, BFSTreeHeight(root));
    collectAll(root, array);

    // Sort nodes alphabetically by name
    qsort(array->nodes, array->size, sizeof(PokemonNode *), compareByNameNode);

    // Print the sorted nodes
    for (int i = 0; i < array->size; i++) {
        printPokemonNode(array->nodes[i]);
    }

    // Free allocated memory
    free(array->nodes);
    free(array);
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner) {
    if (owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice) {
        case 1:
            displayBFS(owner->pokedexRoot);
            break;
        case 2:
            preOrderTraversal(owner->pokedexRoot);
            break;
        case 3:
            inOrderTraversal(owner->pokedexRoot);
            break;
        case 4:
            postOrderTraversal(owner->pokedexRoot);
            break;
        case 5:
            displayAlphabetical(owner->pokedexRoot);
            break;
        default:
            printf("Invalid choice.\n");
    }
}

PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    // Base case: if the tree is empty, return NULL
    if (root == NULL) return NULL;

    // If the current node's ID matches the search ID, return the current node
    if (root->data->id == id) return root;

    // Recursively search the left subtree
    PokemonNode *leftResult = searchPokemonBFS(root->left, id);
    if (leftResult) return leftResult;

    // Recursively search the right subtree
    return searchPokemonBFS(root->right, id);
}

void pokemonFight(OwnerNode *owner) {
    // Check if the owner empty
    if (owner == NULL) return;

    // Check if the Pokedex is empty
    if (owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }

    // Prompt user to enter the IDs of the two Pokemon to fight
    PokemonNode *firstPokemonNode = searchPokemonBFS(owner->pokedexRoot, readIntSafe("Enter ID of the first Pokemon: "));
    PokemonNode *secondPokemonNode = searchPokemonBFS(owner->pokedexRoot, readIntSafe("Enter ID of the second Pokemon: "));

    // Check if both Pokemon IDs are found
    if (firstPokemonNode == NULL || secondPokemonNode == NULL) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }

    // Retrieve the data for both Pokemon
    const PokemonData *firstPokemonData = firstPokemonNode->data;
    const PokemonData *secondPokemonData = secondPokemonNode->data;

    // Calculate the scores for both Pokemon based on their attack and HP
    double firstScore = firstPokemonData->attack * 1.5 + firstPokemonData->hp * 1.2;
    double secondScore = secondPokemonData->attack * 1.5 + secondPokemonData->hp * 1.2;

    // Print the scores and determine the winner
    printf("Pokemon 1: %s (Score = %.2f)\n", firstPokemonData->name, firstScore);
    printf("Pokemon 2: %s (Score = %.2f)\n", secondPokemonData->name, secondScore);
    if (firstScore == secondScore) {
        printf("It's a tie!\n");
        return;
    }
    printf("%s wins!\n", firstScore > secondScore ? firstPokemonData->name : secondPokemonData->name);
}

PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    // Base case: if the tree is empty, return the new node
    if (root == NULL) return newNode;

    // Recursively insert the new node into the left or right subtree
    if (newNode->data->id < root->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    } else if (newNode->data->id > root->data->id) {
        root->right = insertPokemonNode(root->right, newNode);
    }

    // Return the unchanged root node
    return root;
}

void addPokemon(OwnerNode *owner) {
    // Read the Pokemon ID to add
    int pokemonId = readIntSafe("Enter ID to add: ");

    // Check if the ID is within the valid range
    if (!(pokedex[0].id <= pokemonId && pokemonId <= pokedex[sizeof(pokedex) / sizeof(pokedex[0]) - 1].id)) {
        printf("Invalid ID.\n");
        return;
    }

    // Check if the Pokemon is already in the Pokedex
    if (searchPokemonBFS(owner->pokedexRoot, pokemonId) != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", pokemonId);
        return;
    }

    // Create a new Pokemon node and insert it into the Pokedex
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, createPokemonNode(&pokedex[pokemonId - 1]));
    printf("Pokemon %s (ID %d) added.\n", pokedex[pokemonId - 1].name, pokemonId);
}

void enterExistingPokedexMenu() {
    // Check if there are any existing Pokedexes
    if (ownerHead == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }

    // Get the number of owners and prompt user to choose a Pokedex by number
    int ownersAmount = getOwnerAmount(true);
    int pokedexIdx = readIntSafe("Choose a Pokedex by number: ");
    if (pokedexIdx < 1 || pokedexIdx > ownersAmount) {
        printf("Invalid Pokedex number.\n");
        return;
    }

    // Find the chosen Pokedex
    OwnerNode *current = ownerHead;
    for (int i = 1; i < pokedexIdx; i++) {
        current = current->next;
    }

    // Enter the chosen Pokedex menu
    printf("\nEntering %s's Pokedex...\n", current->ownerName);
    int subChoice;
    do {
        printf("\n-- %s's Pokedex Menu --\n", current->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        // Read user's choice and handle it
        subChoice = readIntSafe("Your choice: ");
        switch (subChoice) {
            case 1: addPokemon(current); break;
            case 2: displayMenu(current); break;
            case 3: freePokemon(current); break;
            case 4: pokemonFight(current); break;
            case 5: evolvePokemon(current); break;
            case 6: printf("Back to Main Menu.\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

PokemonNode *removeNodeBST(PokemonNode *root, int id) {
    if (root == NULL) {
        return NULL;
    }

    if (id < root->data->id) {
        root->left = removeNodeBST(root->left, id);
    } else if (id > root->data->id) {
        root->right = removeNodeBST(root->right, id);
    } else {
        // Node with only one child or no child
        if (root->left == NULL) {
            PokemonNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            PokemonNode *temp = root->left;
            free(root);
            return temp;
        }

        // Node with two children: Get the inorder successor (smallest in the right subtree)
        PokemonNode *temp = root->right;
        while (temp && temp->left) temp = temp->left;

        // Copy the inorder successor's content to this node and delete the inorder successor
        root->data = temp->data;
        root->right = removeNodeBST(root->right, temp->data->id);
    }
    return root;
}

void evolvePokemon(OwnerNode *owner) {
    // Check if the Pokedex is empty
    if (owner == NULL || owner->pokedexRoot == NULL) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }

    // Read the ID of the Pokemon to evolve
    int id = readIntSafe("Enter ID of Pokemon to evolve: ");
    PokemonNode *node = searchPokemonBFS(owner->pokedexRoot, id);

    // Check if the Pokemon with the given ID exists
    if (node == NULL) {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }

    // Check if the Pokemon can evolve
    if (node->data->CAN_EVOLVE == CANNOT_EVOLVE) {
        printf("Pokemon with ID %d cannot evolve.\n", id);
        return;
    }

    int envolveID = id + 1;
    // Check if the evolved Pokemon is not already in the Pokedex
    if (searchPokemonBFS(owner->pokedexRoot, envolveID) == NULL) {
        // Create a new node for the evolved Pokemon and insert it into the Pokedex
        owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, createPokemonNode(&pokedex[envolveID]));
    }

    // Print the evolution details
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", node->data->name, id, pokedex[id].name, envolveID);

    // Remove the old Pokemon
    owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, id);
}

OwnerNode *findOwnerByName(const char *name) {
    // Check if the name or ownerHead is NULL
    if (name == NULL || ownerHead == NULL) {
        return NULL;
    }

    // Start searching from the head of the circular list
    OwnerNode *tempOwner = ownerHead;
    do {
        // Compare the current owner's name with the given name
        if (strcmp(tempOwner->ownerName, name) == 0) {
            return tempOwner; // Return the owner if found
        }
        tempOwner = tempOwner->next; // Move to the next owner
    } while (tempOwner != ownerHead); // Continue until we loop back to the head

    // Return NULL if the owner is not found
    return NULL;
}

PokemonNode *createPokemonNode(const PokemonData *data) {
    // Allocate memory for the new node
    PokemonNode *pokeNode = (PokemonNode *) malloc(sizeof(PokemonNode));
    if (pokeNode == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    // Initialize the node's data and pointers
    pokeNode->data = (void *) data;
    pokeNode->left = pokeNode->right = NULL;
    return pokeNode;
}

OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    // Allocate memory for the new owner
    OwnerNode *ownerNode = (OwnerNode *) malloc(sizeof(OwnerNode));
    if (ownerName == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    // Initialize the owner's data and pointers
    ownerNode->ownerName = ownerName;
    ownerNode->pokedexRoot = starter;
    ownerNode->next = NULL;
    ownerNode->prev = NULL;
    return ownerNode;
}

void linkOwnerInCircularList(OwnerNode *newOwner) {
    // Check if the new owner is NULL
    if (newOwner == NULL) {
        printf("New owner is NULL.\n");
        return;
    }

    // If the list is empty, initialize it with the new owner
    if (ownerHead == NULL) {
        // If the list is empty, initialize it with the new owner
        ownerHead = newOwner;
        ownerHead->next = ownerHead->prev = ownerHead;
    } else {
        // Insert the new owner at the end of the list
        OwnerNode *last = ownerHead->prev;
        last->next = newOwner;
        newOwner->prev = last;
        newOwner->next = ownerHead;
        ownerHead->prev = newOwner;
    }
}

void openPokedexMenu() {
    // Prompt for the owner's name
    printf("Your name: ");
    char *name = getDynamicInput();

    // Check if the owner already exists
    if (findOwnerByName(name)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", name);
        free(name);
        return;
    }

    // Prompt for the starter Pokemon choice
    printf("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\n");
    PokemonNode *pokemon = createPokemonNode(&pokedex[(readIntSafe("Your choice: ") - 1) * 3]);

    // Create a new owner and link them in the circular list
    OwnerNode *newOwner = createOwner(name, pokemon);
    linkOwnerInCircularList(newOwner);

    // Confirm the creation of the new Pokedex
    printf("New Pokedex created for %s with starter %s.\n", newOwner->ownerName, newOwner->pokedexRoot->data->name);
}

void removeOwnerFromCircularList(OwnerNode *target) {
    // Check if the target or ownerHead is NULL
    if (target == NULL || ownerHead ==NULL) {
        return;
    }

    // If there's only one node in the list
    if (target->next == target) {
        freeOwnerNode(target);
        ownerHead = NULL;
        return;
    }

    // Update the links to remove the target node
    target->prev->next = target->next;
    target->next->prev = target->prev;
    // Update the head if the target is the head
    if (target == ownerHead) {
        ownerHead = target->next;
    }

    // Free the target node
    freeOwnerNode(target);
}

void deletePokedex() {
    // Check if there are no existing Pokedexes
    if (ownerHead == NULL) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }

    printf("\n=== Delete a Pokedex ===\n");
    int ownersAmount = getOwnerAmount(true);

    // Prompt user to choose a Pokedex by number
    int owner = readIntSafe("Choose a Pokedex to delete by number: ");
    if (owner < 1 || owner > ownersAmount) {
        printf("Invalid owner number.\n");
        return;
    }

    // Find the chosen Pokedex
    OwnerNode *ownerNode = ownerHead;
    for (int i = 1; i < owner; i++) {
        ownerNode = ownerNode->next;
    }

    // Delete the chosen Pokedex
    printf("Deleting %s's entire Pokedex...\n", ownerNode->ownerName);
    removeOwnerFromCircularList(ownerNode);
    printf("Pokedex deleted.\n");
}

PokemonNode *mergeBST(PokemonNode *firstBGTRoot, PokemonNode *secondBSTRoot) {
    // If either tree is empty, return the other tree
    if (firstBGTRoot == NULL || secondBSTRoot == NULL) {
        return firstBGTRoot ? firstBGTRoot : secondBSTRoot;
    }

    // Insert the root of the second tree into the first tree
    firstBGTRoot = insertPokemonNode(firstBGTRoot, createPokemonNode(secondBSTRoot->data));

    // Recursively merge the left and right subtrees
    firstBGTRoot = mergeBST(firstBGTRoot, secondBSTRoot->left);
    return mergeBST(firstBGTRoot, secondBSTRoot->right);
}

void mergePokedexMenu() {
    // Check if there are not enough owners to merge
    if (ownerHead== NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }

    printf("\n=== Merge Pokedexes ===\n");

    // Get the first owner's name and find their Pokedex
    printf("Enter name of first owner: ");
    char *firstOwnerName = getDynamicInput();
    OwnerNode *firstOwnerRoot = findOwnerByName(firstOwnerName);
    if (firstOwnerRoot == NULL) {
        printf("Owner '%s' not found.\n", firstOwnerName);
        free(firstOwnerName);
        return;
    }

    // Get the second owner's name and find their Pokedex
    printf("Enter name of second owner: ");
    char *secondOwnerName = getDynamicInput();
    OwnerNode *secondOwnerRoot = findOwnerByName(secondOwnerName);
    if (secondOwnerRoot == NULL) {
        printf("Owner '%s' not found.\n", secondOwnerName);
        free(firstOwnerName);
        free(secondOwnerName);
        return;
    }

    // Merge the Pokedexes and remove the second owner
    printf("Merging %s and %s...\n", firstOwnerRoot->ownerName, secondOwnerRoot->ownerName);
    firstOwnerRoot->pokedexRoot = mergeBST(firstOwnerRoot->pokedexRoot, secondOwnerRoot->pokedexRoot);
    free(firstOwnerName);
    free(secondOwnerName);
    freeOwnerNode(secondOwnerRoot);
    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n", secondOwnerName);
}

int getOwnerAmount(bool isPrintOwners) {
    int count = 0;
    OwnerNode *current = ownerHead;

    // Traverse the circular list to count owners
    do {
        count++;
        if (isPrintOwners) {
            printf("%d. %s\n", count, current->ownerName);
        }
        current = current->next;
    } while (current != ownerHead);

    return count;
}

void sortOwners() {
    int amount = getOwnerAmount(false);
    if (amount < 2) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }

    // Create an array to hold the owner nodes
    OwnerNode **ownerArray = malloc(amount * sizeof(OwnerNode *));
    if (ownerArray == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // Populate the array with owner nodes
    OwnerNode *current = ownerHead;
    for (int i = 0; i < amount; i++) {
        ownerArray[i] = current;
        current = current->next;
    }

    // Sort the array using qsort
    qsort(ownerArray, amount, sizeof(OwnerNode *), compareOwnerNames);

    // Re-link the sorted nodes into a circular list
    for (int i = 0; i < amount; i++) {
        ownerArray[i]->next = ownerArray[(i + 1) % amount];
        ownerArray[i]->prev = ownerArray[(i - 1 + amount) % amount];
    }
    ownerHead = ownerArray[0];

    // Free the temporary array
    free(ownerArray);
    printf("Owners sorted by name.\n");
}

void printOwnersCircular() {
    // Prompt for direction
    printf("Enter direction (F or B): \n");
    char *direction = getDynamicInput();
    char directionChr = direction[0];
    free(direction);

    // Validate direction input
    if (directionChr != 'F' && directionChr != 'f' && directionChr != 'B' && directionChr != 'b') {
        printf("Invalid input.\n");
        return;
    }

    // Traverse and print owners in the specified direction
    OwnerNode *temp = ownerHead;
    for (int i = 0, count = readIntSafe("How many prints? "); i < count; i++) {
        printf("[%d] %s\n", i + 1, temp->ownerName);
        temp = (directionChr == 'F' || directionChr == 'f') ? temp->next : temp->prev;
    }
}

void mainMenu() {
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice) {
            case 1:
                openPokedexMenu();
                break;
            case 2:
                enterExistingPokedexMenu();
                break;
            case 3:
                deletePokedex();
                break;
            case 4:
                mergePokedexMenu();
                break;
            case 5:
                sortOwners();
                break;
            case 6:
                printOwnersCircular();
                break;
            case 7:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 7);
}

PokemonNode *removePokemonById(PokemonNode *root, int id) {
    // If the tree is empty, return NULL
    if (root == NULL) return NULL;

    // Search for the Pokemon by ID
    PokemonNode *targetNode = searchPokemonBFS(root, id);
    if (targetNode == NULL) {
        // If the Pokemon is not found, print a message and return the original tree
        printf("Pokemon with ID %d not found.\n", id);
        return root;
    }

    // Remove the Pokemon from the tree
    return removeNodeBST(root, id);
}

void freePokemon(OwnerNode *owner) {
    // Check if the owner or their Pokedex is empty
    if (owner == NULL || owner->pokedexRoot == NULL) {
        printf("No Pokemon to release.\n");
        return;
    }

    // Prompt for the Pokemon ID to release
    int id = readIntSafe("Enter Pokemon ID to release: ");

    // Search for the Pokemon by ID
    PokemonNode *pokemonToRemove = searchPokemonBFS(owner->pokedexRoot, id);
    if (pokemonToRemove == NULL) {
        printf("No Pokemon with ID %d found.\n", id);
        return;
    }

    // Remove the Pokemon and update the Pokedex
    printf("Removing Pokemon %s (ID %d).\n", pokemonToRemove->data->name, pokemonToRemove->data->id);
    owner->pokedexRoot = removePokemonById(owner->pokedexRoot, id);
}


void freePokemonNode(PokemonNode *node) {
    // Check if the owner list is empty
    if (node == NULL) return;

    free(node->data);
    free(node);
}

void freePokemonTree(PokemonNode *root) {
    // Check if the tree is empty
    if (root == NULL) return;

    // Recursively free the left and right subtrees
    freePokemonTree(root->left);
    freePokemonTree(root->right);

    // Free the root node's data and the root node itself
    freePokemonNode(root);
}

void freeOwnerNode(OwnerNode *owner) {
    // Check if the owner is empty
    if (owner == NULL) return;

    free(owner->ownerName);
    freePokemonTree(owner->pokedexRoot);
    free(owner);
}

void freeAllOwners(void) {
    // Check if the owner list is empty
    if (ownerHead == NULL ) return;

    // Initialize the current pointer to the head of the list
    OwnerNode *current = ownerHead;
    do {
        // Store the next owner node
        OwnerNode *temp = current->next;
        // Free the current owner node and its Pokedex
        freeOwnerNode(current);
        // Move to the next owner node
        current = temp;
    } while (current != ownerHead); // Continue until we loop back to the head

    // Set the head to NULL after freeing all nodes
    ownerHead = NULL;
}

int main() {
    mainMenu();
    freeAllOwners();
    return 0;
}
