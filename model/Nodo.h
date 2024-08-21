#ifndef NODO_H
#define NODO_H

#include <string>
#include <functional>

enum TipoRecorrido { PRE_ORDEN, IN_ORDEN, POST_ORDEN };

class Nodo {
    public:
        int valor;
        Nodo* izquierda;
        Nodo* derecha;
        Nodo* padre;
        int id;

        Nodo(int val);

        static void insertar(Nodo* raiz, int val);

        static void recorrerInOrden(Nodo* nodo, std::function<void(Nodo*)> fn);
        static void recorrerPreOrden(Nodo* nodo, std::function<void(Nodo*)> fn);
        static void recorrerPostOrden(Nodo* nodo, std::function<void(Nodo*)> fn);
        static void recorrer(Nodo* raiz, std::function<void(Nodo*)> fn, TipoRecorrido tipo);
        static void imprimir(Nodo* raiz, TipoRecorrido tipo = IN_ORDEN);
        static void establecerIdsYPadres(Nodo* raiz);
        static void visualizarArbol(Nodo* raiz);
};

#endif