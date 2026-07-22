#pragma once


class Charger {
    public:
       explicit Charger(){}

       void buildCharge(){
            if(m_at_max) return;
            auto new_level = m_charge_level + 1.f;
            m_charge_level = new_level < 100.f ? new_level : 100.f;
            m_charging = true;
       }

       void discharge(){
          if(not m_charging) return;
          m_charge_level = 0.f;
          m_charging = false;
       }

       bool isCharging(){
          return m_charging;
       }

       float chargeLevel(){
          return m_charge_level;
       }

       float chargeLevelPercent(){
          return m_charge_level / 100.f; 
       }

    private:
       float m_charge_level;
       bool m_at_max;
       bool m_charging;
};