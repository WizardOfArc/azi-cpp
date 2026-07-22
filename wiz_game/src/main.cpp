#include "SFML/Audio/Music.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Angle.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "WizardState.hpp"
#include "Blast.hpp"
#include "Ray.hpp"
#include "Charger.hpp"

#include <cmath>
#include <cstddef>

int Blast::count = 0;

const sf::Color ageColorPallette[] = {
    sf::Color(255,255,255),
    sf::Color(255,225,255),
    sf::Color(255,195,255),
    sf::Color(255,165,255),
    sf::Color(255,135,255),
    sf::Color(255,105,255),
    sf::Color(255,75,255),
    sf::Color(255,45,255),
    sf::Color(255,15,255),
    sf::Color(255,0,255),
    sf::Color(255,0,225),
    sf::Color(255,0,195),
    sf::Color(255,0,165),
    sf::Color(255,0,135),
    sf::Color(255,0,105),
    sf::Color(255,0,75),
    sf::Color(255,0,45),
    sf::Color(255,0,15),
    sf::Color(255,0,0),
    sf::Color(255,0,0),
};

sf::Color blastColor(Blast &blast){
    auto color = ageColorPallette[blast.getLifePhase()];
    return color;
}

sf::Color chargeOrbColor(float charge){
    auto capped = charge > 100.f ? 100.f : charge;
    auto normalized = static_cast<size_t>(capped / 5.f);
    return ageColorPallette[20 - normalized];
}

float blastRadius(Blast &blast){
    return static_cast<float>(20 - blast.getLifePhase());
}


struct BeamDimensions {
    float magnitude;
    sf::Angle angle;
};

struct UnitVector {
    float x;
    float y;
};

void normalizeVect(sf::Vector2f posA, sf::Vector2f posB, BeamDimensions &dimensions){
    auto dx = posB.x - posA.x;
    auto dy = posB.y - posA.y;
    dimensions.magnitude = sqrt(dx*dx+dy*dy);
    auto angle = atan2f(dy, dx); 
    dimensions.angle = sf::radians(angle);
}

UnitVector normalize(float x1, float y1, float x2, float y2){
    auto dx = x2 - x1;
    auto dy = y2 - y1;
    auto mag = sqrt(dx*dx + dy*dy);
    return UnitVector{dx/mag, dy/mag};
};

void removeDead(std::vector<Blast> &toUpdate){
    std::vector<Blast> toKeep;
    for(auto &blast : toUpdate){
        if(blast.isLive()){
            toKeep.push_back(blast);
        }
    }
    toUpdate.clear();
    toUpdate = toKeep;
}

