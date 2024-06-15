#include "FileManager.h"

FileSystemItem::FileSystemItem(const string& name, const fs::path& path) : name{ name }, path{ path }, size(fs::file_size(path)),
creationTime(fs::last_write_time(path).time_since_epoch().count()),
modificationTime(fs::last_write_time(path).time_since_epoch().count()) {}

string FileSystemItem::GetName() const { return name; }
fs::path FileSystemItem::GetPath() const { return path; }
uintmax_t FileSystemItem::GetSize() const { return size; }
time_t FileSystemItem::GetCreationTime() const { return creationTime; }
time_t FileSystemItem::GetModificationTime() const { return modificationTime; }

// Реализация методов класса File
File::File(const string& name, const fs::path& path) : FileSystemItem{ name, path } {}

// Реализация методов класса Directory
Directory::Directory(const string& name, const fs::path& path) : FileSystemItem{ name, path } {
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry.path())) {
            _contents.emplace_back(std::make_unique<Directory>(entry.path().filename().string(), entry.path()));
        }
        else {
            _contents.emplace_back(std::make_unique<File>(entry.path().filename().string(), entry.path()));
        }
    }
}

vector<unique_ptr<FileSystemItem>>& Directory::GetContents() { return _contents; }

uintmax_t Directory::GetSize() const {
    uintmax_t totalSize = 0;
    for (const auto& item : _contents) {
        totalSize += item->GetSize();
    }
    return totalSize;
}

// Реализация методов класса FileManager
FileManager::FileManager(const fs::path& startingDir) : _currentDir{ startingDir } {}

void FileManager::ListDirectory() const {
    for (const auto& entry : fs::directory_iterator(_currentDir)) {
        cout << entry.path().filename() << "\n";
    }
}

