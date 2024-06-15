#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <ctime>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

// Базовый класс FileSystemItem
class FileSystemItem {
protected:
    string name;
    fs::path path;
    uintmax_t size;
    time_t creationTime;
    time_t modificationTime;

public:
    FileSystemItem(const string& name, const fs::path& path);
    virtual ~FileSystemItem() = default;

    string GetName() const;
    fs::path GetPath() const;
    uintmax_t GetSize() const;
    time_t GetCreationTime() const;
    time_t GetModificationTime() const;
};

// Производный класс File
class File : public FileSystemItem {
public:
    File(const string& name, const fs::path& path);
};

// Производный класс Directory
class Directory : public FileSystemItem {
private:
    vector<unique_ptr<FileSystemItem>> _contents;

public:
    Directory(const string& name, const fs::path& path);
    vector<unique_ptr<FileSystemItem>>& GetContents();
    uintmax_t GetSize() const;
};

// Класс FileManager
class FileManager {
private:
    fs::path _currentDir;

public:
    FileManager(const fs::path& startingDir = fs::current_path());

    void ListDirectory() const;
    void CreateFile(const string& fileName);
    void CreateDirectory(const string& dirName);
    void RemoveFileOrDirectory(const string& name);
    void RenameFileOrDirectory(const string& oldName, const string& newName);
    void CopyFileOrDirectory(const string& source, const string& destination);
    void MoveFileOrDirectory(const string& source, const string& destination);
    void ChangeDirectory(const string& dirName);
    void ShowCurrentPath() const;
    void SearchFiles(const string& pattern) const;
};