int main() {
    // TODO: add game state
    const sf::String title = "The Wizard Game";
    const uint32_t screenWidth = 1600;
    const uint32_t screenHeight = 1000;
    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), title);
    window.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("wizard_sprite.png")) { // not a mistake - currently using sprite texture as icon
        window.setIcon(icon);
    }

    sf::Font font("LeroyLettering.ttf");
    sf::Text text(font);

    sf::Texture spriteTexture("wizard_sprite.png");
    sf::Sprite wizard(spriteTexture);

    sf::Texture backgroundTexture("grass.png");
    backgroundTexture.setRepeated(true);
    sf::Sprite background(backgroundTexture, sf::IntRect{{0,0},{screenWidth, screenHeight}});
    
    // -- Sounds and Music ---
    sf::SoundBuffer blastBuffer("blast.wav");
    sf::Sound blastSound(blastBuffer);

    sf::SoundBuffer zapBuffer("zap.wav");
    sf::Sound zapSound(zapBuffer);

    sf::SoundBuffer chargeBuffer("charging.wav");
    sf::Sound chargeSound(chargeBuffer);

    sf::SoundBuffer footstepsBuffer("footstep.wav");
    sf::Sound footstepSound(footstepsBuffer);

    sf::SoundBuffer bounceBuffer("bounce.wav");
    sf::Sound bounceSound(bounceBuffer);
    bounceSound.setVolume(50.f);

    sf::Music music("WizGame.wav");
    music.setLooping(true);
    music.setVolume(100.f);
    music.play();
    // -- end sounds and music

    auto wizardXpos = screenWidth / 2;
    auto wizardYpos = screenHeight - 200; 
    int mousePositionX;
    int mousePositionY;
    float wizCenterX;
    float wizCenterY;
    sf::Vector2i wizardCenter;
    sf::CircleShape glow;

    sf::Vector2f scepterCenter;
    sf::Vector2f mousePosition;

    // float scepterX;
    // float scepterY;
    std::vector<Blast> blasts;

    Charger charger;
    WizardState wizState;

    
    BeamDimensions bDim;
    Ray ray;
    sf::ConvexShape beam(4);
    beam.setOrigin({-20.f, 0.f});
    beam.setPoint(0, {-20.f, 0.f});
    beam.setPoint(1, {0.f, -20.f});
    beam.setPoint(2, {20.f, 0.f});
    beam.setPoint(3, {0.f, 20.f});

    float gravity = 0.1f;
    float dampening = 0.7f;
    float speed = 5.f;

    while (window.isOpen()){
        int orbVerticalOffset = 45;
        auto wizardBox = wizard.getGlobalBounds();
        wizCenterX = (static_cast<float>(wizardBox.size.x) / 2) + wizardBox.position.x;
        wizCenterY = (static_cast<float>(wizardBox.size.y) / 2) + wizardBox.position.y;
        wizardCenter.x = static_cast<int>(wizCenterX);
        wizardCenter.y = static_cast<int>(wizCenterY);
        auto wizardWidth = wizardBox.size.x;
        auto wizardHeight = wizardBox.size.y;
        if(static_cast<float>(mousePositionX) >= wizCenterX){
            wizState.setDirection(WizardDirection::RIGHT);
        } else {
            wizState.setDirection(WizardDirection::LEFT);
        }
        normalizeVect(scepterCenter, mousePosition, bDim);

        while (const std::optional event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()){
                window.close();
            } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()){
                auto scancode = keyPressed->scancode;
                switch (scancode) {
                    case sf::Keyboard::Scancode::Escape:
                       window.close();
                       break;
                    case sf::Keyboard::Scancode::Q:
                       window.close();
                       break;
                    case sf::Keyboard::Scancode::Space:
                       charger.buildCharge();
                       if(not chargeSound.isLooping()){
                          chargeSound.setLooping(true);
                          chargeSound.play();
                       }
                       break;
                    case sf::Keyboard::Scancode::Right:
                       if(wizardBox.position.x + wizardBox.size.x <= screenWidth - 5){
                            wizardXpos += 5;
                            footstepSound.play();
                       }
                       break;
                    case sf::Keyboard::Scancode::Left:
                       if(wizardBox.position.x >= 5){
                            wizardXpos -= 5;
                            footstepSound.play();
                       }
                       break;
                    case sf::Keyboard::Scancode::Down:
                       if(wizardBox.position.y + wizardBox.size.y <= screenHeight - 5){
                            wizardYpos += 5;
                            footstepSound.play();
                       }
                       break;
                    case sf::Keyboard::Scancode::Up:
                       if(wizardBox.position.y >= 5){
                            wizardYpos -= 5;
                            footstepSound.play();
                       }
                       break;
                    default:
                       // do nothing
                }
            } else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()){
                auto scancode = keyReleased->scancode;
                auto x_dimension = bDim.magnitude * charger.chargeLevelPercent();
                switch (scancode){
                    case sf::Keyboard::Scancode::Space:
                       // TODO: play zzap! sound if ray not active yet
                       beam.setPoint(2, {x_dimension, 0.f});
                       beam.setRotation(bDim.angle);
                       ray.setTTL(500);
                       beam.setFillColor(chargeOrbColor(charger.chargeLevel()));
                       chargeSound.stop();
                       chargeSound.setLooping(false);
                       zapSound.play();
                       break;
                    default: 
                       // do nothing
                }
            } else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                mousePositionX = mouseMoved->position.x;
                mousePositionY = mouseMoved->position.y;
                mousePosition.x = static_cast<float>(mouseMoved->position.x);
                mousePosition.y = static_cast<float>(mouseMoved->position.y);
            } else if (const auto *mouseButtonClicked = event->getIf<sf::Event::MouseButtonPressed>()) {
                [[maybe_unused]] auto mbc = mouseButtonClicked;
                auto tapPosition = mbc->position;
                auto vector = normalize(scepterCenter.x, scepterCenter.y, static_cast<float>(tapPosition.x), static_cast<float>(tapPosition.y));
                blastSound.play();
                blasts.push_back(Blast{scepterCenter.x, scepterCenter.y, vector.x*speed, vector.y*speed});
            }
        }

        removeDead(blasts);
        window.clear(sf::Color::Black);
        window.draw(background);
        text.setString(std::format("mouse: ({},{})", mousePositionX, mousePositionY));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(255,0,255));
        window.draw(text);
        wizard.setPosition({static_cast<float>(wizardXpos), static_cast<float>(wizardYpos)});
        if(wizState.getDirection() == WizardDirection::LEFT){
            // flip to reverse
            wizard.setTextureRect(sf::IntRect({static_cast<int>(wizardWidth), 0}, {static_cast<int>(-wizardWidth), static_cast<int>(wizardHeight)}));
            scepterCenter.x = static_cast<float>(wizardCenter.x) - 55;
            scepterCenter.y = static_cast<float>(wizardCenter.y - orbVerticalOffset);
        } else {
            wizard.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(wizardWidth), static_cast<int>(wizardHeight)}));
            scepterCenter.x = static_cast<float>(wizardCenter.x) + 55; 
            scepterCenter.y = static_cast<float>(wizardCenter.y - orbVerticalOffset);
        }

        glow.setPosition(scepterCenter);
        if(charger.isCharging()){
            auto chargeLevel = charger.chargeLevel();
            glow.setFillColor(chargeOrbColor(chargeLevel));
            glow.setRadius(chargeLevel / 5);
            glow.setOrigin({chargeLevel / 5, chargeLevel / 5});
            window.draw(glow);
        }
        window.draw(wizard);
       
        beam.setPosition(scepterCenter);
        beam.setFillColor(chargeOrbColor(charger.chargeLevel()));
        if(ray.live()){
            window.draw(beam);
            ray.update();
            if(!ray.live()){
                charger.discharge();
            }
        }
        for(auto &blast : blasts){
            // auto orb = blastOrbs[blast.getIdx()];
            auto bX = blast.getX();
            auto bY = blast.getY();
            if(blast.isLive() && bX < 0.f){
                blast.end();
            }
            if(blast.isLive() && bX > static_cast<float>(screenWidth)){
                blast.end();
            }
            if(blast.isLive() && bY > static_cast<float>(screenHeight)){
                blast.bounceY(screenHeight, dampening);
                bounceSound.play();
            }
            if(blast.isLive() && bY < 0.f){
                blast.end();
            }
            if(blast.isLive()){
                auto brad = blastRadius(blast);
                sf::CircleShape orb{brad};
                orb.setOrigin({brad, brad});
                orb.setFillColor(blastColor(blast));
                orb.setPosition({bX, bY});
                blast.update(gravity);
                window.draw(orb);
            }

        }

        window.display();
    }
    music.stop();
    return 0;

}