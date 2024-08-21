shopt -s globstar
g++ -Wall -g **/*.cpp -o app.out -lsfml-graphics -lsfml-window -lsfml-system -lboost_iostreams -lboost_system -lboost_filesystem
