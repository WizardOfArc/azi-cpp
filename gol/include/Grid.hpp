#pragma once
#include <vector>

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
	 m_matrix(rows, std::vector<bool>(cols))
	 {
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
				if(m_matrix[static_cast<size_t>(y)][static_cast<size_t>(x)]) continue; 
				neighbors.push_back(Cell{x,y});
			}
		}
		return neighbors;
	}

	int countLiveNeighbors(const Cell& cell){
		auto originY = cell.y;
		size_t upY = static_cast<size_t>(originY == 0 ? m_rows - 1 : originY - 1);
		size_t dnY = static_cast<size_t>(originY == m_rows - 1 ? 0 : originY + 1);
		std::vector<size_t> yCoords{upY, originY, dnY};

		auto originX = cell.x;
		size_t ltX = static_cast<size_t>(originX == 0 ? m_cols - 1 : originX - 1);
		size_t rtX = static_cast<size_t>(originX == m_cols - 1 ? 0 : originX + 1);
		std::vector<size_t> xCoords{ltX, originX, rtX};
		int count = 0;
		for(auto y: yCoords){
			for(auto x: xCoords){
                if(y == originY && x == originX){
					continue;
				}
				if(m_matrix[y][x]){
					count++;
				}
			}
		}
		return count;
	}

	float getCellRadius(){
		return static_cast<float>(m_cell_radius);
	}

	void killCell(Cell& cell){
		m_matrix[cell.y][cell.x] = false;
	}

	void birthCell(Cell& cell){
		m_matrix[cell.y][cell.x] = true;
	}

  private:
     uint32_t m_cols;
	 uint32_t m_rows;
	 uint32_t m_screen_w;
	 uint32_t m_screen_h;
	 uint32_t m_cell_width;
	 uint32_t m_cell_height;
	 uint32_t m_cell_radius;
	 std::vector<std::vector<bool>> m_matrix;
};