#include <exception>
#include <filesystem>
#include <print>
#include <sstream>
#include <string>

/*
  need something that scans directories 
  and we might start with recursive design first for simplicity
  if simple file - print as simple file
  if directory print name followed by a directory indicator 
  and for each item in directory recurse but pepended by a depth indicator / symbol to link to parent  

  if no argument - assume current directory
  if more than one argument only look at first argument
*/

namespace fs = std::filesystem;

inline std::string depth_prefix(int depth){
    std::stringstream ss;
    int step = depth;
    while ( step > 0 ){
        if(step == 1){
            ss << "+-- ";
        } else {
            ss << "|   ";
        }
        step--;
    }
    return ss.str();
}

inline std::string safe_path_to_string(fs::path path){
    std::string name;
    try {
        name = path.string();
    } catch(std::filesystem::filesystem_error e){
        name  = std::format("UNPRINTABLE ({})", e.what());
    }
    return name;
}

void displayFile(fs::path path, int depth); // decl ahead

void displayGeneric(std::filesystem::path path, int depth, std::string suffix){
    auto prefix = depth_prefix(depth);
    auto fn = path.filename();
    auto name = safe_path_to_string(fn.string());
    std::println("{}{}{}", prefix, name, suffix);
} 

void displayUnknown(std::filesystem::path path, int depth){
    std::string suffix = " unknown type <shrug>";
    displayGeneric(path, depth, suffix);
}

void displayNotExist(std::filesystem::path path, int depth){
    std::string suffix = " <<< DOES NOT EXIST >>>>";
    displayGeneric(path, depth, suffix);
}

void displaySimple(std::filesystem::path path, int depth){
    std::string suffix = "";
    displayGeneric(path, depth, suffix);
}


void displayDirectory(fs::path path, int depth){
    std::string suffix = ":";
    displayGeneric(path, depth, suffix);
    for(auto dir_entry : fs::directory_iterator(path)){
        try {
            auto name = dir_entry.path();
            displayFile(name, depth+1);

        } catch (std::filesystem::filesystem_error ferr){
            std::println("someone isn't happy {}", ferr.what());
        }
    }
}

// need full path for investigation but filename for display
void displayFile(fs::path path, int depth){
    if(fs::is_directory(path)){
        displayDirectory(path, depth);
    }
    else if(fs::is_regular_file(path)){
        displaySimple(path, depth);
    }
    else if(not fs::exists(path)){displayNotExist(path,  depth);}
    else {displayUnknown(path, depth);}
}

int main(int argc, char* argv[]){
    fs::path dir;
    if(argc < 2){
        dir = ".";
    } else {
        dir = argv[1];
    }
    displayFile(dir, 0);
    return 0;
}