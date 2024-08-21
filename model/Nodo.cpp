#include <iostream>
#include "gnuplot-iostream.h"
#include "Nodo.h"

Nodo::Nodo(int val){
    valor = val;
    izquierda = nullptr;
    derecha = nullptr;
    padre = nullptr;
    id = 0;
}

void Nodo::insertar(Nodo *raiz, int val) {
    if (val < raiz->valor) {
        if (raiz->izquierda == nullptr) {
            raiz->izquierda = new Nodo(val);
        } else {
            insertar(raiz->izquierda, val);
        }
    } else {
        if (raiz->derecha == nullptr) {
            raiz->derecha = new Nodo(val);
        } else {
            insertar(raiz->derecha, val);
        }
    }
}

void Nodo::recorrerInOrden(Nodo* nodo, std::function<void(Nodo*)> fn) {
    if (nodo == nullptr) {
        return;
    }

    recorrerInOrden(nodo->izquierda, fn);
    fn(nodo);
    recorrerInOrden(nodo->derecha, fn);
}

void Nodo::recorrerPreOrden(Nodo* nodo, std::function<void(Nodo*)> fn) {
    if (nodo == nullptr) {
        return;
    }

    fn(nodo);
    recorrerPreOrden(nodo->izquierda, fn);
    recorrerPreOrden(nodo->derecha, fn);
}

void Nodo::recorrerPostOrden(Nodo* nodo, std::function<void(Nodo*)> fn) {
    if (nodo == nullptr) {
        return;
    }

    recorrerPostOrden(nodo->izquierda, fn);
    recorrerPostOrden(nodo->derecha, fn);
    fn(nodo);
}

void Nodo::recorrer(Nodo* raiz, std::function<void(Nodo*)> fn, TipoRecorrido tipo) {
    switch (tipo) {
        case TipoRecorrido::PRE_ORDEN:
            Nodo::recorrerPreOrden(raiz, fn);
            break;
        case TipoRecorrido::IN_ORDEN:
            Nodo::recorrerInOrden(raiz, fn);
            break;
        case TipoRecorrido::POST_ORDEN:
            Nodo::recorrerPostOrden(raiz, fn);
            break;
        default:
            break;
    }
}

void Nodo::imprimir(Nodo* raiz, TipoRecorrido tipo) {
    recorrer(raiz, [](Nodo* nodo){
        std::cout << " -> " << nodo->valor << std::endl;
    }, tipo);
}

void Nodo::establecerIdsYPadres(Nodo* raiz) {
    int id = 0;
    recorrer(raiz, [&id](Nodo* nodo){
        nodo->id = id++;
        if (nodo->izquierda) {
            nodo->izquierda->padre = nodo;
        }
        if (nodo->derecha) {
            nodo->derecha->padre = nodo;
        }
    }, TipoRecorrido::IN_ORDEN);
}

void Nodo::visualizarArbol(Nodo* raiz) {
    Gnuplot gp;
    Nodo::establecerIdsYPadres(raiz);
	std::string script = R"(
### tree diagram with gnuplot
reset session

#ID  Parent   Name   Color
$Data <<EOD
)";
    recorrer(raiz, [&script](Nodo* nodo) {
        script += std::to_string(nodo->id + 3) + "  " + (nodo->padre != nullptr ? std::to_string(nodo->padre->id + 3) : "NaN") + "  " + std::to_string(nodo->valor) + "  black\n";
    }, TipoRecorrido::IN_ORDEN);
    script += R"(EOD

# put datablock into strings
IDs = Parents = Names = Colors = ''
set table $Dummy
    plot $Data u (IDs = IDs.strcol(1).' '): \
                (Parents = Parents.strcol(2).' '): \
                (Names = Names.strcol(3).' '): \
                (Colors = Colors.strcol(4).' ') w table
unset table

# Top node has no parent ID 'NaN'
Start(n) = int(sum [i=1:words(Parents)] (word(Parents,i) eq 'NaN' ? int(word(IDs,i)) : 0))

# get list index by ID
ItemIdx(s,n) = n == n ? (tmp=NaN, sum [i=1:words(s)] ((word(s,i)) == n ? (tmp=i,0) : 0), tmp) : NaN

# get parent of ID n
Parent(n) = word(Parents,ItemIdx(IDs,n))

# get level of ID n, recursive function
Level(n) = n == n ? Parent(n)>0 ? Level(Parent(n))-1 : 0 : NaN

# get number of children of ID n
ChildCount(n) = int(sum [i=1:words(Parents)] (word(Parents,i)==n))

# Create child list of ID n
ChildList(n) = (Ch = ' ', sum [i=1:words(IDs)] (word(Parents,i)==n ? (Ch = Ch.word(IDs,i).' ',1) : (Ch,0) ), Ch )

# m-th child of ID n
Child(n,m) = word(ChildList(n),m)

# List of leaves, recursive function
LeafList(n) = (LL='', ChildCount(n)==0 ? LL=LL.n.' ' : sum [i=1:ChildCount(n)] (LL=LL.LeafList(Child(n,i)), 0),LL)

# create list of all leaves
LeafAll = LeafList(Start(0))

# get x-position of ID n, recursive function
XPos(n) = ChildCount(n) == 0 ? ItemIdx(LeafAll,n) : (sum [i=1:ChildCount(n)] (XPos(Child(n,i))))/(ChildCount(n))

# create the tree datablock for plotting
set print $Tree
    do for [j=1:words(IDs)] {
        n = int(word(IDs,j))
        print sprintf("% 3d % 7.2f % 4d % 5s %s", n, XPos(n), Level(n), word(Names,j), word(Colors,j))
    }
set print
print $Tree

# get x and y distance from ID n to its parent
dx(n) = XPos(Parent(int(n))) - XPos(int(n))
dy(n) = Level(Parent(int(n))) - Level(int(n))

unset border
unset tics
set offsets 0.25, 0.25, 0.25, 0.25

plot $Tree u 2:3:(dx($1)):(dy($1)):(strcol(5)) w vec nohead ls -1 not,\
        '' u 2:3:(strcol(4)) w p pt 7 ps 6 lc rgb var not, \
        '' u 2:3 w p pt 6 ps 6 lw 1.5 lc rgb "white" not, \
        '' u 2:3:4 w labels tc rgb "white" offset 0,0.1 center notitle
### end of code)";
    gp << script;
}
