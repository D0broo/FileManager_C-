#include "FileManager.h"

void PrintCommands()
{
    cout << "Available commands:\n";
    cout << "ls - List contents of current directory\n";
    cout << "cd <directory_name> - Change directory\n";
    cout << "cd .. - Exit directory\n";
    cout << "pwd - Show current directory path\n";
    cout << "mkfile <file_name> - Create a new file\n";
    cout << "mkdir <directory_name> - Create a new directory\n";
    cout << "rm <file_or_directory_name> - Remove a file or directory\n";
    cout << "mv <source> <destination> - Move a file or directory\n";
    cout << "cp <source> <destination> - Copy a file or directory\n";
    cout << "rn <old_name> <new_name> - Rename a file or directory\n";
    cout << "sr <pattern> - Search files matching pattern\n";
    cout << "exit - Exit the program\n";
}

string Trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector<string> Split(const string& str) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, ' ')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int main() {
    FileManager fileManager;
    string command;

    PrintCommands();
    cout << endl;

    while (true) {
        cout << "> ";
        getline(cin, command);
        cout << endl;

        command = Trim(command);
        vector<string> parts = Split(command);

        if (parts.empty()) {
            continue;
        }

        string cmd = parts[0];

        if (cmd == "exit") {
            break;
        }
        else if (cmd == "ls") {
            fileManager.ListDirectory();
            cout << endl;
        }
        else if (cmd == "cd") {
            if (parts.size() > 1) {
                fileManager.ChangeDirectory(parts[1]);
                cout << endl;
            }
            else {
                cout << "Error: Specify a directory to change to\n";
            }
        }
        else if (cmd == "pwd") {
            fileManager.ShowCurrentPath();
            cout << endl;
        }
        else if (cmd == "mkfile") {
            if (parts.size() > 1) {
                fileManager.CreateFile(parts[1]);
                cout << endl;
            }
            else {
                cout << "Error: Specify a file name to create\n";
            }
        }
        else if (cmd == "mkdir") {
            if (parts.size() > 1) {
                fileManager.CreateDirectory(parts[1]);
                cout << endl;
            }
            else {
                cout << "Error: Specify a directory name to create\n";
            }
        }
        else if (cmd == "rm") {
            if (parts.size() > 1) {
                fileManager.RemoveFileOrDirectory(parts[1]);
                cout << endl;
            }
            else {
                cout << "Error: Specify a name to remove\n";
            }
        }
        else if (cmd == "mv") {
            if (parts.size() > 2) {
                fileManager.MoveFileOrDirectory(parts[1], parts[2]);
                cout << endl;
            }
            else {
                cout << "Error: Specify source and destination to move\n";
            }
        }
        else if (cmd == "cp") {
            if (parts.size() > 2) {
                fileManager.CopyFileOrDirectory(parts[1], parts[2]);
                cout << endl;
            }
            else {
                cout << "Error: Specify source and destination to copy\n";
            }
        }
        else if (cmd == "rn") {
            if (parts.size() > 2) {
                fileManager.RenameFileOrDirectory(parts[1], parts[2]);
                cout << endl;
            }
            else {
                cout << "Error: Specify old and new name to rename\n";
            }
        }
        else if (cmd == "sr") {
            if (parts.size() > 1) {
                fileManager.SearchFiles(parts[1]);
                cout << endl;
            }
            else {
                cout << "Error: Specify a pattern to search for\n";
            }
        }
        else {
            cout << "Invalid command: " << command << "\n";
            cout << endl;
            //PrintCommands(); // Re-display the list of commands on invalid input
        }
    }

    return 0;
}