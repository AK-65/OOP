

#include <iostream>
#include <fstream>
using namespace std;

const int MAX_BOOKS = 5;
const char* FILENAME = "my_library_records.txt";

struct Book {
    char title[50];
    char isbn[20];
    char author[50];
    char publisher[50];
    int copies;
    int edition;
};

Book library[MAX_BOOKS];
int bookCount = 0;

// VALIDATION FUNCTIONS 
bool isValidISBN(const char isbn[]) 
{
    for (int i = 0; isbn[i] != '\0'; i++) {
        if (!((isbn[i] >= '0' && isbn[i] <= '9') || isbn[i] == '-')) {
            return false;
        }
    }
    return true;
}

bool isValidNumber(const char input[])
{
    if (input[0] == '\0') return false;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] < '0' || input[i] > '9') return false;
    }
    return true;
}

//  FILE HANDLING Functions
void saveBooksToFile()
{
	ofstream file(FILENAME);  //from declared constant FILENAME = "my_library_records.txt"
    if (!file) {
        cout << "Error saving to file!\n";
        return;
    }
    file << bookCount << "\n";
    for (int i = 0; i < bookCount; i++) {
        file << library[i].title << "\n";
        file << library[i].isbn << "\n";
        file << library[i].author << "\n";
        file << library[i].publisher << "\n";
        file << library[i].copies << "\n";
        file << library[i].edition << "\n";
    }
    file.close();
    cout << " Data saved to file.\n";
}

void loadBooksFromFile() 
{
    ifstream file(FILENAME);
    if (!file) {
        cout << "No existing data file. Starting fresh.\n";
        return;
    }
    file >> bookCount;
    file.ignore();
    for (int i = 0; i < bookCount && i < MAX_BOOKS; i++) {
        file.getline(library[i].title, 50);
        file.getline(library[i].isbn, 20);
        file.getline(library[i].author, 50);
        file.getline(library[i].publisher, 50);
        file >> library[i].copies >> library[i].edition;
        file.ignore();
    }
    file.close();
    cout << " Loaded " << bookCount << " books from file.\n";
}

// Library CORE FUNCTIONS 
int findBookByISBN(const char target[])
{
    for (int i = 0; i < bookCount; i++) {
        bool match = true;
        for (int j = 0; j < 20; j++) {
            if (library[i].isbn[j] != target[j]) {
                match = false;
                break;
            }
            if (library[i].isbn[j] == '\0') break;
        }
        if (match) return i;
    }
    return -1;
}

void addBook() 
{
    if (bookCount >= MAX_BOOKS) {
        cout << " Library full!\n";
        return;
    }

    cout << "Enter Title: ";
    cin.getline(library[bookCount].title, 50);

    char isbn[20];
    do {
        cout << "Enter ISBN (0-9, - only): ";
        cin.getline(isbn, 20);
    }
    while (!isValidISBN(isbn));

    for (int i = 0; i < 20 && isbn[i]; i++) {
        library[bookCount].isbn[i] = isbn[i];
    }
    library[bookCount].isbn[19] = '\0';

    cout << "Enter Author: ";
    cin.getline(library[bookCount].author, 50);
    cout << "Enter Publisher: ";
    cin.getline(library[bookCount].publisher, 50);
    cout << "Enter Copies: ";
    cin >> library[bookCount].copies;
    cin.ignore();

    char editionStr[10];
    do {
        cout << "Enter Edition: ";
        cin.getline(editionStr, 10);
    } while (!isValidNumber(editionStr));
    library[bookCount].edition = 0;
    for (int i = 0; editionStr[i]; i++) {
        library[bookCount].edition = library[bookCount].edition * 10 + (editionStr[i] - '0');
    }

    bookCount++;
    saveBooksToFile(); // AUTO-SAVE after add
    cout << " Book added & saved.\n";
}

void deleteBook()
{
    char isbn[20];
    cout << "Enter ISBN to delete: ";
    cin.getline(isbn, 20);
    int index = findBookByISBN(isbn);

    if (index == -1) {
        cout << "Book not found.\n";
        return;
    }

    for (int i = index; i < bookCount - 1; i++) {
        library[i] = library[i + 1];
    }
    bookCount--;
    saveBooksToFile(); // AUTO-SAVE after delete
    cout << " Book deleted & saved.\n";
}

void modifyBook() 
{
    char isbn[20];
    cout << "Enter ISBN to modify: ";
    cin.getline(isbn, 20);
    int index = findBookByISBN(isbn);

    if (index == -1) {
        cout << " Book not found.\n";
        return;
    }

    cout << "Current: " << library[index].title << "\nNew Title: ";
    char temp[50];
    cin.getline(temp, 50);
    if (temp[0]) for (int i = 0; i < 50 && temp[i]; i++) library[index].title[i] = temp[i];

    cout << "Current Author: " << library[index].author << "\nNew Author: ";
    cin.getline(temp, 50);
    if (temp[0]) 
        for (int i = 0; i < 50 && temp[i]; i++) library[index].author[i] = temp[i];

    cout << "Current Publisher: " << library[index].publisher << "\nNew Publisher: ";
    cin.getline(temp, 50);
    if (temp[0]) for (int i = 0; i < 50 && temp[i]; i++) library[index].publisher[i] = temp[i];

    cout << "Current Copies: " << library[index].copies << "\nNew Copies: ";
    char numStr[10];
    cin.getline(numStr, 10);
    if (isValidNumber(numStr)) {
        library[index].copies = 0;
        for (int i = 0; numStr[i]; i++) {
            library[index].copies = library[index].copies * 10 + (numStr[i] - '0');
        }
    }

    saveBooksToFile(); // AUTO-SAVE after modifying
    cout << "Book updated & saved.\n";
}

