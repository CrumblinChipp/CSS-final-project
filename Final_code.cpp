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
        if (borrowedTitle == return_input) {
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
        if (bookContent.find("Borrowed by: " + username) != string::npos) {
            // Skip the line indicating the book is borrowed by the user
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
