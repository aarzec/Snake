#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include "../include/sfmlbutton.hpp"
#include "../model/Nodo.h"

const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20;
const float SPEED = 0.15f;

enum Direction { Up, Down, Left, Right };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int xPos, int yPos) : x(xPos), y(yPos) {}
};

class Snake {
public:
    Snake(int blockSize) : size(blockSize), direction(Direction::Right), grow(false) {
        snakeBody.push_back(SnakeSegment(5, 5));
        snakeBody.push_back(SnakeSegment(4, 5));
        snakeBody.push_back(SnakeSegment(3, 5));
    }

    void move() {
        if (grow) {
            snakeBody.push_back(SnakeSegment(snakeBody.back().x, snakeBody.back().y));
            grow = false;
        }

        for (int i = snakeBody.size() - 1; i > 0; --i) {
            snakeBody[i] = snakeBody[i - 1];
        }

        switch (direction) {
            case Direction::Up: snakeBody[0].y -= 1; break;
            case Direction::Down: snakeBody[0].y += 1; break;
            case Direction::Left: snakeBody[0].x -= 1; break;
            case Direction::Right: snakeBody[0].x += 1; break;
        }

        if (snakeBody[0].x < 0) snakeBody[0].x = WIDTH / SIZE - 1;
        if (snakeBody[0].x >= WIDTH / SIZE) snakeBody[0].x = 0;
        if (snakeBody[0].y < 0) snakeBody[0].y = HEIGHT / SIZE - 1;
        if (snakeBody[0].y >= HEIGHT / SIZE) snakeBody[0].y = 0;
    }

    void growSnake() {
        grow = true;
    }

    bool checkCollision() {
        for (size_t i = 1; i < snakeBody.size(); ++i) {
            if (snakeBody[0].x == snakeBody[i].x && snakeBody[0].y == snakeBody[i].y)
                return true;
        }
        return false;
    }

    void setDirection(Direction dir) {
        if ((direction == Direction::Up && dir != Direction::Down) ||
            (direction == Direction::Down && dir != Direction::Up) ||
            (direction == Direction::Left && dir != Direction::Right) ||
            (direction == Direction::Right && dir != Direction::Left)) {
            direction = dir;
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape rectangle(sf::Vector2f(size, size));
        for (auto& segment : snakeBody) {
            rectangle.setPosition(segment.x * size, segment.y * size);
            window.draw(rectangle);
        }
    }

    sf::Vector2i getHeadPosition() {
        return sf::Vector2i(snakeBody[0].x, snakeBody[0].y);
    }

private:
    std::vector<SnakeSegment> snakeBody;
    int size;
    Direction direction;
    bool grow;
};

class Food {
public:
    Food(int blockSize, int num, sf::Font ft) : size(blockSize), fuente(ft) {
        food.setSize(sf::Vector2f(size, size));
        food.setFillColor(sf::Color::Red);
        generateNewPosition();
        labelNumero.setFont(fuente);
        labelNumero.setCharacterSize(16);
        labelNumero.setFillColor(sf::Color::White);
        setNumero(num);
    }

    void generateNewPosition() {
        position.x = rand() % (WIDTH / size);
        position.y = rand() % (HEIGHT / size);
        food.setPosition(position.x * size, position.y * size);
    }

    void setNumero(int num) {
        n = num;
        labelNumero.setString(std::to_string(n));
        labelNumero.setPosition(position.x * size + 2, position.y * size);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(food);
        window.draw(labelNumero);
    }

    sf::Vector2i getPosition() {
        return position;
    }

private:
    sf::RectangleShape food;
    sf::Vector2i position;
    sf::Text labelNumero;
    int size;
    sf::Font fuente;
    int n;
};

template <typename T>
T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

sf::Color inverseLegibleColor(sf::Color color) {
    int r = color.r;
    int g = color.g;
    int b = color.b;
    if (r * 0.299 + g * 0.7 + b * 0.114 > 150) {
        return sf::Color::Black;
    } else {
        return sf::Color::White;
    }
}

bool generarNumerosUnicos(int index, std::vector<int>& result, std::unordered_set<int>& usados) {
    if (index == 100) {
        return true;
    }

    int num = rand() % 100 + 1;
    if (usados.find(num) == usados.end()) {
        usados.insert(num);
        result[index] = num;
        if (generarNumerosUnicos(index + 1, result, usados)) {
            return true;
        }

        usados.erase(num);
    }

    return generarNumerosUnicos(index, result, usados);
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    const unsigned int FPS = 30;

    const int windowWidth = WIDTH;
    const int windowHeight = HEIGHT + 75;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "La culebra");
    window.setFramerateLimit(FPS);

