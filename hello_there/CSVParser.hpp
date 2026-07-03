#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <vector>

class FileHandler {
public:
   explicit FileHandler(const std::string& filename, std::ios::openmode mode = std::ios::in);
   ~FileHandler();


   std::ifstream& getStream();
   bool is_open() const;

private:
   std::ifstream m_file_stream;
};

class CSVParser {
public:
    explicit CSVParser(const std::string& file_name, char delimiter = ',');
    std::vector<std::vector<std::string>> parse();
private:
    FileHandler m_file_handler;
    char m_delimiter;
};