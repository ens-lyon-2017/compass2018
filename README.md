# compass2018
c'est du sabotage/de l'exploitation

## Ce qu'on a :

* une multiplication matricielle
* un abstract
* un squelette d'article dans le répertoire 2018-COMPAS

## Ce qu'il nous faut

* des stats

## Notre projet (proposition d'axe de rédaction par Antonin)

 * Multiplication matricielle 
   1. Exécuter les stats précises sur la multiplication matricielle.
   2. Dire que c'est un peu pourri
   3. Estimer le gain avec la proposition de Sébastien segments+pointeurs/registres
 * Huffmann 
   1. Faire les stats sur chip8. 
   2. Changer Huffmann. 
   3. Refaire les mêmes stats. 
   4. Comparer

## asm2017

L'assembleur de Sébastien et Alain, si vous souhaitez l'utiliser

* 64 bits
* push/pop avec une taille
* Ne rien mettre sur une ligne après un label
* Faites attention à là où vous écrivez : typiquement la mémoire par défaut s'arrête à `0x60000`, n'écrivez pas au-delà sous peine de `fatal error: memory_write: Out of bounds` ; plus d'infos dans `doc/emu_memory_layout.md`. 
