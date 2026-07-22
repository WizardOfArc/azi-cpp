#pragma once

#include <chrono>
#include <ratio>

class Ray {
   public:
     void setTTL(int ttl){
        m_active = true;
        m_time_left = std::chrono::duration<int, std::milli>(ttl);
        m_update_time = std::chrono::system_clock::now();
     }

     explicit Ray(int ttl, bool active):
     m_active(active)
     {
        setTTL(ttl);
     }

     explicit Ray(){
        Ray(0, false);
     }


     void update(){
        if(m_active){
            auto now = std::chrono::system_clock::now();
            auto since = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_update_time);
            if(since > m_time_left){
                m_active = false;
                m_time_left = std::chrono::duration<int, std::milli>(0);
            } else if (m_time_left == std::chrono::duration<int, std::milli>(0)){
                m_active = false;
            } else {
                m_time_left = m_time_left-since;
            }
            m_update_time = now;
        }
     }

     bool live(){
        return m_active;
     }

     void end(){
        m_active = false;
        m_time_left = std::chrono::duration<int, std::milli>(0);
        m_update_time = std::chrono::system_clock::now();
     }

   private:
     bool m_active;
     std::chrono::milliseconds m_time_left;
     std::chrono::time_point<std::chrono::system_clock> m_update_time;

};