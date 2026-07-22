#include <cstddef>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

class StringHeap {
    public:
      StringHeap(){};

      void add(std::string newString){
        auto newSize = newString.size();
        if(newSize > m_maxLength){
            m_maxLength = newSize;
        }
        if(newString.empty()){
            std::println("Empty string inputted - ignoring entry");
            return;
        }
        store.push_back(newString);
        heapifyUp(store.size() - 1);
      }

      bool empty(){
        return store.empty();
      }

      size_t getMaxLength(){
        return m_maxLength;
      }

      std::string get(){
        if(store.empty()){
            return "";
        }
        std::string shortest = store[0]; 
        store[0] = store[store.size() - 1];
        store.pop_back();
        heapifyDown(0);
        return shortest;
      }

    private:
      size_t m_maxLength = 0;
      std::vector<std::string> store;
      size_t parentIndex(size_t idx){
        auto pIdx = idx / 2;
        return pIdx;
      };

      size_t leftChildIndex(size_t idx){
        auto lcIdx =  (2*idx) + 1;
        return lcIdx;
      }

      size_t rightChildIndex(size_t idx){
        auto rcIdx =  (2*idx)+2;
        return rcIdx;
      }

      bool compare(std::string parent, std::string child){
        if(parent == child){
            return true;
        }
        auto pLength = parent.length();
        auto cLength = child.length();
        if(pLength < cLength){
            return true;
        } else if (pLength > cLength){
            return false;
        } else {
            for(size_t i=0; i < pLength; i++){
                if(parent[i] < child[i]){
                    return true;
                } else if (parent[i] > child[i]){
                    return false;
                }
            }
        }

        return pLength <= cLength;
      }

      void heapifyUp(size_t idx){
        auto currIdx = idx;
        while(currIdx > 0){
            auto pIdx = parentIndex(currIdx);
            if(compare(store[pIdx], store[currIdx])){
                return;
            }
            std::string swap = store[pIdx];
            store[pIdx] = store[currIdx];
            store[currIdx] = swap;
            currIdx = pIdx;
        }
      }

      void heapifyDown(size_t idx){
        auto currIdx = idx;
        size_t lcIdx;
        size_t rcIdx;
        std::string swap;
        while(currIdx < store.size()){
            lcIdx = leftChildIndex(currIdx);
            rcIdx = rightChildIndex(currIdx);
            if(lcIdx >= store.size() && rcIdx >= store.size()){
                return;
            } 
            size_t idxOfSmallest;
            if(lcIdx >= store.size()){
                idxOfSmallest = rcIdx;
            } else if (rcIdx >= store.size()){
                idxOfSmallest = lcIdx;
            } else if (compare(store[lcIdx], store[rcIdx])){
                idxOfSmallest = lcIdx;
            } else {
                idxOfSmallest = rcIdx;
            }
            if(compare(store[currIdx], store[idxOfSmallest])){
                return;
            } else {
                swap = store[idxOfSmallest];
                store[idxOfSmallest] = store[currIdx];
                store[currIdx] = swap;
                currIdx = idxOfSmallest;
            }
        }
      }

};

std::string pad(size_t width, char letter){
    std::stringstream sstr;
    for(size_t i = 0; i < width; i++){
        sstr << letter;
    }
    return sstr.str();
}

std::string center(std::string toCenter, size_t width, char padChar){
    auto padWidth = width - toCenter.length();
    auto leftPadWidth = padWidth / 2;
    auto rightPadWidth = padWidth - leftPadWidth;
    auto leftPad = pad(leftPadWidth, padChar);
    auto rightPad = pad(rightPadWidth, padChar);
    return std::format("{}{}{}", leftPad, toCenter, rightPad);
}

int main() {
   
    StringHeap heap;
    std::string line;
    while(std::getline(std::cin, line)){
       if(line == "<()>"){
        std::println("stopping...");
        break;
       }
       heap.add(line); 
    }

    while(not heap.empty()){
        auto newLine = heap.get();
        auto padded = center(newLine, heap.getMaxLength(), '=');
        std::println(" <[( {} )]>",padded);
    }

    return 0;
}