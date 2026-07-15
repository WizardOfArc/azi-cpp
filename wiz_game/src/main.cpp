#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/VideoMode.hpp"
#include <SFML/Graphics.hpp>

#include "WizardState.hpp"
#include "Blast.hpp"

#include <cmath>
#include <print>

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
    sf::Color(235,0,225),
    sf::Color(215,0,195),
    sf::Color(195,0,165),
    sf::Color(175,0,135),
    sf::Color(155,0,105),
    sf::Color(135,0,75),
    sf::Color(115,0,45),
    sf::Color(95,0,15),
    sf::Color(75,0,0),
    sf::Color(55,0,0),
};

sf::Color blastColor(Blast &blast){
    return ageColorPallette[blast.getLifePhase()];
}

float blastRadius(Blast &blast){
    return static_cast<float>(20 - blast.getLifePhase());
}

struct UnitVector {
    float x;
    float y;
};

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
    const sf::String title = "The Wizard Game";
    const uint32_t screenWidth = 800;
    const uint32_t screenHeight = 600;
    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), title);
    window.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("wizard_sprite.png")) {
        window.setIcon(icon);
    }

    sf::Font font("LeroyLettering.ttf");
    sf::Text text(font);

    sf::Texture spriteTexture("wizard_sprite.png");
    sf::Sprite wizard(spriteTexture);

    auto wizardXpos = screenWidth / 2;
    auto wizardYpos = screenHeight - 200; 
    int mousePositionX;
    int mousePositionY;
    float wizCenterX;
    float wizCenterY;
    sf::Vector2i wizardCenter;
    sf::CircleShape glow;
    glow.setFillColor(sf::Color(255,78,255));
    glow.setRadius(20);
    glow.setOrigin({20.f,20.f});

    float scepterX;
    float scepterY;
    std::vector<Blast> blasts;

    bool hasGlow;
    WizardState wizState;

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
                       // add glow
                       hasGlow = true;
                       break;
                    case sf::Keyboard::Scancode::Right:
                       if(wizardBox.position.x + wizardBox.size.x <= screenWidth - 5){
                            wizardXpos += 5;
                       }
                       break;
                    case sf::Keyboard::Scancode::Left:
                       if(wizardBox.position.x >= 5){
                            wizardXpos -= 5;
                       }
                       break;
                    case sf::Keyboard::Scancode::Down:
                       if(wizardBox.position.y + wizardBox.size.y <= screenHeight - 5){
                            wizardYpos += 5;
                       }
                       break;
                    case sf::Keyboard::Scancode::Up:
                       if(wizardBox.position.y >= 5){
                            wizardYpos -= 5;
                       }
                       break;
                    default:
                       // do nothing
                }
            } else if (const auto *keyReleased = event->getIf<sf::Event::KeyReleased>()){
                auto scancode = keyReleased->scancode;
                auto vector = normalize(scepterX, scepterY, static_cast<float>(mousePositionX), static_cast<float>(mousePositionY));
                float speed = 1.f;
                switch (scancode){
                    case sf::Keyboard::Scancode::Space:
                       // remove glow
                       hasGlow = false;
                       // release blast (create and add to vector)
                       blasts.push_back(Blast{scepterX, scepterY, vector.x*speed, vector.y*speed});
                       break;
                    default: 
                       // do nothing
                }
            } else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
                mousePositionX = mouseMoved->position.x;
                mousePositionY = mouseMoved->position.y;
            }
        }

        removeDead(blasts);
        window.clear(sf::Color::Black);

        text.setString(std::format("mouse: ({},{})", mousePositionX, mousePositionY));
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(255,0,255));
        window.draw(text);
        wizard.setPosition({static_cast<float>(wizardXpos), static_cast<float>(wizardYpos)});
        if(wizState.getDirection() == WizardDirection::LEFT){
            // flip to reverse
            wizard.setTextureRect(sf::IntRect({static_cast<int>(wizardWidth), 0}, {static_cast<int>(-wizardWidth), static_cast<int>(wizardHeight)}));
            scepterX = static_cast<float>(wizardCenter.x) - 55;
            scepterY = static_cast<float>(wizardCenter.y - orbVerticalOffset);
        } else {
            wizard.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(wizardWidth), static_cast<int>(wizardHeight)}));
            scepterX = static_cast<float>(wizardCenter.x) + 55; 
            scepterY = static_cast<float>(wizardCenter.y - orbVerticalOffset);
        }
        glow.setPosition({scepterX, scepterY});
        if(hasGlow){
            window.draw(glow);
        }
        window.draw(wizard);
        for(auto &blast : blasts){
            // auto orb = blastOrbs[blast.getIdx()];
            auto bX = blast.getX();
            auto bY = blast.getY();
            if(blast.isLive() && bX < 0.f){
                blast.bounceX(0.f);
            }
            if(blast.isLive() && bX > static_cast<float>(screenWidth)){
                blast.bounceX(screenWidth);
            }
            if(blast.isLive() && bY > static_cast<float>(screenHeight)){
                blast.bounceY(screenHeight);
            }
            if(blast.isLive() && bY < 0.f){
                blast.bounceY(0.f);
            }
            if(blast.isLive()){
                auto brad = blastRadius(blast);
                sf::CircleShape orb{brad};
                orb.setOrigin({brad, brad});
                orb.setFillColor(blastColor(blast));
                orb.setPosition({bX, bY});
                blast.update();
                window.draw(orb);
            }

        }

        window.display();
    }
    return 0;

}