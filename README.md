# compass2018
c'est du sabotage/de l'exploitation

## Ce qu'on a :

* une multiplication matricielle
* un abstract

## Ce qu'il nous faut

* des stats
* un article

## asm2017

L'assembleur de Sébastien et Alain, si vous souhaitez l'utiliser

* 64 bits
* push/pop avec une taille
* Ne rien mettre sur une ligne après un label
* Faites attention à là où vous écrivez : typiquement la mémoire par défaut s'arrête à `0x60000`, n'écrivez pas au-delà sous peine de `fatal error: memory_write: Out of bounds` ; plus d'infos dans `doc/emu_memory_layout.md`. 
