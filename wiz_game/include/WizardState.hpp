#pragma once


enum class WizardDirection {
   LEFT,
   RIGHT
};

class WizardState {
    public:
        explicit WizardState(){
            _dir = WizardDirection::RIGHT;
            m_health = 100;
        }
        void setDirection(WizardDirection dir){
           _dir = dir;
        }
        void toggleDir(){
            if(_dir == WizardDirection::LEFT){
                _dir = WizardDirection::RIGHT;
            } else if(_dir == WizardDirection::RIGHT){
                _dir = WizardDirection::LEFT;
            }
        }
        WizardDirection getDirection(){
            return _dir;
        }

        int getHealth(){
            return m_health;
        }

        void takeDamage(int damage){
            int newHealth = m_health - damage;
            m_health = newHealth >= 0 ? newHealth : 0;
        }

        bool isLive(){
            return m_health > 0;
        }


   private:
       WizardDirection _dir;
       int m_health;
};