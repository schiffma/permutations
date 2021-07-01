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

Currently N <= 10 is supported, input with string length > 10 will be generate a specific error message.

The extension was created with Visual Studio Community 2019 and tested with DB Browser for SQLite Version 3.12.2 on Windows 10.

The persistent creation of 10! = 3628800 rows will take  approximately 1s query execution time on a modern Intel Core i7 processor based system with SSD.

CREATE TABLE perm10 AS SELECT * FROM permutations('abcdefghij');

Execution finished without errors.<br>
Result: query executed successfully. Took 957ms<br>
At line 24:<br>
CREATE TABLE perm10 AS SELECT * FROM permutations('abcdefghij');<br>
