# cpp-simple-vector
A small analogue of the vector template class from the standard library is implemented.

Data is stored in memory dynamically allocated on the heap. Memory is allocated uninitialized, initialization occurs when elements are actually added to the vector.

When adding new elements, if there is not enough allocated memory, a new memory section twice the size of the previous one is allocated, into which data from the old memory section is moved (or copied), after which the old section is deleted.

Реализован небольшой аналог шаблонного класса vector из стандартной библиотеки.

Данные хранятся в памяти, динамически выделяемой в куче. Память выделяется неинициализированная, инициализация происходит при фактическом добавлении элементов в вектор.

При добавлении новых элементов, если выделенной памяти недостаточно - выделяется новый участок памяти размером в два раза больше предыдущего, в который перемещаются (либо копируются) данные из старого участка памяти, после чего старый участок удаляется.
