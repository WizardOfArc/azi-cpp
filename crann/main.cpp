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
    } catch(std::exception e){
        name  = std::format("UNPRINTABLE ({})", e.what());
    }
    return name;
}

void displayUnknown(std::filesystem::path path, int depth){
    auto prefix = depth_prefix(depth);
    auto fn = path.filename();
    auto name = safe_path_to_string(fn.string());
    std::println("{} {} unknwn type <shrug>", prefix, name);
}

void displayNotExist(std::filesystem::path path, int depth){
    auto prefix = depth_prefix(depth);
    auto fn = path.filename();
    auto name = safe_path_to_string(fn);
    std::println("{} <<< {} DOES NOT EXIST >>>>", prefix, name);
}

void displaySimple(std::filesystem::path path, int depth){
    std::string name;
    auto filename = path.filename();
    auto prefix = depth_prefix(depth);
    name = safe_path_to_string(filename);
    std::println("{}{}", prefix, name);
}

void displayFile(fs::path path, int depth); // decl ahead

void displayDirectory(fs::path path, int depth){
    auto prefix = depth_prefix(depth);
    auto filename = path.filename();
    std::println("{}{} (dir)", prefix, safe_path_to_string(filename));
    // scan dir to get members and for each call the appropriate display function
    for(auto dir_entry : fs::directory_iterator(path)){
        auto name = dir_entry.path();
        displayFile(name, depth+1);
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
    if(argc < 2){
        std::println("No arguments were passed");
    } else {
        std::println("Argument passed");
        std::println("-> {}", argv[1]);
        if(argc > 2){
            std::println("the following are ignored:");
            for (int i = 2; i < argc; i++){
                std::println(" - {}", argv[i]);
            }
        }
    }
    auto dir = argv[1];
    displayFile(dir, 0);
    return 0;
}