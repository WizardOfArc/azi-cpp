#pragma once
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>
#include <format>
#include "Grid.hpp"


struct CellRenderData {
    float width;
    float height;
    float scrnX;
    float scrnY;
};

class GameState {
    public:
        explicit GameState(uint32_t scrWidth, uint32_t scrHeight, size_t scale):  m_scale{scale} {
            m_paused = false;
            auto requestedScale = m_sizes[m_scale];
            const uint32_t gridCols = 40*requestedScale;
            const uint32_t gridRows = 30*requestedScale;
            m_grid = Grid{gridCols, gridRows, scrWidth, scrHeight};
        } 

        uint32_t rightMost(){
            return m_grid.maxPosX();
        }

        uint32_t bottomMost(){
            return m_grid.maxPosY();
        }

        bool isPaused(){
            return m_paused;
        }

        void togglePause(){
            m_paused = !m_paused;
        }

        void updateNursery(){
            m_nursery.clear();
            auto neighbors = m_grid.getDeadNeighbors();
            for( const auto neighbor : neighbors){
                m_nursery.insert(neighbor);
            }
        }

        uint32_t column(int x){
            return m_grid.colFromPosX(static_cast<uint32_t>(x));
        }

        uint32_t row(int y){
            return m_grid.rowFromPosY(static_cast<uint32_t>(y));
        }

        std::vector<Cell> getLivingCells(){
            std::vector<Cell> livingCells;
            for(auto cell: m_live){
                livingCells.push_back(cell);
            }
            return livingCells;
        }

        std::vector<Cell> getNursery(){
            std::vector<Cell> nursery;
            for(auto cell: m_nursery){
                nursery.push_back(cell);
            }
            return nursery;
        }

        void calculateNextRound(){
            std::vector<Cell> toDie;
            std::vector<Cell> toLive;
            std::vector<Cell> toRemoveFromNursery;
            for(const auto live: m_live){
                int neighbors = m_grid.countLiveNeighbors(live);
                if(neighbors < 2 || neighbors > 3){
                    toDie.push_back(live);
                }
            }
            for(const auto nurse: m_nursery){
                int neighbors = m_grid.countLiveNeighbors(nurse);
                if(neighbors == 3){
                    toLive.push_back(nurse);
                } else if (neighbors == 0){
                    toRemoveFromNursery.push_back(nurse);
                }
            }
            for(auto denurse: toRemoveFromNursery){
                m_grid.removeDeadNeighbor(denurse);
            }

            for(auto kill: toDie){
                m_live.erase(kill);
                m_grid.killCell(kill);
            }
            for(auto generate: toLive){
                m_live.insert(generate);
                m_grid.birthCell(generate);
            }
            updateNursery();
        }

        void clear(){
            for(auto live: m_live){
                m_grid.killCell(live);
            }
            m_nursery.clear();
            m_live.clear();
        }

        CellRenderData getCellRenderData(const Cell& cell){
            return CellRenderData{
                .width=m_grid.getCellWidth(),
                .height=m_grid.getCellHeight(),
                .scrnX=static_cast<float>(m_grid.posX(cell.x)),
                .scrnY=static_cast<float>(m_grid.posY(cell.y))
            };
        }

    void placeShape(int posX, int posY, std::vector<std::vector<bool>> layout){
         /*
             +---- w ----+
             |           |
             |    o      |
             h           h
             |           |
             +-----w-----+
         */
         auto h = static_cast<int>(layout.size());
         auto w = static_cast<int>(layout[0].size());
         auto originX = m_grid.colFromPosX(static_cast<uint32_t>(posX));
         auto originY = m_grid.rowFromPosY(static_cast<uint32_t>(posY));
         int colIdxForOrigin = w % 2 == 0 ? (w-1) / 2 : w / 2;
         int rowIdxForOrigin = h % 2 == 0 ? (h-1) / 2 : h / 2;
         int originCol = static_cast<int>(originX);
         int originRow = static_cast<int>(originY);
         for(int row = 0; row < h; row++){
            for(int col = 0; col < w; col++){
                if(not layout[static_cast<size_t>(row)][static_cast<size_t>(col)]) continue;
                int colOffset = col - colIdxForOrigin;
                int rowOffest = row - rowIdxForOrigin;
                uint32_t cellX;
                if(originCol + colOffset >= 0){
                    cellX = static_cast<uint32_t>(originCol + colOffset) % m_grid.getCols();
                } else {
                    cellX = static_cast<uint32_t>((originCol + colOffset) + static_cast<int>(m_grid.getCols()));
                }
                uint32_t cellY;
                if(originRow + rowOffest >= 0){
                    cellY = static_cast<uint32_t>(originRow + rowOffest) % m_grid.getRows();
                } else {
                    cellY = static_cast<uint32_t>((originRow + rowOffest) + static_cast<int>(m_grid.getRows()));
                }
                Cell newCell{cellX, cellY};
                m_live.insert(newCell);
                m_grid.birthCell(newCell);
            }
         }
         updateNursery();
    }

