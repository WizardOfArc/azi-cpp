#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Mouse.hpp"
#include <SFML/Graphics.hpp>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <windows.h>
#include <cstdlib>

#include "GameState.hpp"


int main() {
	size_t sizeIdx = 3;

    // create the window and set up
    const sf::String title = "Azi's Game of Life";
	const uint32_t screenWidth = 800;
	const uint32_t screenHeight = 600;
    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), title);

	GameState gs{screenWidth, screenHeight, sizeIdx};
    window.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("icon.png")) {
        window.setIcon(icon);
    }

	sf::Font font("LeroyLettering.ttf");
	sf::Text text(font);
	text.setString(std::format("It Lives! scale:{} ({}X{})", sizeIdx, gs.getCols(), gs.getRows()));
	sf::Color bannerColor(0, 200, 125);
	text.setFillColor(bannerColor);
	text.setCharacterSize(24);
	text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	text.setPosition({150.f, 0.f});

	sf::RectangleShape sideBorder;
	sideBorder.setSize({
		static_cast<float>(screenWidth - gs.rightMost()+1 ),
		static_cast<float>(screenHeight) 
	});
	sideBorder.setPosition({static_cast<float>(gs.rightMost()+1), 0});
	sideBorder.setFillColor(sf::Color::Red);

	sf::RectangleShape bottomBorder;
	bottomBorder.setSize({
		static_cast<float>(screenWidth),
		static_cast<float>(screenHeight - gs.bottomMost()+1) 
	});
	bottomBorder.setPosition({0, static_cast<float>(gs.bottomMost()+1)});
	bottomBorder.setFillColor(sf::Color::Red);


	sf::Text buttonPressed(font);
	buttonPressed.setCharacterSize(18);
	buttonPressed.setFillColor(sf::Color(255, 189,0));
	buttonPressed.setStyle(sf::Text::Bold);
	buttonPressed.setPosition({700.f, 0.f});
	buttonPressed.setString("*");

	sf::Text mousePos(font);
	mousePos.setCharacterSize(10);
	sf::Color mouseColor = sf::Color(255, 100, 255);
	mousePos.setFillColor(mouseColor);
	mousePos.setStyle(sf::Text::Bold);
	mousePos.setPosition({400.f, 300.f});
	mousePos.setString("*");
	int mouseX;
	int mouseY;
// run the program as long as the window is open
    while (window.isOpen()) {
	    if(gs.isPaused()){
			text.setString(std::format("It's Paused [||] <spacebar> to unpause.", gs.getScale(), gs.getCols(), gs.getRows()));
			text.setFillColor(sf::Color(200, 128, 0));
		} else {
			text.setString(std::format("It Lives! scale:{} ({}X{})", gs.getScale(), gs.getCols(), gs.getRows()));
			text.setFillColor(bannerColor);
		}
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent()) {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				auto scancode = keyPressed->scancode;
				switch(scancode){
					case sf::Keyboard::Scancode::Escape:
						window.close();
						break;
					case sf::Keyboard::Scancode::Space:
					    gs.togglePause();
						buttonPressed.setString("[space]");
						break;
					case sf::Keyboard::Scancode::Up:
					    gs.placeGliderUR(mouseX, mouseY);
						buttonPressed.setString("[up]");
						break;
					case sf::Keyboard::Scancode::Right:
					    gs.placeGliderDR(mouseX, mouseY);
						buttonPressed.setString("[right]");
						break;
					case sf::Keyboard::Scancode::Down:
					    gs.placeGliderDL(mouseX, mouseY);
						buttonPressed.setString("[down]");
						break;
					case sf::Keyboard::Scancode::Left:
					    gs.placeGliderUL(mouseX, mouseY);
						buttonPressed.setString("[left]");
						break;
					case sf::Keyboard::Scancode::Num1:
						gs.updateGridScale(1);
						buttonPressed.setString("[1]");
						break;
					case sf::Keyboard::Scancode::Num2:
						gs.updateGridScale(2);
						buttonPressed.setString("[2]");
						break;
					case sf::Keyboard::Scancode::Num3:
						gs.updateGridScale(3);
						buttonPressed.setString("[3]");
						break;
					case sf::Keyboard::Scancode::Num4:
						gs.updateGridScale(4);
						buttonPressed.setString("[4]");
						break;
					case sf::Keyboard::Scancode::Num5:
						gs.updateGridScale(5);
						buttonPressed.setString("[5]");
						break;
					case sf::Keyboard::Scancode::A:
					    gs.placeVLozenge(mouseX, mouseY);
						buttonPressed.setString("[A]");
						break;
					case sf::Keyboard::Scancode::C:
					    gs.placeColony(mouseX, mouseY);
						buttonPressed.setString("[C]");
						break;
					case sf::Keyboard::Scancode::H:
					    gs.placeSpinnerH(mouseX, mouseY);
						buttonPressed.setString("[H]");
						break;
					case sf::Keyboard::Scancode::K:
						buttonPressed.setString("[K]");
					    gs.clear();
						break;
					case sf::Keyboard::Scancode::R:
					    gs.placeRing(mouseX, mouseY);
						buttonPressed.setString("[R]");
						break;
					case sf::Keyboard::Scancode::S:
					    gs.placeStar(mouseX, mouseY);
						buttonPressed.setString("[S]");
						break;
					case sf::Keyboard::Scancode::V:
					    gs.placeSpinnerV(mouseX, mouseY);
						buttonPressed.setString("[V]");
						break;
					case sf::Keyboard::Scancode::X:
					    gs.placeBigX(mouseX, mouseY);
						buttonPressed.setString("[X]");
						break;
					case sf::Keyboard::Scancode::Z:
					    gs.placeHLozenge(mouseX, mouseY);
						buttonPressed.setString("[Z]");
						break;
					default:
					    break;
					   
				}
			} else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()){
				auto position = mouseMoved->position;
				mouseX = position.x <= static_cast<int>(gs.rightMost()) ? position.x : static_cast<int>(gs.rightMost());
				mouseY = position.y <= static_cast<int>(gs.bottomMost()) ? position.y : static_cast<int>(gs.bottomMost());
				mousePos.setPosition({static_cast<float>(mouseX), static_cast<float>(mouseY)});
			    mouseColor.r = gs.normX2Color(mouseX);	
			    mouseColor.b = gs.normY2Color(mouseY);	
				mousePos.setFillColor(mouseColor);
				mousePos.setString(std::format("*({},{})", gs.column(mouseX), gs.row(mouseY)));
			}
        }

        window.clear(sf::Color::Black);
        // draw stuff in here
		window.draw(text);
		// render green circle per live cell
		for(const auto live: gs.getLivingCells()){
			auto crd = gs.getCellRenderData(live);
           sf::CircleShape liveCell(crd.radius);
		   liveCell.setPosition({crd.scrnX,crd.scrnY});
		   liveCell.setFillColor(sf::Color::Green);
		   window.draw(liveCell);
		}
		
		// render faded purple per nursery 
		// /*  // uncomment to see potential places for life
		for(const auto nurse: gs.getNursery()){
		   auto crd = gs.getCellRenderData(nurse);
           sf::CircleShape nurseCell(crd.radius);
		   nurseCell.setPosition({crd.scrnX,crd.scrnY});
		   nurseCell.setFillColor(sf::Color::Blue);
		   window.draw(nurseCell);
		}

		window.draw(mousePos);
		window.draw(buttonPressed);
		window.draw(sideBorder);
		window.draw(bottomBorder);
		// */
		if( not gs.isPaused()){
			gs.calculateNextRound();
		}	
        // end the current frame
        window.display();
    }
	return 0;
}