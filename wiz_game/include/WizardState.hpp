#pragma once


enum class WizardDirection {
   LEFT,
   RIGHT
};

class WizardState {
    public:
        explicit WizardState(){
            _dir = WizardDirection::RIGHT;
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
   private:
       WizardDirection _dir;
};