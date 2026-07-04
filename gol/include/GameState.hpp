#pragma once
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>
#include "Grid.hpp"


struct CellRenderData {
    float radius;
    float scrnX;
    float scrnY;
};

class GameState {
    public:
        explicit GameState(uint32_t cols, uint32_t rows,uint32_t scrWidth, uint32_t scrHeight): m_grid{cols, rows, scrWidth, scrHeight} {
            m_paused = false;
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
            for(auto living : m_live ){
                auto neighbors = m_grid.getDeadNeighbors(living);
                for( const auto neighbor : neighbors){
                    m_nursery.insert(neighbor);
                }
            }
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
                }
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

        CellRenderData getCellRenderData(const Cell& cell){
            return CellRenderData{
                .radius=m_grid.getCellRadius(),
                .scrnX=static_cast<float>(m_grid.posX(cell.x)),
                .scrnY=static_cast<float>(m_grid.posY(cell.y))
            };
        }

    void placeColony(int x, int y){
        std::vector<std::vector<bool>> layout = {
            {true,true},
            {true, true}
        };
        placeShape(x, y, layout);
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
    }

    void placeBigX(int posX, int posY){
        /*
             10001
             01010
             00100
             01010
             10001
        */
        std::vector<std::vector<bool>> layout = {
            {true,false,false,false,true},
            {false,true,false,true,false},
            {false,false,true,false,false},
            {false,true,false,true,false},
            {true,false,false,false,true}
        };
        placeShape(posX, posY, layout);
    }

    void placeRing(int posX, int posY){
        /*
        */
        std::vector<std::vector<bool>> layout {
            {false, true, true, false },
            {true, false, false, true },
            {true, false, false, true },
            {false, true, true, false }
        };
        placeShape(posX, posY, layout);
    }
        
    // TODO: add code for placing gliders in each direction
    // it seems that any "dynamic" entity has something like UB when placed this way - will try other stable types to see if they work like colony and ring
    // so TODO: star and lozenges

    void placeStar(int posX, int posY){
        std::vector<std::vector<bool>> layout = {
            {false, true, false},
            {true, false, true},
            {false, true, false}
        };
        placeShape(posX, posY, layout);
    }


    void placeHLozenge(int posX, int posY){
        std::vector<std::vector<bool>> layout = {
            {false, true, true, false},
            {true, false, false, true},
            {false, true, true, false}
        };
        placeShape(posX, posY, layout);
    }


    void placeVLozenge(int posX, int posY){
        std::vector<std::vector<bool>> layout = {
            {false, true, false},
            {true, false, true},
            {true, false, true},
            {false, true, false}
        };
        placeShape(posX, posY, layout);
    }

    private:
        Grid m_grid;
        std::set<Cell> m_live;
        std::set<Cell> m_nursery;   
        bool m_paused;
};
