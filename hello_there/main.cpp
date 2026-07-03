#include "CSVParser.hpp"
#include <cstddef>
#include <exception>
#include <print>
#include <sstream>

enum class Justification{
    Left,
    Right
};

std::string pad(int padding) {
    std::stringstream sstream{""};
    for(int i=0; i < padding; i++){
        sstream << " ";
    }
    return sstream.str();
}

std::string center(std::string& text, size_t width){
    auto text_width = text.length();
    auto difference = width - text_width;
    if(difference <= 0){
        return text;
    }
    auto left = difference / 2;
    auto right = difference - left;
    auto left_padding = pad((int)left);
    auto right_padding = pad((int)right);
    return left_padding + text + right_padding;
}

std::string justify(std::string& text, size_t width, Justification justify){
    auto with_air = " " + text + " ";
    auto text_width = with_air.length();
    auto difference = width - text_width;
    if(difference <= 0){
        return with_air;
    }
    auto padding = pad((int) difference);
    switch (justify) {
        case Justification::Left:
            return with_air + padding;
        case Justification::Right:
            return padding + with_air;
    }
}

std::string horizontal_border(std::vector<size_t>& column_widths){
    std::stringstream sstream;
    sstream << "+";
    for(auto width : column_widths) {
        for(size_t i=0; i < width; i++){
            sstream << "-";
        }
        sstream << "+";
    }
    return sstream.str();
}

std::string pretty_row(std::vector<std::string>& row, std::vector<size_t>& column_widths) {
    std::stringstream sstream;
    auto row_size = row.size();
    auto num_columns = column_widths.size();
    sstream << "|";
    for(size_t i=0; i < num_columns; i++){
        std::string curr_cell;
        if(i < row_size){
            curr_cell = row[i];
        } else {
            curr_cell = "";
        }
        std::string padded_cell;
        auto curr_width = column_widths[i];
        if(i==0){
            padded_cell = justify(curr_cell, curr_width, Justification::Left);
        } else if (i == num_columns - 1){
            padded_cell = justify(curr_cell, curr_width, Justification::Right);
        } else {
            padded_cell = center(curr_cell, curr_width);
        }
        sstream << padded_cell << "|";
    }
    return sstream.str();
}

std::string pretty_table(std::vector<std::vector<std::string>>& table, std::vector<size_t>& column_widths) {
    std::stringstream sstream;
    sstream << horizontal_border(column_widths) << "\n";
    for(auto row : table){
        sstream << pretty_row(row, column_widths) << "\n";
        sstream << horizontal_border(column_widths) << "\n";
    }
    return sstream.str();
}

int main() {
    try{
       CSVParser parser = CSVParser("data.csv");
       auto data = parser.parse();

       // get number of columns
       size_t num_columns = 0;
       for(auto& row : data){
          size_t cols_per_row = row.size(); 
          if(cols_per_row > num_columns){
            num_columns = cols_per_row;
          }
       }
       // set up column width mapping
        std::vector<size_t> column_widths(num_columns);
       constexpr int spacing = 2;
       for(auto& row: data){
          auto curr_row_size = row.size();
          for(size_t i = 0; i < num_columns; i++){
             if(i < curr_row_size){
                auto old_col_width = column_widths[i];
                auto cell_width = row[i].length() + spacing;
                if(cell_width > old_col_width){
                    column_widths[i] = cell_width;
                }
             } 
          }
       }
       std::println("the pretty CSV:\n{}",pretty_table(data, column_widths));
    } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << "\n";

    }
}