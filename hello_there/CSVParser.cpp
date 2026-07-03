#include "CSVParser.hpp"
#include <sstream>
#include <stdexcept>
#include <string>

FileHandler::FileHandler(const std::string& filename, std::ios::openmode mode):
  m_file_stream{filename, mode}
{
   if(not m_file_stream.is_open()){
      throw std::runtime_error("Failed to open file: " + filename);
   }
}

FileHandler::~FileHandler(){
    if(not m_file_stream.is_open()) return;
    else m_file_stream.close();
}

std::ifstream& FileHandler::getStream(){
   return m_file_stream;
}

bool FileHandler::is_open() const{
   return m_file_stream.is_open();
}

CSVParser::CSVParser(const std::string& file_name, char delimiter): 
m_file_handler{file_name},
m_delimiter{delimiter}{

}

using row_t = std::vector<std::string>;
std::vector<row_t> CSVParser::parse(){
   std::ifstream& file{ m_file_handler.getStream() };
   std::vector<row_t> data;
   std::string line;  

   while(std::getline(file, line)){
      row_t row;
      std::stringstream sstream{line};
      std::string token;
      while(std::getline(sstream, token, m_delimiter)){
         row.push_back(token);
      }
      data.push_back(row);
   }
   return data;
}