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


int main(int argc, char* argv[]) {
	const uint32_t sizes[] = {1,2,4,5,10};
	uint32_t requestedScale;
	size_t sizeIdx = 5;
	if(argc == 1){
		requestedScale = 10;
	} else {
		try {
			sizeIdx = static_cast<size_t>(atoi(argv[1]));
			if(sizeIdx > 5){
				requestedScale = 10;
			} else if(sizeIdx < 1){
				sizeIdx = 1;
				requestedScale = 1;
			} else {
				requestedScale = sizes[sizeIdx-1];
			}
		} catch (std::exception e){
			[[maybe_unused]] auto ex = e;
			requestedScale = 10;
		}
	}

    // create the window and set up
    const sf::String title = "Azi's Game of Life";
	const uint32_t screenWidth = 800;
	const uint32_t screenHeight = 600;
	const uint32_t gridCols = 40*requestedScale;
	const uint32_t gridRows = 30*requestedScale;
    sf::RenderWindow window(sf::VideoMode({screenWidth, screenHeight}), title);

	GameState gs{gridCols, gridRows, screenWidth, screenHeight};
    window.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("icon.png")) {
        window.setIcon(icon);
    }

	sf::Font font("LeroyLettering.ttf");
	sf::Text text(font);
	text.setString(std::format("It Lives! scale:{} ({}X{})", sizeIdx, gridCols, gridRows));
	text.setFillColor(sf::Color::Green);
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

	sf::Text mousePos(font);
	mousePos.setCharacterSize(10);
	mousePos.setFillColor(sf::Color::Magenta);
	mousePos.setStyle(sf::Text::Bold);
	mousePos.setPosition({400.f, 300.f});
	mousePos.setString("*");
	int mouseX;
	int mouseY;
// run the program as long as the window is open
    while (window.isOpen()) {
	    if(gs.isPaused()){
			text.setString(std::format("It's Paused [||] <spacebar> to unpause.", sizeIdx, gridCols, gridRows));
			text.setFillColor(sf::Color::Yellow);
		} else {
			text.setString(std::format("It Lives! scale:{} ({}X{})", sizeIdx, gridCols, gridRows));
			text.setFillColor(sf::Color::Green);
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
						break;
					case sf::Keyboard::Scancode::A:
					    gs.placeVLozenge(mouseX, mouseY);
						break;
					case sf::Keyboard::Scancode::C:
					    gs.placeColony(mouseX, mouseY);
						break;
					case sf::Keyboard::Scancode::R:
					    gs.placeRing(mouseX, mouseY);
						break;
					case sf::Keyboard::Scancode::S:
					    gs.placeStar(mouseX, mouseY);
						break;
					case sf::Keyboard::Scancode::X:
					    gs.placeBigX(mouseX, mouseY);
						break;
					case sf::Keyboard::Scancode::Z:
					    gs.placeHLozenge(mouseX, mouseY);
						break;
					default:
					    break;
					   
				}
			} else if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()){
				auto position = mouseMoved->position;
				mouseX = position.x <= static_cast<int>(gs.rightMost()) ? position.x : static_cast<int>(gs.rightMost());
				mouseY = position.y <= static_cast<int>(gs.bottomMost()) ? position.y : static_cast<int>(gs.bottomMost());
				mousePos.setPosition({static_cast<float>(mouseX), static_cast<float>(mouseY)});
				
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