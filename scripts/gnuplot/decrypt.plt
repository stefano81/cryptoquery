set xlabel 'Number of attributes'
set ylabel 'Time in microseconds'
set key left box

plot 'decrypt-a-sorted' w linespoint title 'Curve A', 'decrypt-a1-sorted' w linespoint title 'Curve A1', 'decrypt-e-sorted' w linespoint title 'Curve E'
