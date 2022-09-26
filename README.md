# metaSPP_cpp
Heuristique de construction et recherche locale pour le Set Packing Problem (SPP)

*Commande pour compiler*:
```bash
make clean && make
```

*Commande pour lancer le programme*:
```bash
./bin/DM1
```

**ATTENTION:** le répertoire `instances` doit se trouver dans le même répertoire d'où le programme est lancé. Si le programme est lancé depuis `bin` alors `instances` doit être dans `bin`. Pour utiliser le makefile les répertoires `bin`, `include`, `src` et `obj` doivent être présents.

### Descente/Plus profonde descente
Pour effectuer des améliorations par recherche locale de type plus profonde descente veuillez définir DEEPSEARCH (macro) à true :
```c
#define DEEPSEARCH true
```
Sinon pour des descentes normales :
```c
#define DEEPSEARCH false
```
