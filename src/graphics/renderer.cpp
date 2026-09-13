#include "renderer.hpp"

namespace graphics {

constexpr float gridThickness = 1.f;
constexpr float boundaryThickness = 2.f;
constexpr float historyThickness = 2.f;
constexpr float goalThickness = 2.f;

constexpr sf::Color gridColor(120, 120, 120);
constexpr sf::Color historyColor(120, 180, 160);

Renderer::Renderer(const game::Map& map, float scale)
    : m_map(map)
    , m_scale(scale)
    , m_window(sf::VideoMode({ static_cast<unsigned>((map.width + 2) * m_scale),
                   static_cast<unsigned>((map.height + 2) * m_scale) }),
          "Racetrack")
{
}

Direction Renderer::getAction(const game::GameState& state, const std::vector<game::Line>& history)
{
    while (m_window.isOpen()) {
        const sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
        const sf::Vector2f mouseWorld = m_window.mapPixelToCoords(mousePos);
        // -1 to invert toScreenSpace, + 0.5 to get nodes, not cells when truncating (round to 0)
        const auto pos = mouseWorld / m_scale - sf::Vector2f(0.5f, 0.5f);
        const Point p = { static_cast<int>(pos.x), static_cast<int>(pos.y) };
        const game::PlayerState& current = state.getActive();
        const Direction d = p - (current.position + current.velocity);
        const bool isValidAction = d.lenSq() <= 2;

        while (const std::optional event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                m_window.close();
            else if (const auto* ev = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (ev->button == sf::Mouse::Button::Left && isValidAction) {
                    return d;
                }
            }
        }

        m_window.clear(sf::Color::White);
        draw(state);
        draw(history);
        // highlight possible move on hover
        if (isValidAction) {
            const float buttonSize = m_scale * 0.75f;
            sf::RectangleShape actionButton({ buttonSize, buttonSize });
            actionButton.setOrigin({ buttonSize * 0.5f, buttonSize * 0.5f });
            actionButton.setFillColor(sf::Color::Red);
            actionButton.setPosition(toScreenSpace(p));
            m_window.draw(actionButton);
        }
        m_window.display();
    }

    return { 0, 0 };
}

sf::Vector2f Renderer::toScreenSpace(Point p) const
{
    return { (p.x + 1) * m_scale, (p.y + 1) * m_scale };
}

void Renderer::draw(const game::GameState& state)
{
    // map
    // grid
    sf::RectangleShape lineVertical({ gridThickness, m_map.height * m_scale });
    lineVertical.setFillColor(gridColor);
    for (unsigned x = 0; x < m_map.width + 1; ++x) {
        lineVertical.setPosition(toScreenSpace({ static_cast<int>(x), 0 }));
        m_window.draw(lineVertical);
    }

    sf::RectangleShape lineHorizontal({ m_map.width * m_scale, gridThickness });
    lineHorizontal.setFillColor(gridColor);
    for (unsigned y = 0; y < m_map.height + 1; ++y) {
        lineHorizontal.setPosition(toScreenSpace({ 0, static_cast<int>(y) }));
        m_window.draw(lineHorizontal);
    }

    // boundary
    draw(m_map.boundary0, sf::Color::Black, boundaryThickness);
    draw(m_map.boundary1, sf::Color::Black, boundaryThickness);

    // goals
    for (const game::Line& goal : m_map.goals) {
        draw(goal, sf::Color::Blue, goalThickness);
    }

    // players
    const float playerSize = 0.75f * m_scale; // radius
    sf::CircleShape playerShape(playerSize);
    playerShape.setOrigin({ playerSize, playerSize });
    playerShape.setFillColor(sf::Color::Red);
    for (const game::PlayerState& playerState : state.playerStates) {
        playerShape.setPosition(toScreenSpace(playerState.position));
        m_window.draw(playerShape);
    }

    // current player
    const game::PlayerState& current = state.getActive();
    const Point dest = current.position + current.velocity;
    const game::Line move { .points = { current.position, dest } };
    draw(move, sf::Color::Green, 2.f);

    const float buttonSize = m_scale * 0.75f;
    sf::RectangleShape actionButton({ buttonSize, buttonSize });
    actionButton.setOrigin({ buttonSize * 0.5f, buttonSize * 0.5f });
    actionButton.setFillColor(sf::Color::Green);
    for (const Direction& action : game::ACTIONS) {
        const Point p = dest + action;
        actionButton.setPosition(toScreenSpace(p));
        m_window.draw(actionButton);
    }
}

void Renderer::draw(const game::GameHistory& history)
{
    for (const game::Line& line : history) {
        draw(line, historyColor, historyThickness, m_scale * 0.75f);
    }
}

void Renderer::draw(const game::Line& line, sf::Color color, float thickness, float pointSize)
{
    std::vector<sf::Vertex> lineF;
    lineF.reserve(line.points.size() * 2 + 2);
    for (size_t i = 1; i < line.points.size(); ++i) {
        const sf::Vector2f p0 = toScreenSpace(line.points[i - 1]);
        const sf::Vector2f p1 = toScreenSpace(line.points[i]);
        const sf::Vector2f diff = (p1 - p0);
        const float l = diff.length();
        if (l == 0.f) {
            continue;
        }
        const sf::Vector2f d = diff / l;
        const sf::Vector2f ort = { -d.y, d.x };
        lineF.emplace_back(p0 + ort * thickness, color);
        lineF.emplace_back(p0 - ort * thickness, color);
        lineF.emplace_back(p1 + ort * thickness, color);
        lineF.emplace_back(p1 - ort * thickness, color);
    }

    m_window.draw(lineF.data(), lineF.size(), sf::PrimitiveType::TriangleStrip);

    if (pointSize > 0.f) {
        sf::RectangleShape crossBar({ pointSize, thickness });
        crossBar.setOrigin({ pointSize * 0.5f, thickness * 0.5f });
        crossBar.setFillColor(color);

        crossBar.rotate(sf::degrees(45));
        for(const Point& p : line.points) {
            crossBar.setPosition(toScreenSpace(p));
            m_window.draw(crossBar);
        }
        
        crossBar.rotate(sf::degrees(90));
        for(const Point& p : line.points) {
            crossBar.setPosition(toScreenSpace(p));
            m_window.draw(crossBar);
        }
    }
}

}
