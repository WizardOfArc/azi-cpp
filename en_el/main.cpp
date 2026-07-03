#include <cstddef>
#include <cstdio>
#include <print>
#include <fstream>
#include <stdexcept>

class FileHandler {
public:
   explicit FileHandler(const std::string& filename, std::ios::openmode mode = std::ios::in): m_file_stream{filename, mode} {
      if(not m_file_stream.is_open()){
        throw std::runtime_error("Failed to open file: " + filename);
      }
   }
   ~FileHandler(){
    if(not m_file_stream.is_open()) return;
    else m_file_stream.close();
   }


   std::ifstream& getStream(){
      return m_file_stream;
   }

   bool is_open() const{
    return m_file_stream.is_open();
   }

private:
   std::ifstream m_file_stream;
};

class LineCounter {
public:
   explicit LineCounter(const std::string& file_name):
   m_file_handler{file_name}{}
   size_t count_lines(){
        std::string line;
        std::ifstream& fs{m_file_handler.getStream()};
        size_t count = 0;
        while(std::getline(fs, line)){
            count++;
        } 
        return count;
   }
private:
   FileHandler m_file_handler;

};

int main(int argc, char* argv[]){
   if(argc < 2){
      std::println("please provide a filename");
      return 1;
   }
   namespace fs = std::filesystem;
   for(int i=1; i < argc; i++){
      auto filename = argv[i];
      auto status = fs::status(filename);
      auto ft = status.type();
      if(ft == fs::file_type::directory){
        std::println("'{}' is a directory", filename);
        continue;
      }
      try{
      LineCounter lc{filename};
        auto count = lc.count_lines();
        std::println("{}: {}", filename, count);
      } catch (const std::runtime_error& e) {
        std::println("{}: ?? ({})", filename, e.what());
      }
   }
}