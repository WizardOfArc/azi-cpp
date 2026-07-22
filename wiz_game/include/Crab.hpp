#pragma once

#include <print>

class Crab {
    public:

    static int crabs_created;

       explicit Crab(float x, float y, int hesitate):
       m_pos_x{x},
       m_pos_y{y},
       m_hesitation{hesitate}{
            m_live = true;
            m_updateTick = 0;
            crabs_created += 1;
            m_id = crabs_created;
            m_leap_factor = 30.f;
       }

       explicit Crab(){
          Crab(0,0,30);
       }

       void update(float targetX, float targetY){
           m_updateTick += 1;
           auto deltaX = targetX - m_pos_x;
           auto deltaY = targetY - m_pos_y;
           if(m_updateTick > m_hesitation){
                if(abs(deltaX) > abs(deltaY)){
                    m_pos_x += (m_leap_factor*deltaX/abs(deltaX)); 
                } else {
                    m_pos_y += (m_leap_factor*deltaY/abs(deltaY));
                }
                m_updateTick = 0;
           }
       }

       void end(){
            std::println("crab[{}] ended", m_id);
            m_live = false;
       }

       bool isLive(){
        return m_live;
       }

       float crabX(){
           return m_pos_x;
       }
    
       float crabY(){
           return m_pos_y;
       }
    
    private:
       bool m_live;
       float m_pos_x;
       float m_pos_y;
       int m_hesitation; 
       int m_updateTick;
       float m_leap_factor;
       int m_id;

};