void searchBookByISBN()
{
    char isbn[20];
    cout << "Enter ISBN: ";
    cin.getline(isbn, 20);
    int index = findBookByISBN(isbn);

    if (index == -1) {
        cout << " Not found.\n";
        return;
    }
    cout << "\n FOUND:\n";
    cout << "Title: " << library[index].title << "\n";
    cout << "ISBN: " << library[index].isbn << "\n";
    cout << "Author: " << library[index].author << "\n";
    cout << "Publisher: " << library[index].publisher << "\n";
    cout << "Copies: " << library[index].copies << "\n";
    cout << "Edition: " << library[index].edition << "\n";
}

void searchBookByTitle()
{
    char title[50];
    cout << "Enter Title: ";
    cin.getline(title, 50);
    bool found = false;

    for (int i = 0; i < bookCount; i++) {
        bool match = true;
        for (int j = 0; j < 50; j++) {
            if (library[i].title[j] != title[j]) {
                match = false;
                break;
            }
            if (library[i].title[j] == '\0') break;
        }
        if (match) {
            cout << "\n MATCH:\n";
            cout << "ISBN: " << library[i].isbn << " | Copies: " << library[i].copies << "\n";
            found = true;
        }
    }
    if (!found) cout << " No matches found.\n";
}

void sortBooksByTitle()
{
   
    for (int i = 0; i < bookCount - 1; i++) {
        for (int j = 0; j < bookCount - i - 1; j++) {
        
            bool swapNeeded = false;
            for (int k = 0; k < 50; k++) {
                if (library[j].title[k] == '\0' && library[j + 1].title[k] != '\0') {
                    swapNeeded = true;
                    break;
                }
                if (library[j].title[k] > library[j + 1].title[k]) {
                    swapNeeded = true;
                    break;
                }
                if (library[j].title[k] == '\0') break;
            }
            if (swapNeeded) {
                Book temp = library[j];
                library[j] = library[j + 1];
                library[j + 1] = temp;
            }
        }
    }
    saveBooksToFile(); // AUTO-SAVE after sorting
    cout << " Books sorted by title & saved.\n";
}


// Book Modification Functions
void borrowBook()
{
    char isbn[20];
    cout << "Enter ISBN to borrow: ";
    cin.getline(isbn, 20);
    int index = findBookByISBN(isbn);

    if (index == -1) {
        cout << " Book not found.\n";
        return;
    }
    if (library[index].copies <= 0) {
        cout << " No copies available.\n";
        return;
    }

    library[index].copies--;
    saveBooksToFile();
    cout << " Borrowed. Copies left: " << library[index].copies << "\n";
}

void returnBook()
{
    char isbn[20];
    cout << "Enter ISBN to return: ";
    cin.getline(isbn, 20);
    int index = findBookByISBN(isbn);

    if (index == -1) {
        cout << " Book not found.\n";
        return;
    }

    library[index].copies++;
    saveBooksToFile();
    cout << " Returned. Copies now: " << library[index].copies << "\n";
}

void displayBooks()
{
    if (bookCount == 0) {
        cout << " Library empty.\n";
        return;
    }

    cout << "\n LIBRARY (" << bookCount << " Books):\n";
    cout << "----------------------\n";
    for (int i = 0; i < bookCount; i++) {
        cout << i + 1 << ". " << library[i].title << " | ";
        cout << library[i].isbn << " | Copies: " << library[i].copies << " | Book edition: " << library[i].edition << " | Book publisher: " << library[i].publisher << "\n";
    }
}

int main()
{
    loadBooksFromFile(); // Load present books in library on startup

    while (true)
    {
        cout << "\n=== LIBRARY MANAGEMENT SYSTEM ===\n";
        cout << "Books: " << bookCount << "/" << MAX_BOOKS << "\n";
        cout << "1. Add Book\n2. Delete\n3. Modify\n4. Search ISBN\n";
        cout << "5. Search Title\n6. Sort\n7. Borrow\n8. Return\n";
        cout << "9. Display All\n0. Exit (Auto-saves)\nChoice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1: addBook(); break;
        case 2: deleteBook(); break;
        case 3: modifyBook(); break;
        case 4: searchBookByISBN(); break;
        case 5: searchBookByTitle(); break;
        case 6: sortBooksByTitle(); break;
        case 7: borrowBook(); break;
        case 8: returnBook(); break;
        case 9: displayBooks(); break;
        case 0: saveBooksToFile();
            cout << " Goodbye!\n";
            return 0;
        default: cout << " Invalid!\n";
        }
    }
}
