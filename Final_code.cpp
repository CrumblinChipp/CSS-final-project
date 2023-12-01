//THE LIBRARY SYSTEM FOR COMPUTER PROGRAMMING PROJECT//
//HEADER//
#include <iostream> //FOR READING CODE
#include <string> //TO INCLUDE STRING
#include <fstream> //FOR READING AND CREATING FILES
#include <ctime> //FOR THE DUE DATE FUNCTION
#include <chrono> //FOR DUE DATE FUNCTION

//FOR CLEARING THE CONSOLE SCREEN
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

//FOR CLEARING THE CONSOLE SCREEN
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

//FOR CHECKING THE ACCOUNT EXISTENCE AND TO CHECK THE USERNAME AND PASSWORD CORRECTNESS
bool isloggedin(string& username) {
    string password, un, pw;

    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    ifstream read("c:\\" + username + ".txt");
    getline(read, un);
    getline(read, pw);

    if (un == username && pw == password) {
        return true;
    } else {
        return false;
    }
}


//FOR THE SEARCH FUNCTION
bool searchbook() {
    string search_input;
    cout << "Enter book's title or author: ";
    cin.ignore();  // Ignore any newline characters in the buffer
    getline(cin, search_input); //To take the entire line of entry by the user

    ifstream read("c:\\" + search_input + ".txt"); //to search for the file of the users account


    //if function if the username and password match an existing accoutn record
    if (read.is_open()) {
        string title, author;
        getline(read, title);
        getline(read, author);

        cout << "Book found!" << endl;
        cout << "Title: " << title << endl;
        cout << "Author: " << author << endl;

        read.close();
        return true;
    } else {
        cout << "Book not found!" << endl;
        return false;
    }
}


//TO CHECK IF THE BOOK BEING BORROWED IS AVAILABLE OR NOT
bool isBookBorrowed(const string& search_input) {
    ifstream bookFile("c:\\" + search_input + ".txt");
    string line;

    while (getline(bookFile, line)) {
        if (line.find("Borrowed by:") != string::npos) {
            // Book is already borrowed
            return true;
        }
    }

    return false;
}

string calculateDueDate(); //declaration for the book due date

//FUNCTION TO BORROW A BOOK
bool borrowbook(const string& username) {
    string search_input;
    cout << "Enter book's title to borrow: ";
    cin.ignore();  // Ignore any newline characters in the buffer
    getline(cin, search_input);

    if (isBookBorrowed(search_input)) {
        cout << "Sorry, this book is already borrowed by someone else!" << endl;
        return false;
    }

    ifstream read("c:\\" + search_input + ".txt"); //search for the file of the book being borrowed

    if (read.is_open()) {
        string title, author;
        getline(read, title);
        getline(read, author);
        read.close();

        // Check if the book is already borrowed by the user
        ifstream userBorrowedBooksFile("c:\\" + username + "_borrowed_books.txt");
        string borrowedTitle;
        bool alreadyBorrowed = false;

        while (getline(userBorrowedBooksFile, borrowedTitle)) {
            if (borrowedTitle == search_input) {
                cout << "Sorry, you have already borrowed this book!" << endl;
                alreadyBorrowed = true;
                break;
            }
        }

        userBorrowedBooksFile.close();

        if (!alreadyBorrowed) {
            // Update the user's list of borrowed books with due date
            ofstream userBorrowedBooksFile("c:\\" + username + "_borrowed_books.txt", ios::app);
            userBorrowedBooksFile << search_input << " (Due by: " << calculateDueDate() << ")" << endl;
            userBorrowedBooksFile.close();

            // Update the book file to indicate it's borrowed by the user
            ofstream book_file("c:\\" + search_input + ".txt", ios::app);  // Open in append mode
            book_file << "Borrowed by: " << username << endl;
            book_file.close();

            cout << "Book successfully borrowed! Due by: " << calculateDueDate() << endl;
        }

        return true;
    } else {
        cout << "Book not available for borrowing!" << endl;
        return false;
    }
}

//FUNCTION TO REGISTER BOOKS BEING DONATED.
bool registerbook() {
    string book_title, book_author;

    cout << "Enter book's title: ";
    cin.ignore();  // Ignore any newline characters in the buffer
    getline(cin, book_title);

    cout << "Enter book's author: ";
    getline(cin, book_author);

    ofstream book_file("c:\\" + book_title + ".txt");
    book_file << book_title << endl << book_author;
    book_file.close();

    cout << "Book successfully registered!" << endl;

    return true;
}

