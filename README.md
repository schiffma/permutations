# permutations
Permutation table-valued function implemented as SQLite run-time loadable extension.

Examples:

select * from permutations('ABC');

permut</br>
ABC</br>
ACB</br>
BAC</br>
BCA</br>
CAB</br>
CBA</br>

DROP TABLE IF EXISTS perm10;<br>
CREATE TABLE perm10 AS SELECT * FROM permutations('abcdefghij');

Additional Information:

Currently N <= 10 is supported, input with string length > 10 will be ignored.

The extension was created with Visual Studio Community 2019 and tested with DB Browser for SQLite Version 3.12.2 on Windows 10.
