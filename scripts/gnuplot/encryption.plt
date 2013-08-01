set xlabel 'Number of attributes'
set ylabel 'Time in microseconds'
set key left box

plot 'encrypt-a-sorted' w linespoint title 'Curve A', 'encrypt-a1-sorted' w linespoint title 'Curve A1', 'encrypt-e-sorted' w linespoint title 'Curve E'