void FileManager::CreateFile(const string& fileName) {
    try {
        fs::path filePath = _currentDir / fileName;

        if (!fs::exists(filePath.parent_path())) {
            fs::create_directories(filePath.parent_path());
        }

        ofstream file(filePath);
        if (file.is_open()) {
            cout << "File created: " << filePath << "\n";
            file.close();
        }
        else {
            cout << "Error creating file: " << filePath << "\n";
        }
    }
    catch (const fs::filesystem_error& e) {
        cout << "Filesystem error: " << e.what() << "\n";
    }
    catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

void FileManager::CreateDirectory(const string& dirName) {
    fs::path dirPath = _currentDir / dirName;
    if (fs::create_directory(dirPath)) {
        cout << "Directory created: " << dirPath << "\n";
    }
    else {
        cout << "Error creating directory: " << dirPath << "\n";
    }
}

void FileManager::RemoveFileOrDirectory(const string& name) {
    fs::path path = _currentDir / name;
    if (fs::is_directory(path)) {
        if (fs::remove_all(path)) {
            cout << "Directory removed: " << path << "\n";
        }
        else {
            cout << "Error removing directory: " << path << "\n";
        }
    }
    else if (fs::is_regular_file(path)) {
        if (fs::remove(path)) {
            cout << "File removed: " << path << "\n";
        }
        else {
            cout << "Error removing file: " << path << "\n";
        }
    }
    else {
        cout << "Error: " << path << " is not a file or directory.\n";
    }
}

void FileManager::RenameFileOrDirectory(const string& oldName, const string& newName) {
    fs::path oldPath = _currentDir / oldName;
    fs::path newPath = _currentDir / newName;

    if (fs::exists(oldPath)) {
        try {
            fs::rename(oldPath, newPath);
            cout << "Renamed " << oldPath << " to " << newPath << "\n";
        }
        catch (const fs::filesystem_error& ex) {
            cout << "Error renaming " << oldPath << " to " << newPath << ": " << ex.what() << "\n";
        }
        catch (...) {
            cout << "Error renaming " << oldPath << " to " << newPath << ": Unknown error\n";
        }
    }
    else {
        cout << "Error: " << oldPath << " does not exist.\n";
    }
}

void FileManager::CopyFileOrDirectory(const string& source, const string& destination) {
    fs::path sourcePath = _currentDir / source;
    fs::path destinationPath = _currentDir / destination;

    if (fs::exists(sourcePath)) {
        if (fs::is_directory(sourcePath)) {
            try {
                fs::create_directory(destinationPath);
                for (const auto& entry : fs::directory_iterator(sourcePath)) {
                    CopyFileOrDirectory(entry.path().filename().string(), (destinationPath / entry.path().filename()).string());
                }
                cout << "Directory copied: " << sourcePath << " to " << destinationPath << "\n";
            }
            catch (const fs::filesystem_error& e) {
                cout << "Error copying directory: " << sourcePath << " to " << destinationPath << "\n";
                cout << e.what() << "\n";
            }
            catch (...) {
                cout << "Error copying directory: " << sourcePath << " to " << destinationPath << ": Unknown error\n";
            }
        }
        else if (fs::is_regular_file(sourcePath)) {
            try {
                ifstream src(sourcePath, std::ios::binary);
                ofstream dst(destinationPath, std::ios::binary);
                if (!src.is_open() || !dst.is_open()) {
                    cout << "Error opening file for copy: " << sourcePath << " to " << destinationPath << "\n";
                    return;
                }
                dst << src.rdbuf();
                src.close();
                dst.close();
                cout << "File copied: " << sourcePath << " to " << destinationPath << "\n";
            }
            catch (const std::ios_base::failure& e) {
                cout << "Error copying file: " << sourcePath << " to " << destinationPath << "\n";
                cout << e.what() << "\n";
            }
            catch (...) {
                cout << "Error copying file: " << sourcePath << " to " << destinationPath << ": Unknown error\n";
            }
        }
        else {
            cout << "Error: " << sourcePath << " is not a file or directory.\n";
        }
    }
    else {
        cout << "Error: " << sourcePath << " does not exist.\n";
    }
}

void FileManager::MoveFileOrDirectory(const string& source, const string& destination) {
    fs::path sourcePath = _currentDir / source;
    fs::path destinationPath = _currentDir / destination;

    if (fs::exists(sourcePath)) {
        if (fs::is_directory(sourcePath)) {
            try {
                fs::rename(sourcePath, destinationPath);
                cout << "Directory moved: " << sourcePath << " to " << destinationPath << "\n";
            }
            catch (const fs::filesystem_error& e) {
                try {
                    fs::create_directory(destinationPath);
                    for (const auto& entry : fs::directory_iterator(sourcePath)) {
                        MoveFileOrDirectory(entry.path().filename().string(), (destinationPath / entry.path().filename()).string());
                    }
                    fs::remove_all(sourcePath);
                    cout << "Directory moved: " << sourcePath << " to " << destinationPath << "\n";
                }
                catch (const fs::filesystem_error& e) {
                    cout << "Error moving directory: " << sourcePath << " to " << destinationPath << "\n";
                    cout << e.what() << "\n";
                }
            }
        }
        else if (fs::is_regular_file(sourcePath)) {
            try {
                fs::rename(sourcePath, destinationPath);
                cout << "File moved: " << sourcePath << " to " << destinationPath << "\n";
            }
            catch (const fs::filesystem_error& e) {
                try {
                    ifstream src(sourcePath, std::ios::binary);
                    ofstream dst(destinationPath, std::ios::binary);
                    dst << src.rdbuf();
                    src.close();
                    dst.close();
                    fs::remove(sourcePath);
                    cout << "File moved: " << sourcePath << " to " << destinationPath << "\n";
                }
                catch (const std::ios_base::failure& e) {
                    cout << "Error moving file: " << sourcePath << " to " << destinationPath << "\n";
                    cout << e.what() << "\n";
                }
            }
        }
        else {
            cout << "Error: " << sourcePath << " is not a file or directory.\n";
        }
    }
    else {
        cout << "Error: " << sourcePath << " does not exist.\n";
    }
}

void FileManager::ChangeDirectory(const string& dirName) {
    fs::path newDir;
    if (dirName == "..") {
        newDir = _currentDir.parent_path();
    }
    else {
        newDir = _currentDir / dirName;
    }

    if (fs::is_directory(newDir)) {
        _currentDir = newDir;
        cout << "Current directory changed to: " << newDir << "\n";
    }
    else {
        cout << "Error: " << newDir << " is not a directory.\n";
    }
}

void FileManager::ShowCurrentPath() const {
    cout << "Current path: " << _currentDir << "\n";
}

void FileManager::SearchFiles(const string& pattern) const {
    cout << "Searching for files matching pattern: " << pattern << "\n";
    for (const auto& entry : fs::recursive_directory_iterator(_currentDir)) {
        if (fs::is_regular_file(entry.path()) && entry.path().filename().string().find(pattern) != string::npos) {
            cout << entry.path() << "\n";
        }
    }
}