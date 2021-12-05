## Ejecución:

Este programa está escrito en base al estándar POSIX y simula la ejecución de los comandos cat, tr y wc conectados por tuberías:

`cat FILEIN1 FILEIN2 ... FILEINn | tr -s SRC -d DST | wc -lc`

El programa se debe ejecutar según este patrón:

`./cat_pipe_tr_pipe_wc [-e] -s SRC -d DST [FILEIN1 FILEIN2 ... FILEINn]`

Si no se incluye ningún fichero se usará la entrada estándar. Ambas cadenas indicadas en `SRC` y `DST` deben tener la misma longitud que debe ser mayor o igual a 1. La opción `-e` permite que los carácteres de las cadenas `SRC` y `DST` tengan carácteres de control (que serán tratados como un carácter), por ejemplo, de esta forma la letra "A" sería sustituida por un salto de línea:

`./cat_pipe_tr_pipe_wc -e -s 'A' -d '\n' file1 file2`

----------------------------------------------------------------------------

## Execution:

Program based on the POSIX standard that simulates the execution of cat, tr and wc commands connected by pipes:

`cat FILEIN1 FILEIN2 ... FILEINn | tr -s SRC -d DST | wc -lc`

The program must be run according to this pattern:

`./cat_pipe_tr_pipe_wc [-e] -s SRC -d DST [FILEIN1 FILEIN2 ... FILEINn]`

If no file is included, standard input will be used. Both strings indicated in `SRC` and `DST` must have the same length that must be greater than or equal to 1. The `-e` option allows the characters of the` SRC` and `DST` strings to have control characters ( which will be treated as a character), for instance, in this example the letter "A" would be replaced by a line break:

`./cat_pipe_tr_pipe_wc -e -s 'A' -d '\n' file1 file2`
