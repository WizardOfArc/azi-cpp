#pragma once

#include <chrono>

#include <print>

const float LIFETIME = 2000;

class Blast {
    public:
       static int count;

       Blast(float posX, float posY, float velX, float velY):
       m_initial_pos_x(posX),
       m_initial_pos_y(posY),
       m_v_x(velX),
       m_v_y(velY)
       {
            auto now = std::chrono::system_clock::now();
            m_birth_time = now;
            m_update_time = now;
            m_live = true;
            count += 1;
            m_id = count;
            m_pos_x = posX;
            m_pos_y = posY;
       };

       float getX(){
          return m_pos_x;
       }

       void bounceX(float x){
           m_pos_x = x;
           m_initial_pos_x = x;
           m_v_x = -(m_v_x);
       }

       void bounceY(float y){
           m_pos_y = y;
           m_initial_pos_y = y;
           m_v_y = -(m_v_y);
       }

       float getY(){
           return m_pos_y;
       }

       void update(){
           auto now = std::chrono::system_clock::now();
           auto age_dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_birth_time);
           float age = static_cast<float>(age_dur.count());
        //    m_pos_x = m_initial_pos_x + age*m_v_x;
        //    m_pos_y = m_initial_pos_y + age*m_v_y;
           auto since_dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_update_time);
           float time_since_update = static_cast<float>(since_dur.count());
           m_pos_x += time_since_update*m_v_x;
           m_pos_y += time_since_update*m_v_y;
           m_update_time = now;
           if(age > LIFETIME){
              m_live = false;
              return;
           }
           auto ratio = age / LIFETIME;
           auto percent = static_cast<uint8_t>(ratio * 100);
           m_phase_of_life = percent / 5;
       }

       void end(){
          m_live = false;
       }

       bool isLive(){
          return m_live;
       }

       size_t getIdx(){
           return static_cast<size_t>(m_id - 1);
       }

       uint8_t getLifePhase(){
          return m_phase_of_life;
       }

    private:
       float m_initial_pos_x;
       float m_initial_pos_y;
       float m_pos_x;
       float m_pos_y;
       float m_v_x;
       float m_v_y;
       bool m_live;
       int m_id;
       uint8_t m_phase_of_life;
       std::chrono::time_point<std::chrono::system_clock> m_birth_time;
       std::chrono::time_point<std::chrono::system_clock> m_update_time;
};
