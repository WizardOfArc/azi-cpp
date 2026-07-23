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
#include "Crab.hpp"
#include "WizgameColors.hpp"
#include "GameState.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <print>

int Blast::count = 0;
int Crab::crabs_created = 0;

sf::Color blastColor(Blast &blast){
    auto phase = blast.getLifePhase();
    auto color = wizgame::ageColor(phase);
    return color;
}


float blastRadius(Blast &blast){
    return static_cast<float>(20 - blast.getLifePhase());
}



int main() {
    // TODO: add game state
    const sf::String title = "The Wizard Game";
    const uint32_t screenWidth = 1600;
    const uint32_t screenHeight = 1000;
    GameState state{screenWidth, screenHeight, 5.f };
    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), title);
    window.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("wizard_sprite.png")) { // not a mistake - currently using sprite texture as icon
        window.setIcon(icon);
    }

    sf::Font font("LeroyLettering.ttf");
    sf::Text text(font);
    text.setOrigin({300, 50});

    sf::Texture wizardTexture("wizard_sprite.png");
    sf::Sprite wizard(wizardTexture);

    sf::Texture spriteSheetTexture("sprite_sheet.png");

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
    float wizCenterX;
    float wizCenterY;
    sf::Vector2i wizardCenter;
    sf::CircleShape glow;

    sf::Vector2f scepterCenter;
    sf::Vector2f mousePosition;

    std::vector<Blast> blasts;

    std::vector<Crab> crabs;


    
    sf::ConvexShape beam(4);
    beam.setOrigin({-20.f, 0.f});
    beam.setPoint(0, {-20.f, 0.f});
    beam.setPoint(1, {0.f, -20.f});
    beam.setPoint(2, {20.f, 0.f});
    beam.setPoint(3, {0.f, 20.f});


    while (window.isOpen()){
        int orbVerticalOffset = 45;
        auto wizardBox = wizard.getGlobalBounds();
        wizCenterX = (static_cast<float>(wizardBox.size.x) / 2) + wizardBox.position.x;
        wizCenterY = (static_cast<float>(wizardBox.size.y) / 2) + wizardBox.position.y;
        wizardCenter.x = static_cast<int>(wizCenterX);
        wizardCenter.y = static_cast<int>(wizCenterY);
        auto wizardWidth = wizardBox.size.x;
        auto wizardHeight = wizardBox.size.y;


        if(mousePosition.x >= wizCenterX){
            state.turnWizardRight();
        } else {
            state.turnWizardLeft();
        }
        state.updateBeam(scepterCenter, mousePosition);

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
                       state.chargeUp();
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
                switch (scancode){
                    case sf::Keyboard::Scancode::Space:
                       // TODO: play zzap! sound if ray not active yet
                       beam.setPoint(2, {state.getBeamXDimension(), 0.f});
                       beam.setRotation(state.getBeamAngle());
                       state.startBeam();
                       beam.setFillColor(wizgame::chargeColor(state.getChargeLevel()));
                       chargeSound.stop();
                       chargeSound.setLooping(false);
                       zapSound.play();
                       break;
                    default: 
                       // do nothing
                }
            } else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                mousePosition.x = static_cast<float>(mouseMoved->position.x);
                mousePosition.y = static_cast<float>(mouseMoved->position.y);
            } else if (const auto *mouseButtonClicked = event->getIf<sf::Event::MouseButtonPressed>()) {
                [[maybe_unused]] auto mbc = mouseButtonClicked;
                auto tapPosition = mbc->position;
                blastSound.play();
                state.spawnBlast(scepterCenter, tapPosition);
            }
        }
        state.prune();
        state.spawnCrabs();
        window.clear(sf::Color::Black);
        window.draw(background);
        window.draw(text);
        wizard.setPosition({static_cast<float>(wizardXpos), static_cast<float>(wizardYpos)});
        if(state.wizardFacingLeft()){
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
        if(state.isCharging()){
            auto chargeLevel = state.getChargeLevel();
            glow.setFillColor(wizgame::chargeColor(chargeLevel));
            glow.setRadius(chargeLevel / 5);
            glow.setOrigin({chargeLevel / 5, chargeLevel / 5});
            window.draw(glow);
        }
        window.draw(wizard);
       
        beam.setPosition(scepterCenter);
        auto chargeColor = wizgame::chargeColor(state.getChargeLevel());
        beam.setFillColor(chargeColor);
        if(state.isBeamLive()){
            window.draw(beam);
            state.updateBeam();
            if(!state.isBeamLive()){
                state.discharge();
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
                blast.bounceY(screenHeight, state.getDampening());
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
                blast.update(state.getGravity());
                window.draw(orb);
            }

        }
        for(auto &a_crab: crabs){
            if(a_crab.isLive()){
                sf::Sprite crab(spriteSheetTexture, sf::IntRect({137, 267}, {128,92}));
                crab.setOrigin({64, 46});
                crab.setPosition({a_crab.crabX(), a_crab.crabY()});
                a_crab.update(static_cast<float>(wizardCenter.x),static_cast<float>(wizardCenter.y));
                window.draw(crab);

                // check for collisions with beam
                auto box = crab.getGlobalBounds();
                auto beamBox = beam.getGlobalBounds();
                auto intersection_option = box.findIntersection(beamBox);
                if(intersection_option && state.getChargeLevel() > 0){
                    a_crab.end();
                }

                auto wizardCollide = box.findIntersection(wizardBox);
                if(wizardCollide){
                    text.setString("OUCH!!!");
                    text.setFillColor(sf::Color::Red);
                    text.setStyle(
                        sf::Text::Bold |
                        sf::Text::Underlined
                    );
                    text.setCharacterSize(150);
                    text.setPosition({static_cast<float>(screenWidth)/2, static_cast<float>(screenHeight)/2});
                }

                for(auto &blast : blasts){
                    auto bx = blast.getX();
                    auto by = blast.getY();
                    if(box.contains({bx, by})){
                        a_crab.end();
                        blast.end();
                    }
                }

            }
        }
        
        // TODO figure out collisions


        // crab.setTextureRect(sf::IntRect{{static_cast<int>(mousePosition.x), static_cast<int>(mousePosition.y)}, {128, 50}});

        window.display();
    }
    music.stop();
    return 0;

}