#pragma once
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
        } 

        uint32_t rightMost(){
            return m_grid.maxPosX();
        }

        uint32_t bottomMost(){
            return m_grid.maxPosY();
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
        auto left = m_grid.colFromPosX(static_cast<uint32_t>(x));
        auto top = m_grid.rowFromPosY(static_cast<uint32_t>(y));
        if(left < 0){
            left = 0;
        } 
        if(top < 0){
            top = 0;
        }
        auto right = left + 1;
        if(right >= m_grid.getCols()){
            right = m_grid.getCols() - 1;
        }
        auto bottom = top + 1;
        if(bottom >= m_grid.getRows()){
            bottom = m_grid.getRows();
        }
        std::vector<uint32_t> Xs{left,right};
        std::vector<uint32_t> Ys{top, bottom};
        for(auto new_y: Ys){
            for(auto new_x: Xs){
                Cell cell{new_x,new_y};
                m_live.insert(cell);
                m_grid.birthCell(cell);
            }
        }
    }

    void placeSpinner(int x, int y){
        auto centerX = m_grid.colFromPosX(static_cast<uint32_t>(x));
        auto spinnerY = m_grid.rowFromPosY(static_cast<uint32_t>(y));
        auto left = centerX - 1;
        if(left < 0){
            left = m_grid.getCols() - 1;
        } 
        auto right = centerX + 1;
        if(right == m_grid.getCols()){
            right = 0;
        }
        std::vector<uint32_t> Xs{left, centerX, right};
        for(auto spinnerX : Xs ){
            Cell newCell{spinnerX, spinnerY};
            m_live.insert(newCell);
            m_grid.birthCell(newCell);
        }

    }

    void placeRing(int posX, int posY){
        /*
        diagram of  what we want
          
            col2Rt -+
                    |
          colRt---+ |
                  v v
              0 1 1 0      rowUp
              1 0 0 1  <-- originY
              1 0 0 1      rowDown
              0 1 1 0      row2Down
              ^ ^ 
              | +--originX
              |
              +--- colLt
            ----------
        */
        
        auto originX = m_grid.colFromPosX(static_cast<uint32_t>(posX));
        auto colLt = originX == 0 ? m_grid.getCols() - 1 : originX - 1;
        auto colRt = (originX + 1) % m_grid.getCols();
        auto col2Rt = (originX + 2) % m_grid.getCols();

        auto originY = m_grid.rowFromPosY(static_cast<uint32_t>(posY));
        auto rowUp = originY == 0 ? m_grid.getRows() - 1 : originY - 1;
        auto rowDown = (originY + 1) % m_grid.getRows();
        auto row2Down = (originY + 2) % m_grid.getRows();

        std::vector<Cell> toBirth{
            Cell{originX, rowUp},
            Cell{colRt, rowUp},
            Cell{colLt, originY},
            Cell{col2Rt, originY},
            Cell{colLt, rowDown},
            Cell{col2Rt, rowDown},
            Cell{originX, row2Down},
            Cell{colRt, row2Down},
        };
        for(auto newCell: toBirth){
            m_live.insert(newCell);
            m_grid.birthCell(newCell);
        }
    }
        
    // TODO: add code for placing gliders in each direction
    // it seems that any "dynamic" entity has something like UB when placed this way - will try other stable types to see if they work like colony and ring
    // so TODO: star and lozenges

    void placeStar(int posX, int posY){
        auto originX = m_grid.colFromPosX(static_cast<uint32_t>(posX));
        auto colLt = originX - 1;
        if(colLt < 0){
            colLt = m_grid.getCols() - 1;
        }
        auto colRt = (originX + 1) % m_grid.getCols();
        auto originY = m_grid.rowFromPosY(static_cast<uint32_t>(posY));
        auto rowUp = originY - 1;
        if(rowUp < 0){
            rowUp = m_grid.getRows() - 1;
        }
        auto rowDown = (originY + 1) % m_grid.getRows();
        /*
            star:
            0 1 0
            1 0 1
            0 1 0
        */
        std::vector<Cell> newCells{
            Cell{colLt, originY},
            Cell{originX, rowDown},
            Cell{originX, rowUp},
            Cell{colRt, originY}
        };

        for(auto newCell: newCells){
            m_live.insert(newCell);
            m_grid.birthCell(newCell);
        }
    }


    void placeHLozenge(int posX, int posY){
        auto originX = m_grid.colFromPosX(static_cast<uint32_t>(posX));
        auto colLt = originX - 1;
        if(colLt < 0){
            colLt = m_grid.getCols() - 1;
        }
        auto colRt = (originX + 1) % m_grid.getCols();
        auto col2Rt = (originX + 2) % m_grid.getCols();

        auto originY = m_grid.rowFromPosY(static_cast<uint32_t>(posY));
        auto rowUp = originY - 1;
        if(rowUp < 0){
            rowUp = m_grid.getRows() - 1;
        }
        auto rowDown = (originY + 1) % m_grid.getRows();

        /*
            horiz lozenge:
            0 1 1 0
            1 0 0 1
            0 1 1 0
        */
        std::vector<Cell> newCells{
            Cell{colLt, originY},
            Cell{originX, rowUp},
            Cell{originX, rowDown},
            Cell{colRt, rowUp},
            Cell{colRt, rowDown},
            Cell{col2Rt, originY},
        };

        for(auto newCell: newCells){
            m_live.insert(newCell);
            m_grid.birthCell(newCell);
        }
    }


    void placeVLozenge(int posX, int posY){
        auto originX = m_grid.colFromPosX(static_cast<uint32_t>(posX));
        auto colLt = originX - 1;
        if(colLt < 0){
            colLt = m_grid.getCols() - 1;
        }
        auto colRt = (originX + 1) % m_grid.getCols();

        auto originY = m_grid.rowFromPosY(static_cast<uint32_t>(posY));
        auto rowUp = originY - 1;
        if(rowUp < 0){
            rowUp = m_grid.getRows() - 1;
        }
        auto rowDown = (originY + 1) % m_grid.getRows();
        auto row2Down = (originY + 2) % m_grid.getRows();


        /*
            vert lozenge:
            0 1 0
            1 0 1
            1 0 1
            0 1 0
        */
        std::vector<Cell> newCells{
            Cell{colLt, originY},
            Cell{colLt, rowDown},
            Cell{originX, rowUp},
            Cell{originX, row2Down},
            Cell{colRt, originY},
            Cell{colRt, rowDown}
        };

        for(auto newCell: newCells){
            m_live.insert(newCell);
            m_grid.birthCell(newCell);
        }
    }

    private:
        Grid m_grid;
        std::set<Cell> m_live;
        std::set<Cell> m_nursery;   
};
