#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <set>

struct Cell {
    uint32_t x;
	uint32_t y;

	bool operator<(const Cell& other) const {
       return x < other.x || (x == other.x && y < other.y);
	}
};

class Grid {
  public:
     explicit Grid(uint32_t cols, uint32_t rows, uint32_t screen_w, uint32_t screen_h):
	 m_cols{cols},
	 m_rows{rows},
	 m_screen_w(screen_w),
	 m_screen_h(screen_h),
	 m_u8_matrix(std::vector<uint8_t>(cols*rows))
	 {
		m_cell_width = static_cast<uint32_t>(static_cast<float>(m_screen_w) / static_cast<float>(m_cols));
		m_cell_height = static_cast<uint32_t>(static_cast<float>(m_screen_h) / static_cast<float>(m_rows));
	 }

	 explicit Grid(){
		Grid(40, 30, 800, 600);
	 }

	void updateDimensions(uint32_t cols, uint32_t rows){
         m_cols = cols;
		 m_rows = rows;
		 m_u8_matrix = std::vector<uint8_t>(rows*cols);
		m_cell_width = static_cast<uint32_t>(static_cast<float>(m_screen_w) / static_cast<float>(m_cols));
		m_cell_height = static_cast<uint32_t>(static_cast<float>(m_screen_h) / static_cast<float>(m_rows));
	}

    uint32_t maxPosX(){
        return (m_cols * m_cell_width) - 1;
    }

    uint32_t maxPosY(){
        return (m_rows * m_cell_height) - 1;
    }

	uint32_t posX(uint32_t x){
		return x * m_cell_width;
	}

	uint32_t colFromPosX(uint32_t pos_x) {
		return pos_x / m_cell_width;
	}

	uint32_t posY(uint32_t y){
		return y * m_cell_height;
	}

	uint32_t rowFromPosY(uint32_t pos_y){
		return pos_y / m_cell_height;
	}

	uint32_t getCols(){
		return m_cols;
	}

	uint32_t getRows(){
		return m_rows;
	}


	std::vector<Cell> getDeadNeighbors(){
		std::vector<Cell> neighbors;
		for (auto nIdx : m_deadNeighbors){
			auto x = static_cast<uint32_t>(nIdx % m_cols);
			auto y = static_cast<uint32_t>(nIdx / m_cols);
			neighbors.push_back(Cell{x,y});
		}
		return neighbors;
	}

	int countLiveNeighbors(const Cell& cell){
		return m_u8_matrix[ cell.y * m_cols + cell.x] >> 1;
	}

	float getCellWidth(){
		return static_cast<float>(m_cell_width);
	}

	float getCellHeight(){
		return static_cast<float>(m_cell_height);
	}

	void removeDeadNeighbor(Cell &cell){
		size_t nIdx = cell.y * m_cols + cell.x;
		m_deadNeighbors.erase(nIdx);
	}

	void killCell(Cell& cell){
		size_t idx = cell.y * m_cols + cell.x;
		m_u8_matrix[idx] &= (15 << 1); // bitwise & with 11110 to turn off last bit only
		// update neighbors
		size_t upY = cell.y == 0 ? m_rows - 1 : cell.y - 1;
		size_t dnY = cell.y == m_rows - 1 ? 0 : cell.y + 1;
		size_t ltX = cell.x == 0 ? m_cols - 1 : cell.x - 1;
		size_t rtX = cell.x == m_cols - 1 ? 0 : cell.x + 1;
		std::vector<size_t> neighborIdxs = {
		 upY * m_cols + cell.x,
		 upY * m_cols + rtX,
		 cell.y * m_cols + rtX,
		 dnY * m_cols + rtX,
		 dnY * m_cols + cell.x,
		 dnY * m_cols + ltX,
		 cell.y * m_cols + ltX,
		 upY * m_cols + ltX
		};
		for(auto nIdx : neighborIdxs){
			m_u8_matrix[nIdx] -= 2;
			if(m_u8_matrix[nIdx] % 2 == 1){
				m_deadNeighbors.erase(nIdx);
			}
		}
		// ones bit is liveness bit the remaining bits are live neighbor count
	}

	void birthCell(Cell& cell){
		size_t idx = cell.y * m_cols + cell.x;
		m_u8_matrix[idx] |= 1;
		m_deadNeighbors.erase(idx);
		// update neighbors
		size_t upY = cell.y == 0 ? m_rows - 1 : cell.y - 1;
		size_t dnY = cell.y == m_rows - 1 ? 0 : cell.y + 1;
		size_t ltX = cell.x == 0 ? m_cols - 1 : cell.x - 1;
		size_t rtX = cell.x == m_cols - 1 ? 0 : cell.x + 1;
        
		std::vector<size_t> neighborIdxs = {
		 upY * m_cols + cell.x,
		 upY * m_cols + rtX,
		 cell.y * m_cols + rtX,
		 dnY * m_cols + rtX,
		 dnY * m_cols + cell.x,
		 dnY * m_cols + ltX,
		 cell.y * m_cols + ltX,
		 upY * m_cols + ltX
		};

		for(auto nIdx : neighborIdxs){
			m_u8_matrix[nIdx] += 2;
			if(m_u8_matrix[nIdx] % 2 == 0){
                m_deadNeighbors.insert(nIdx);
			}
		}
		// ones bit is liveness bit the remaining bits are live neighbor count
	}

  private:
     uint32_t m_cols;
	 uint32_t m_rows;
	 uint32_t m_screen_w;
	 uint32_t m_screen_h;
	 uint32_t m_cell_width;
	 uint32_t m_cell_height;
	 std::vector<uint8_t> m_u8_matrix;
	 std::set<size_t> m_deadNeighbors;
	 //  don't care about the rest
};