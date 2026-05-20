
/*  Library Management System
 *  Demonstrates: Inheritance, Function Hiding, Composition,
 *                Friend Functions, and the 'this' Pointer
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Forward Declarations

class Library;
class LibraryMember;


//BASE CLASS: LibraryResource

class LibraryResource {
protected:
    int    resourceID;
    string title;
    string author;
    bool   isAvailable;

public:
    // Constructor
    LibraryResource(int id, const string& title, const string& author, bool available = true)
        : resourceID(id), title(title), author(author), isAvailable(available) {
    }

    // Getters 
    int    getResourceID()  const { return resourceID; }
    string getTitle()       const { return title; }
    string getAuthor()      const { return author; }
    bool   getIsAvailable() const { return isAvailable; }

    // Setters 
    // 'this' POINTER USAGE #1:
    // Used in setTitle() to differentiate between the parameter 'title'
    // and the member variable 'this->title' since both share the same name.
    void setTitle(const string& title) {
        this->title = title;   // 'this->title' is the member; 'title' is the parameter
    }
    //same Usage of this->pointer, this->author is the object member, 'author' is parameter
    void setAuthor(const string& author) {
        this->author = author;
    }

    void setIsAvailable(bool available) {
        this->isAvailable = available;
    }

    // Virtual Methods
    virtual void displayDetails() const {
        cout << "  ID       : " << resourceID << "\n"
            << "  Title    : " << title << "\n"
            << "  Author   : " << author << "\n"
            << "  Available: " << (isAvailable ? "Yes" : "No") << "\n";
    }

    // Base version (will be hidden by derived classes)
    virtual double calculateLateFee(int daysLate) const {
        return daysLate * 1.0;   // Default fallback
    }

    virtual ~LibraryResource() {}

    // adminView needs direct access to protected members to demonstrate friend privilege
    friend void adminView(const Library& lib, const vector<LibraryMember>& members);
};

//  DERIVED CLASS: Book
class Book : public LibraryResource {
private:
    string ISBN;
    int    pageCount;

public:
    Book(int id, const string& title, const string& author,
        const string& isbn, int pages)
        : LibraryResource(id, title, author), ISBN(isbn), pageCount(pages) {
    }

    // Getters / Setters
    string getISBN()      const { return ISBN; }
    int    getPageCount() const { return pageCount; }
    void   setISBN(const string& isbn) { this->ISBN = isbn; }
    void   setPageCount(int pages) { this->pageCount = pages; }

    // Function Hiding: hides LibraryResource::calculateLateFee()
    double calculateLateFee(int daysLate) const override {
        return daysLate * 5.0;   // Rs. 5 per day
    }

    void displayDetails() const override {
        cout << "  [BOOK]\n";
        LibraryResource::displayDetails();
        cout << "  ISBN     : " << ISBN << "\n"
            << "  Pages    : " << pageCount << "\n";
    }
};


//  DERIVED CLASS: Magazine
class Magazine : public LibraryResource {
private:
    int issueNumber;

public:
    Magazine(int id, const string& title, const string& author, int issue)
        : LibraryResource(id, title, author), issueNumber(issue) {
    }

    int  getIssueNumber() const { return issueNumber; }
    void setIssueNumber(int issue) { this->issueNumber = issue; }

    // Function Hiding: hides LibraryResource::calculateLateFee()
    double calculateLateFee(int daysLate) const override {
        return daysLate * 3.0;   // Rs. 3 per day
    }

    void displayDetails() const override {
        cout << "  [MAGAZINE]\n";
        LibraryResource::displayDetails();
        cout << "  Issue No.: " << issueNumber << "\n";
    }
};


//  DERIVED CLASS: DVD
class DVD : public LibraryResource {
private:
    int duration;   // in minutes

public:
    DVD(int id, const string& title, const string& author, int dur)
        : LibraryResource(id, title, author), duration(dur) {
    }

    int  getDuration() const { return duration; }
    void setDuration(int dur) { this->duration = dur; }

    // Function Hiding: hides LibraryResource::calculateLateFee()
    double calculateLateFee(int daysLate) const override {
        return daysLate * 10.0;   // Rs. 10 per day
    }

    void displayDetails() const override {
        cout << "  [DVD]\n";
        LibraryResource::displayDetails();
        cout << "  Duration : " << duration << " min\n";
    }
};

//  CLASS: LibraryMember   (COMPOSITION with LibraryResource)
class LibraryMember {
private:
    int    memberID;
    string name;

    // COMPOSITION=> LibraryMember stores LibraryResource objects BY VALUE in a vector.
    // When a LibraryMember is destroyed, this vector is destroyed too,
    // taking all borrowed items with it (strong ownership).
    vector<LibraryResource> borrowedItems;

public:
    LibraryMember(int id, const string& name)
        : memberID(id), name(name) {
    }

    // Getters 
    int    getMemberID() const { return memberID; }
    string getName()     const { return name; }

    // 'this' POINTER USAGE #2:
    // Returns a reference to the current object so calls can be chained,
    // e.g.  member.setName("Ali").displayBorrowedItems();
    LibraryMember& setName(const string& name) {
        this->name = name;   // differentiate member vs parameter
        return *this;        // return current object for method chaining
    }

    // Borrow a resource 
    void borrowResource(LibraryResource res) {
        if (!res.getIsAvailable()) {
            cout << "  [Error] Resource \"" << res.getTitle()
                << "\" is already borrowed.\n";
            return;
        }
        res.setIsAvailable(false);
        borrowedItems.push_back(res);
        cout << "  Resource \"" << res.getTitle()
            << "\" borrowed successfully by " << name << ".\n";
    }

    // Return a resource 
    void returnResource(int resourceID) {
        for (int i = 0; i < (int)borrowedItems.size(); ++i) {
            if (borrowedItems[i].getResourceID() == resourceID) {
                borrowedItems[i].setIsAvailable(true);
                cout << "  Resource \"" << borrowedItems[i].getTitle()
                    << "\" returned successfully.\n";
                borrowedItems.erase(borrowedItems.begin() + i);
                return;
            }
        }
        cout << "  [Error] Resource ID " << resourceID
            << " not found in " << name << "'s borrowed list.\n";
    }

    // Displaying borrowed items 
    void displayBorrowedItems() const {
        if (borrowedItems.empty()) {
            cout << "  " << name << " has no borrowed items.\n";
            return;
        }
        cout << "  Borrowed items for " << name << " (ID: " << memberID << "):\n";
        for (const auto& item : borrowedItems) {
           
            item.displayDetails();
        }
    }

    // Calculate total late fee 
    double calculateTotalLateFee(int daysLate) const {
        double total = 0.0;
        for (const auto& item : borrowedItems) {
            total += item.calculateLateFee(daysLate);
        }
        return total;
    }

    // Friend function declaration (defined after Library class)
    friend void adminView(const Library& lib, const vector<LibraryMember>& members);
};


//  CLASS: Library  (holds all resources)
class Library {
private:
    vector<LibraryResource> resources;

public:
    // Add any LibraryResource (by value)  
    void addResource(const LibraryResource& res) {
        resources.push_back(res);
        cout << "  Resource \"" << res.getTitle() << "\" added to library.\n";
    }

    // Find resource index by ID (-1 if not found)
    int findResourceIndex(int id) const {
        for (int i = 0; i < (int)resources.size(); ++i)
            if (resources[i].getResourceID() == id) return i;
        return -1;
    }

    // Return a copy of the resource (for borrowing)
    LibraryResource getResource(int index) const {
        return resources[index];
    }

    // Update availability in the master list when a resource is borrowed
    void markUnavailable(int id) {
        int idx = findResourceIndex(id);
        if (idx != -1) resources[idx].setIsAvailable(false);
    }

    // Update availability in the master list when a resource is returned
    void markAvailable(int id) {
        int idx = findResourceIndex(id);
        if (idx != -1) resources[idx].setIsAvailable(true);
    }

    void displayAllResources() const {
        if (resources.empty()) {
            cout << "  No resources in the library.\n";
            return;
        }
        for (const auto& r : resources) {
            
            r.displayDetails();
        }
    }

	// Friend function declaration. Defined after LibraryMember class below
    friend void adminView(const Library& lib, const vector<LibraryMember>& members);
};


//  FRIEND FUNCTION: adminView
//  Declared as friend of both Library and LibraryMember.
//  Has unrestricted access to their private members.

void adminView(const Library& lib, const vector<LibraryMember>& members) {
   
    cout << " ** ADMIN VIEW **\n";
   
    cout << "\n ALL LIBRARY RESOURCES \n";
    if (lib.resources.empty()) {
        cout << "  (no resources)\n";
    }
    else {
        for (const auto& r : lib.resources) {
           
            // shown direct private-member access via friend privilege
            cout << "  ID       : " << r.resourceID << "\n"
                << "  Title    : " << r.title << "\n"
                << "  Author   : " << r.author << "\n"
                << "  Available: " << (r.isAvailable ? "Yes" : "No") << "\n";
        }
    }

    cout << "\n ALL REGISTERED MEMBERS \n";
    if (members.empty()) {
        cout << "  (no members)\n";
    }

    else {
        for (int i = 0; i < members.size(); ++i) {
            const LibraryMember& m = members[i];
           
            // Again direct private-member access here, via friend privilege
            cout << "  Member ID : " << m.memberID << "\n"
                << "  Name      : " << m.name << "\n";
            if (m.borrowedItems.empty()) {
                cout << "  Borrowed  : (none)\n";
            }
            else {
                cout << "  Borrowed Items:\n";
                for (const auto& item : m.borrowedItems) {
                    cout << "    - [ID " << item.resourceID << "] "
                        << item.title << "\n";
                }
            }
        }
    }
   
}


//  HELPER: find member index
int findMemberIndex(const vector<LibraryMember>& members, int memberID) {
    for (int i = 0; i < (int)members.size(); ++i)
        if (members[i].getMemberID() == memberID) return i;
    return -1;
}

//  MENU-DRIVEN MAIN
int main() {
    Library              library;
    vector<LibraryMember> members;

    int choice = 0;

   
    cout << "  Welcome to the Library Management System\n";
	cout << "................................................\n";

    while (true) {
        cout << "\ MAIN MENU \n"
            << " 1. Add a new Resource\n"
            << " 2. Register a new Member\n"
            << " 3. Borrow a Resource\n"
            << " 4. Return a Resource\n"
            << " 5. Display all Resources\n"
            << " 6. Display Member's Borrowed Items\n"
            << " 7. Calculate Late Fee\n"
            << " 8. Admin View\n"
            << " 9. Exit\n"
            << "..............\n"
            << "Enter choice: ";
        cin >> choice;

        // 1) Adding Resource 
        if (choice == 1) {
            cout << "\nResource Type:\n"
                << "  1. Book\n  2. Magazine\n  3. DVD\n"
                << "Enter type: ";
            int type; cin >> type;

            int    id;
            string title, author;
            cout << "Resource ID   : "; cin >> id;
            cin.ignore();
            cout << "Title         : "; getline(cin, title);
            cout << "Author/Creator: "; getline(cin, author);

          

            if (type == 1) {
                string isbn; int pages;
                cout << "ISBN          : "; cin >> isbn;
                cout << "Page Count    : "; cin >> pages;
                Book b(id, title, author, isbn, pages);
                library.addResource(b);
            }
            else if (type == 2) {
                int issue;
                cout << "Issue Number  : "; cin >> issue;
                Magazine m(id, title, author, issue);
                library.addResource(m);
            }
            else if (type == 3) {
                int dur;
                cout << "Duration (min): "; cin >> dur;
                DVD d(id, title, author, dur);
                library.addResource(d);
            }
            else {
                cout << " Invalid resource type.\n";
            }
            
        }
        // 2) Register Member
        else if (choice == 2) {
            int    id;
            string name;
            cout << "\nMember ID  : "; cin >> id;
            cin.ignore();
            cout << "Member Name: "; getline(cin, name);

            members.push_back(LibraryMember(id, name));
            cout << "  Member \"" << name << "\" registered successfully.\n";

           
        }
        //3) Borrow Resource 
        else if (choice == 3) {
            int memberID, resourceID;
            cout << "\nMember ID  : "; cin >> memberID;
            cout << "Resource ID: "; cin >> resourceID;

            int mIdx = findMemberIndex(members, memberID);
            int rIdx = library.findResourceIndex(resourceID);

            if (mIdx == -1) {
                cout << "  [Error] Member not found.\n";
            }
            else if (rIdx == -1) {
                cout << "  [Error] Resource not found.\n";
            }
            else if (!library.getResource(rIdx).getIsAvailable()) {
                cout << "  [Error] Resource is already borrowed.\n";
            }
            else {
                members[mIdx].borrowResource(library.getResource(rIdx));
                library.markUnavailable(resourceID);
            }    
        }
        // 4) Return Resource 
        else if (choice == 4) {
            int memberID, resourceID;
            cout << "\nMember ID  : "; cin >> memberID;
            cout << "Resource ID: "; cin >> resourceID;

            int mIdx = findMemberIndex(members, memberID);
            if (mIdx == -1) {
                cout << "  [Error] Member not found.\n";
            }
            else {
                members[mIdx].returnResource(resourceID);
                library.markAvailable(resourceID);
            }      
        }

        // 5) Display all Resources
        else if (choice == 5) {
            cout << "\n--- ALL RESOURCES ---\n";
            library.displayAllResources();     
        }

        // 6) Display Member's Borrowed Items
        else if (choice == 6) {
            int memberID;
            cout << "\nMember ID: "; cin >> memberID;
            int mIdx = findMemberIndex(members, memberID);
            if (mIdx == -1) {
                cout << "  [Error] Member not found.\n";
            }
            else {
                members[mIdx].displayBorrowedItems();
            }
           
        }
        // 7) Calculate Late Fee 
        else if (choice == 7) {
            int memberID, daysLate;
            cout << "\nMember ID : "; cin >> memberID;
            cout << "Days Late : "; cin >> daysLate;

            int mIdx = findMemberIndex(members, memberID);
            if (mIdx == -1) {
                cout << "  [Error] Member not found.\n";
            }
            else if (daysLate < 0) {
                cout << "  [Error] Days late cannot be negative.\n";
            }
            else {
                double fee = members[mIdx].calculateTotalLateFee(daysLate);
                cout << "  Total Late Fee for "
                    << members[mIdx].getName()
                    << ": Rs. " << fee << "\n";
            }            
        }

        //  8) Admin View 
        else if (choice == 8) {
            adminView(library, members);
    
        }
        // 9) Exit 
        else if (choice == 9) {
            cout << "\n Goodbye!\n";
            break;

        }
        else {
            cout << "Invalid menu option. Please choose 1-9.\n";
        }
    }

    return 0;
}