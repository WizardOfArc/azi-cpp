#pragma once

#include <vector>

#include "Charger.hpp"
#include "Crab.hpp"
#include "Blast.hpp"
#include "Ray.hpp"
#include "SFML/System/Vector2.hpp"
#include "WizardState.hpp"



struct UnitVector {
    float x;
    float y;
};


UnitVector normalize(sf::Vector2f source, sf::Vector2i target){
    auto x1 = source.x;
    auto y1 = source.y;
    auto x2 = static_cast<float>(target.x);
    auto y2 = static_cast<float>(target.y);
    auto dx = x2 - x1;
    auto dy = y2 - y1;
    auto mag = sqrt(dx*dx + dy*dy);
    return UnitVector{dx/mag, dy/mag};
};


struct BeamDimensions {
    float magnitude;
    sf::Angle angle;
};

void normalizeVect(sf::Vector2f posA, sf::Vector2f posB, BeamDimensions &dimensions){
    auto dx = posB.x - posA.x;
    auto dy = posB.y - posA.y;
    dimensions.magnitude = sqrt(dx*dx+dy*dy);
    auto angle = atan2f(dy, dx); 
    dimensions.angle = sf::radians(angle);
}



class GameState {
    public:
        explicit GameState(int gameWidth, int gameHeight, float speed):
        m_gameWidth(gameWidth), 
        m_gameHeight(gameHeight),
        m_blast_speed(speed){
           m_playing = true;
           m_level = 1;
           m_gravity = 0.1f;
           m_dampening = 0.7f;
           m_score = 0;
           m_score_at_level = 0;
        } 

        bool isRunning(){
            return m_playing;
        }

        int getScore(){
            return m_score;
        }

        void incrementScore(){
            m_score += 1;
            m_score_at_level += 1;
            if(m_score_at_level >= 10){
                incrementLevel();
                m_score_at_level = 0;
            }
        }

        int getWizardHealth(){
            return m_wizard.getHealth();
        }

        void hurtWizard(int hits){
            m_wizard.takeDamage(hits);
        }

        void prune(){
            std::vector<Blast> blastsToKeep;
            for(auto &blast: m_blasts){
                if(blast.isLive()){
                    blastsToKeep.push_back(blast);
                }
            }
            m_blasts.clear();
            m_blasts = blastsToKeep;

            std::vector<Crab> crabsToKeep;
            for(auto &crab: m_crabs){
                if(crab.isLive()){
                    crabsToKeep.push_back(crab);
                }
            }
            m_crabs.clear();
            m_crabs = crabsToKeep;

        }

        void incrementLevel(){
            m_level += 1;
        }

        int getLevel(){
            return m_level;
        }

        void spawnCrabs(){
            if(not m_crabs.empty()){
                return;
            }
            for(int i=0; i < m_level; i++){
                auto rndX = static_cast<float>(rand() % m_gameWidth);
                auto rndY = static_cast<float>(rand() % m_gameHeight);
                auto rndHesitation = (rand() + 15) % 45;
                m_crabs.push_back(Crab{rndX, rndY, rndHesitation});
            }

        }

        void spawnBlast(sf::Vector2f source, sf::Vector2i target){
           auto uVector = normalize(source,target);
           m_blasts.push_back(Blast{source.x, source.y, uVector.x*m_blast_speed, uVector.y*m_blast_speed});
        }

        void update(){
             if(not m_wizard.isLive()){
                m_playing = false;
             }
        }

        float getGravity(){
            return m_gravity;
        }

        float getDampening(){
            return m_dampening;
        }

        void updateBeam(sf::Vector2f source, sf::Vector2f target){
            normalizeVect(source, target, m_bDim);
        }

        float getBeamMagnitude(){
            return m_bDim.magnitude;
        }

        sf::Angle getBeamAngle(){
            return m_bDim.angle;
        }

        float getChargeLevel(){
            return m_charger.chargeLevel();
        }

        void discharge(){
            m_charger.discharge();
        }

        bool isCharging(){
            return m_charger.isCharging();
        }

        void chargeUp(){
            m_charger.buildCharge();
        }

        float getBeamXDimension(){
            return m_bDim.magnitude * m_charger.chargeLevelPercent();
        }

        void startBeam(){
            m_beam.setTTL(500);
        }

        bool isBeamLive(){
            return m_beam.live();
        }

        void updateBeam(){
            m_beam.update();
        }

        void turnWizardLeft(){
            m_wizard.setDirection(WizardDirection::LEFT);
        }

        void turnWizardRight(){
            m_wizard.setDirection(WizardDirection::RIGHT);
        }

        bool wizardFacingLeft(){
            return m_wizard.getDirection() == WizardDirection::LEFT;
        }

        std::vector<Crab> &getCrabs(){
            return m_crabs; // will this return a reference or a copy?
        }

        std::vector<Blast> &getBlasts(){
            return m_blasts;
        }

    private:
       bool m_playing;
       int m_level;
       int m_score;
       std::vector<Crab> m_crabs;
       std::vector<Blast> m_blasts;
       WizardState m_wizard;
       Charger m_charger;
       int m_gameWidth;
       int m_gameHeight;
       float m_blast_speed;
       float m_gravity;
       float m_dampening;
       BeamDimensions m_bDim;
       Ray m_beam;
       int m_score_at_level;

};