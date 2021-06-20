# permutations
Permutation table-valued function implemented as SQLite run-time loadable extension.

select * from permutations('ABC');

permut</br>
ABC</br>
ACB</br>
BAC</br>
BCA</br>
CAB</br>
CBA</br>

Currently N <= 10 is supported, input with string length > 10 will be ignored.

The extension was created with Visual Studio Community 2019 and tested with DB Browser for SQLite Version 3.12.2 .