    void placeBigX(int posX, int posY){
        placeShape(posX, posY, 
            std::vector<std::vector<bool>> {
                {true,false,false,false,true},
                {false,true,false,true,false},
                {false,false,true,false,false},
                {false,true,false,true,false},
                {true,false,false,false,true}
            }
        );
    }

    void placeRing(int posX, int posY){
        placeShape(posX, posY,
            std::vector<std::vector<bool>> {
                {false, true, true, false },
                {true, false, false, true },
                {true, false, false, true },
                {false, true, true, false }
            });
    }
        
    void placeStar(int posX, int posY){
        placeShape(posX, posY, 
            std::vector<std::vector<bool>> {
                {false, true, false},
                {true, false, true},
                {false, true, false}
        });
    }


    void placeHLozenge(int posX, int posY){
        placeShape(posX, posY, 
            std::vector<std::vector<bool>> {
                {false, true, true, false},
                {true, false, false, true},
                {false, true, true, false}
            });
    }


    void placeVLozenge(int posX, int posY){
        placeShape(posX, posY, 
            std::vector<std::vector<bool>> {
                {false, true, false},
                {true, false, true},
                {true, false, true},
                {false, true, false}
        });
    }

    void placeColony(int x, int y){
        placeShape(x, y, 
            std::vector<std::vector<bool>> {
                {true,true},
                {true, true}
            }
        );
    }

    void placeSpinnerH(int x, int y){
         placeShape( x, y,
            std::vector<std::vector<bool>>{
                {true, true, true}
            }
         );
    }

    void placeSpinnerV(int x, int y){
         placeShape( x, y,
            std::vector<std::vector<bool>>{
                {true},
                {true},
                {true}
            }
         );
    }

    void placeGliderDR(int x, int y){
        placeShape(x,y,
           std::vector<std::vector<bool>>{
            {false, false, true},
            {true, false, true},
            {false, true, true}
           }
        );
    }

    void placeGliderUR(int x, int y){
        placeShape(x,y,
           std::vector<std::vector<bool>>{
            {false, true, true},
            {true, false, true},
            {false, false, true}
           }
        );
    }

    void placeGliderUL(int x, int y){
        placeShape(x,y,
            std::vector<std::vector<bool>>{
              {true, true, false},
              {true, false, true},
              {true, false, false}
            }
        );
    }

    void placeDot(int x, int y){
        placeShape(x, y, std::vector<std::vector<bool>>{
            {true}
        });
    }

    void placeGliderDL(int x, int y){
        placeShape(x,y,
            std::vector<std::vector<bool>>{
              {true, false, false},
              {true, false, true},
              {true, true, false}
            }
        );
    }

    void updateGridScale(int scale){
        if(m_at_max) {
            m_at_max = false;
        }
        auto potentialScale = static_cast<size_t>(scale);
        if(potentialScale == m_scale) return;
        m_scale = static_cast<size_t>(scale);
        m_paused = true;
        clear();
        auto requestedScale = m_sizes[m_scale -1];
        const uint32_t gridCols = 40*requestedScale;
        const uint32_t gridRows = 30*requestedScale;
        m_grid.updateDimensions(gridCols, gridRows);
        m_paused = false;
    }

    void updateToMax(){
        if(m_at_max) return;
        m_at_max = true;
        m_paused = true;
        clear();
        m_grid.updateDimensions(800, 600);
        m_paused = false;
    }

    uint8_t normX2Color(int mouseX){
      auto col = m_grid.colFromPosX(static_cast<uint32_t>(mouseX));
      auto ratio = static_cast<float>(col) / static_cast<float>(m_grid.getCols());
      return static_cast<uint8_t>(ratio * 255.f);
    }	

    uint8_t normY2Color(int mouseY){	
      auto row = m_grid.rowFromPosY(static_cast<uint32_t>(mouseY));
      auto ratio = static_cast<float>(row) / static_cast<float>(m_grid.getRows());
      return static_cast<uint8_t>(ratio * 255.f);
    }

    uint32_t getCols() {
        return m_grid.getCols();
    }

    uint32_t getRows() {
        return m_grid.getRows();
    }
    
    std::string getScale() {
        if(m_at_max){
            return "MAX";
        }
        return std::format("{}",m_scale);
    }

    private:
        Grid m_grid;
        std::set<Cell> m_live;
        std::set<Cell> m_nursery;   
        bool m_paused;
        size_t m_scale;
        bool m_at_max;
        const std::vector<uint32_t> m_sizes{1,2,4,5,10};
};