    // Cargar fuentes
    sf::Font ft_arial;
    ft_arial.loadFromFile("fonts/Arial.ttf");

    // Controles
    sf::Text labelPuntuacion("Puntuacion: 0", ft_arial, 20);
    labelPuntuacion.setFillColor(sf::Color::White);
    labelPuntuacion.setPosition(50, windowHeight - 50);

    sf::RectangleShape rectFooter(sf::Vector2f(windowWidth, 100));
    rectFooter.setFillColor(sf::Color::Black);
    rectFooter.setOutlineColor(sf::Color(50, 50, 50));
    rectFooter.setOutlineThickness(1);
    rectFooter.setPosition(0, windowHeight - 75);
    RectButton btnArbol(ft_arial, sf::Vector2f(150, 35.f), sf::Vector2f(windowWidth - 170.f, windowHeight - 50));
    btnArbol.setButtonLabel(20.f, "Mostrar arbol");
    btnArbol.setButtonColor(sf::Color(0, 200, 0), sf::Color(0, 150, 0), sf::Color(0, 100, 0));
    btnArbol.setLabelColor(sf::Color::White);

    // Otros
    sf::Clock clock;
    float timer = 0.0f;
    std::unordered_set<int> numerosUsados;
    int indiceAleatorios = 0;
    std::vector<int> aleatorios(100);
    int puntuacion = 0;
    if (generarNumerosUnicos(0, aleatorios, numerosUsados)) {
        std::cout << aleatorios.size() << std::endl;
    } else {
        std::cout << "No se pudieron generar 100 numeros unicos." << std::endl;
        exit(1);
    }
    Nodo* arbolComidos = nullptr;

    // Elementos
    Food food(SIZE, aleatorios[indiceAleatorios], ft_arial);
    Snake snake(SIZE);

    sf::Event event;
    while (window.isOpen()) {
        // Step
        float time = clock.restart().asSeconds();
        timer += time;

        // - Eventos
        while (window.pollEvent(event)) {
            btnArbol.getButtonStatus(window, event);

            if (btnArbol.isPressed) {
                if (arbolComidos != nullptr) {
                    Nodo::visualizarArbol(arbolComidos);
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) snake.setDirection(Direction::Up);
                if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) snake.setDirection(Direction::Down);
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) snake.setDirection(Direction::Left);
                if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) snake.setDirection(Direction::Right);
            }
        }

        // - Actualizar
        if (timer > SPEED) {
            timer = 0.0f;
            snake.move();

            if (snake.getHeadPosition() == food.getPosition()) {
                if (arbolComidos == nullptr) {
                    arbolComidos = new Nodo(aleatorios[indiceAleatorios]);
                } else {
                    Nodo::insertar(arbolComidos, aleatorios[indiceAleatorios]);
                }

                indiceAleatorios++;

                if (indiceAleatorios == 100) {
                    std::cout << "¡Felicidades! Has completado el juego" << std::endl;
                    window.close();
                    exit(0);
                }

                puntuacion += 1;
                labelPuntuacion.setString("Puntuacion: " + std::to_string(puntuacion));

                snake.growSnake();
                food.generateNewPosition();
                food.setNumero(aleatorios[indiceAleatorios]);
            }

            if (snake.checkCollision()) {
                window.close();
            }
        }

        // Dibujar
        window.clear();
        // - Fondo
        window.draw(rectFooter);
        // - Textos
        window.draw(labelPuntuacion);
        // - Botones
        btnArbol.draw(window);
        // - Juego
        snake.draw(window);
        food.draw(window);

        window.display();
    }

    return 0;
}
