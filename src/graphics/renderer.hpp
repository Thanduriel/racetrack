#pragma once

#ifdef WITH_GRAPHICS

#include "../game/map.hpp"
#include "../game/game.hpp"
#include "../game/player.hpp"

#include <SFML/Graphics.hpp>

namespace graphics {

	class Renderer
	{
	public:
		Renderer(const game::Map& map);

		Direction getAction(const game::GameState& state);
	private:
		sf::Vector2f toScreenSpace(Point p) const;

		void draw(const game::GameState& state);
		void draw(const game::Line& line, sf::Color color, float thickness);

		const game::Map& m_map;
		float m_scale; // pixels per lattice unit
		sf::RenderWindow m_window;
	};
}

#endif // WITH_GRAPHICS
