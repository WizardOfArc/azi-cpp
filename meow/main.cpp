#include <filesystem>
#include <print>
#include <fstream>
#include <stdexcept>
#include <string>
#include <set>
#include <vector>

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


class FileReader {
    public: 
       explicit FileReader(const std::string& file_name, const bool& show_number, const bool& squeeze):
       m_file_handler{file_name},
       m_show_number{show_number},
       m_squeeze{squeeze}{}

       void display(){
           std::string line;
           int count = 0;
           std::ifstream& fs{m_file_handler.getStream()};
           while(std::getline(fs, line)){
              count++;
              if(line == "" && m_squeeze) continue;
              
              if(m_show_number){
                std::println("{}: {}", count, line);
              } else {
                std::println("{}", line);
              }
              
           }

       }

    private:
      FileHandler m_file_handler;
      bool m_show_number;
      bool m_squeeze;
};

class OptGetter {
    public:
    explicit OptGetter(int raw_argc, char* raw_args[]){
        if(raw_argc == 1) return;
        for(int i=1; i< raw_argc; i++){
            if(raw_args[i][0] == '-'){
                // add option to set
                for(int j = 1; raw_args[i][j] != 0; j++){
                    m_options.insert(raw_args[i][j]);
                }
            } else {
                m_optionless_args.push_back(std::string{raw_args[i]});
            }
        }
    }
    bool has_opt(char opt){
        return m_options.contains(opt);
    }

    std::vector<std::string> get_args(){
        return m_optionless_args;
    }
    private:
      std::vector<std::string> m_optionless_args;
      std::set<char> m_options;
};

int main(int argc, char* argv[]){
    bool number = false;
    bool squeeze = false;

    auto opts = OptGetter{argc, argv};
    if(opts.has_opt('n')){
        number = true;
    }
    if(opts.has_opt('s')){
        squeeze = true;
    }

    namespace fs = std::filesystem;
    auto args = opts.get_args();
    for (auto fn : args){
        auto status = fs::status(fn);
        auto file_type = status.type();
        if(file_type == fs::file_type::directory){
            std::println("'{}' is a directory", fn);
            continue;
        } 
        try{
            auto reader = FileReader{fn, number, squeeze};
            reader.display();
        } catch (const std::runtime_error& e) {
            std::println(">>> {} could not be read ({}) <<<", fn, e.what());
        }
    }
}