#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

#include <print>

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
        uint32_t minimum = m_cell_height < m_cell_width ? m_cell_height : m_cell_width;
		m_cell_radius =  minimum / 2;
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
        uint32_t minimum = m_cell_height < m_cell_width ? m_cell_height : m_cell_width;
		m_cell_radius =  minimum / 2;

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

	std::vector<Cell> getDeadNeighbors(const Cell& cell){
		auto originY = cell.y;
		uint32_t upY = originY == 0 ? m_rows - 1 : originY - 1;
		uint32_t dnY = originY == m_rows - 1 ? 0 : originY + 1;
		std::vector<uint32_t> yCoords{upY, originY, dnY};

		auto originX = cell.x;
		uint32_t ltX = originX == 0 ? m_cols - 1 : originX - 1;
		uint32_t rtX = originX == m_cols - 1 ? 0 : originX + 1;
		std::vector<uint32_t> xCoords{ltX, originX, rtX};

		std::vector<Cell> neighbors;
		for(auto y : yCoords){
			for(auto x: xCoords){
				if(x == originX && y == originY) continue;
				if(m_u8_matrix[y*m_cols + x] % 2 == 0){
					neighbors.push_back(Cell{x,y});
				}
			}
		}
		return neighbors;
	}

	int countLiveNeighbors(const Cell& cell){
		return m_u8_matrix[ cell.y * m_cols + cell.x] >> 1;
	}

	float getCellRadius(){
		return static_cast<float>(m_cell_radius);
	}

	float getCellWidth(){
		return static_cast<float>(m_cell_width);
	}

	float getCellHeight(){
		return static_cast<float>(m_cell_height);
	}

	void killCell(Cell& cell){
		size_t idx = cell.y * m_cols + cell.x;
		m_u8_matrix[idx] &= (15 << 1); // bitwise & with 11110 to turn off last bit only
		// update neighbors
		size_t upY = cell.y == 0 ? m_rows - 1 : cell.y - 1;
		size_t dnY = cell.y == m_rows - 1 ? 0 : cell.y + 1;
		size_t ltX = cell.x == 0 ? m_cols - 1 : cell.x - 1;
		size_t rtX = cell.y == m_cols - 1 ? 0 : cell.x + 1;
		// ones bit is liveness bit the remaining bits are live neighbor count
		m_u8_matrix[ upY * m_cols + cell.x ] -= 2; 
		m_u8_matrix[  upY * m_cols + rtX ] -= 2;
		m_u8_matrix[  cell.y * m_cols + rtX ] -= 2;
		m_u8_matrix[  dnY * m_cols + rtX ] -= 2;
		m_u8_matrix[  dnY * m_cols + cell.x ] -= 2;
		m_u8_matrix[  dnY * m_cols + ltX ] -= 2;
		m_u8_matrix[  cell.y * m_cols + ltX ] -= 2;
        m_u8_matrix[  upY * m_cols + ltX ] -= 2;
		// TODO add logic to update neighboring cells'
		// live neighbor count
	}

	void birthCell(Cell& cell){
		size_t idx = cell.y * m_cols + cell.x;
		m_u8_matrix[idx] |= 1;
		// update neighbors
		size_t upY = cell.y == 0 ? m_rows - 1 : cell.y - 1;
		size_t dnY = cell.y == m_rows - 1 ? 0 : cell.y + 1;
		size_t ltX = cell.x == 0 ? m_cols - 1 : cell.x - 1;
		size_t rtX = cell.y == m_cols - 1 ? 0 : cell.x + 1;
		// ones bit is liveness bit the remaining bits are live neighbor count
		m_u8_matrix[ upY * m_cols + cell.x ] += 2; 
		m_u8_matrix[  upY * m_cols + rtX ] += 2;
		m_u8_matrix[  cell.y * m_cols + rtX ] += 2;
		m_u8_matrix[  dnY * m_cols + rtX ] += 2;
		m_u8_matrix[  dnY * m_cols + cell.x ] += 2;
		m_u8_matrix[  dnY * m_cols + ltX ] += 2;
		m_u8_matrix[  cell.y * m_cols + ltX ] += 2;
        m_u8_matrix[  upY * m_cols + ltX ] += 2;
		// TODO add logic to update neighboring cells'
		// live neighbor count
	}

  private:
     uint32_t m_cols;
	 uint32_t m_rows;
	 uint32_t m_screen_w;
	 uint32_t m_screen_h;
	 uint32_t m_cell_width;
	 uint32_t m_cell_height;
	 uint32_t m_cell_radius;
	 std::vector<uint8_t> m_u8_matrix;
	 // TODO replace vector vector of bool
	 // with a vector of char or uint8
	 // where we can store neighbor counts in bits 
	 // and liveness in the least bit
	 //  00000000
	 //     ^  ^^
	 //     |  |+-- liveness bit
	 //     |  +-- end live neighbor count bits
	 //     +--  start live neighbor count bits
	 //  don't care about the rest
};