//FUNCTION FOR BOOKS BEING RETURNED
bool returnbook(const string& username) {
    string return_input;
    cout << "Enter book's title to return: ";
    cin.ignore();  // Ignore any newline characters in the buffer
    getline(cin, return_input);

    // Check if the book is borrowed by the user
    ifstream userBorrowedBooksFile("c:\\" + username + "_borrowed_books.txt");
    string borrowedTitle;
    bool found = false;

    // Create a temporary file to store the updated list of borrowed books
    ofstream tempFile("c:\\" + username + "_temp.txt");

    while (getline(userBorrowedBooksFile, borrowedTitle)) {
        if (borrowedTitle.find(return_input) != string::npos) {
            // Book is found, indicating it was borrowed by the user
            found = true;
            cout << "Book successfully returned!" << endl;
        } else {
            // Write other books to the temporary file
            tempFile << borrowedTitle << endl;
        }
    }

    userBorrowedBooksFile.close();
    tempFile.close();

    // Remove the original file and rename the temporary file
    remove(("c:\\" + username + "_borrowed_books.txt").c_str());
    rename(("c:\\" + username + "_temp.txt").c_str(), ("c:\\" + username + "_borrowed_books.txt").c_str());

    if (!found) {
        cout << "You haven't borrowed this book!" << endl;
        return false;
    }

    // Update the book file to indicate it's available for borrowing
    ifstream bookFile("c:\\" + return_input + ".txt");
    string bookContent;
    string newContent;

    while (getline(bookFile, bookContent)) {
        if (bookContent.find("Borrowed by:") != string::npos) {
            // Skip the line indicating the book is borrowed
        } else {
            newContent += bookContent + "\n";
        }
    }

    bookFile.close();

    ofstream updatedBookFile("c:\\" + return_input + ".txt");
    updatedBookFile << newContent;
    updatedBookFile.close();

    return true;
}

string calculateDueDate() {
    // Calculate due date as two weeks from the current date
    auto now = chrono::system_clock::now();
    auto dueDate = now + chrono::hours(24 * 14);  // 14 days

    time_t dueTime = chrono::system_clock::to_time_t(dueDate);
    return ctime(&dueTime);  // Convert time to string
}

//DISPLAYS THE BORROWED BOOKS BY THE ACCOUNT
void displayUserBorrowedBooks(const string& username) {
    cout << "YOUR BORROWED BOOKS:" << endl;

    ifstream userBorrowedBooksFile("c:\\" + username + "_borrowed_books.txt");
    string borrowedTitle;

    while (getline(userBorrowedBooksFile, borrowedTitle)) {
        cout << borrowedTitle << endl;
    }

    userBorrowedBooksFile.close();
}

int main() {
    int account_choice, menu_choice;
    string username;


    //ACCOUNT LOG IN OR REGISTER
    cout << "~~WELCOME TO THE LIBRARY SYSTEM~~" << endl;
    cout << "[1] Register" << endl;
    cout << "[2] Log in" << endl;
    cout << "Enter your choice: ";
    cin >> account_choice;

    if (account_choice == 1) {
        string username, password;

        cout << "Enter desired username : ";
        cin >> username;
        cout << "Enter desired password : ";
        cin >> password;

        ofstream user_file("c:\\" + username + ".txt");
        user_file << username << endl << password;
        user_file.close();
        cout << "Account successfully registered!" << endl;
    } else if (account_choice == 2) {
        bool status = isloggedin(username);

        if (!status) {
            cout << "False login" << endl;
            system("PAUSE");
            return 0;
        } else {
            cout << "Successfully logged in" << endl;
            system("PAUSE");
            cout << "Press Enter to continue...";
            cin.ignore();
            cin.get();

            clearScreen();
        }
    } else {
        cout << "Invalid input, try again" << endl;
        return 0;
    }


    //MAIN SYSTEM MENU
    do {
        cout << "WELCOME " << username << "!" << endl;
        cout << "\n" << endl;
        displayUserBorrowedBooks(username);
        cout << "\n\n" << endl;
        cout << "[1] Browse" << endl;
        cout << "[2] Search" << endl;
        cout << "[3] Borrow" << endl;
        cout << "[4] Return" << endl;
        cout << "[5] Donate" << endl;
        cout << "[6] Exit" << endl;
        cout << "Enter your choice: ";
        cin >> menu_choice;

        switch (menu_choice) {
            case 1:
                cout << "This will be the browsing choice" << endl;
                break;
            case 2:
                searchbook();
                break;
            case 3:
                borrowbook(username);
                displayUserBorrowedBooks(username); // Display user's borrowed books
                break;
            case 4:
                returnbook(username);
                displayUserBorrowedBooks(username);
                break;
            case 5:
                registerbook();
                break;
            case 6:
                continue;
            default:
                cout << "Invalid input, please try again" << endl;
                break;
        }

        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();

        clearScreen();

    } while (!(menu_choice == 6));

    cout << "~EXITING THE LIBRARY SYSTEM~" << endl;
    cout<< "Thank you for your using our system~"<<endl

    return 0;
}
