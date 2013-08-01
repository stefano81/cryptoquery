set xlabel 'Number of attributes'
set ylabel 'Time in microseconds'
set key left box

plot 'keygen-a-sorted' w linespoint title 'Curve A', 'keygen-a1-sorted' w linespoint title 'Curve A1', 'keygen-e-sorted' w linespoint title 'Curve E'
