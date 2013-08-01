set xlabel 'Number of attributes'
set ylabel 'Time in microseconds'
set key left box

plot 'setup-a-sorted' w linespoint title 'Curve A', 'setup-a1-sorted' w linespoint title 'Curve A1', 'setup-e-sorted' w linespoint title 'Curve E'
