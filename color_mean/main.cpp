#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

class Color {
private:
    float m_red;
    float m_green;
    float m_blue;
      
public:
    Color(float red, float green, float blue): 
    m_red(normalize(red)),
    m_green(normalize(green)),
    m_blue(normalize(blue)){
    }

    float normalize(float color_factor){
        if(color_factor < 0) return 0;
        if(color_factor > 100) return 100;
        return color_factor;
    }

    std::string to_string(){
        std::stringstream ss;
        ss << "R:" << m_red << ", ";
        ss << "G:" << m_green << ", ";
        ss << "B:" << m_blue;

        return ss.str();
    }

    float get_red(){
        return m_red;
    }

    float get_green(){
        return m_red;
    }

    float get_blue(){
        return m_red;
    }

    Color mean_color(Color other){
        float red = (m_red + other.get_red()) / 2;
        float green = (m_green + other.get_green()) / 2;
        float blue = (m_blue + other.get_blue()) / 2;
        return Color(red, green, blue);
    }

};

int main() {
   float red;
   float green;
   float blue;
   std::cout << "Enter R G B for first colour:";
   std::cin >> red >> green >> blue;
   Color first_color(red, green, blue);
   std::cout << "First color normalized => " << first_color.to_string() << "\n";
   std::cout << "Enter R G B for secnd colour:";
   std::cin >> red >> green >> blue;
   Color second_color(red, green, blue);
   std::cout << "Second color normalized => " << second_color.to_string() << "\n";

   Color mean = first_color.mean_color(second_color);
   std::cout << "\nMean Color:\n" << mean.to_string() << "\n";
   
   return 0;
}