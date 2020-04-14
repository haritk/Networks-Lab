set terminal qt 0
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel "Size(Bytes)" font "Helvetica, 12"
set title "TCP Hybla-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:35000]
set grid
set xtics 50
plot "1_hybla_congestionwindow.txt" using 1:2 title "Congestion Window" with lines lc "red" lw 2

set terminal qt 1
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel " Speed(Kbps)" font "Helvetica, 12"
set title "TCP Hybla-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:500]
set grid
set xtics 50
plot "1_hybla_throughput.txt" using 1:2 title "Throughput" with lines lc "blue" lw 2

set terminal qt 2
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel " Speed(Kbps)" font "Helvetica, 12"
set title "TCP Hybla-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:500]
set grid
set xtics 50
plot "1_hybla_goodput.txt" using 1:2 title "Goodput" with lines lc "green" lw 2

set terminal qt 3
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel "Size(Bytes)" font "Helvetica, 12"
set title "TCP Westwood-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:30000]
set grid
set xtics 50
plot "1_westwood_congestionwindow.txt" using 1:2 title "Congestion Window" with lines lc "red" lw 2

set terminal qt 4
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel " Speed(Kbps)" font "Helvetica, 12"
set title "TCP Westwood-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:500]
set grid
set xtics 50
plot "1_westwood_throughput.txt" using 1:2 title "Throughput" with lines lc "blue" lw 2

set terminal qt 5
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel " Speed(Kbps)" font "Helvetica, 12"
set title "TCP Westwood-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:500]
set grid
set xtics 50
plot "1_westwood_goodput.txt" using 1:2 title "Goodput" with lines lc "green" lw 2

set terminal qt 6
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel "Size(Bytes)" font "Helvetica, 12"
set title "TCP Yeah-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:20000]
set grid
set xtics 50
plot "1_yeah_congestionwindow.txt" using 1:2 title "Congestion Window" with lines lc "red" lw 2

set terminal qt 7
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel " Speed(Kbps)" font "Helvetica, 12"
set title "TCP Yeah-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:400]
set grid
set xtics 50
plot "1_yeah_throughput.txt" using 1:2 title "Throughput" with lines lc "blue" lw 2

set terminal qt 8
set xlabel "Time(s)" font "Helvetica, 12"
set ylabel " Speed(Kbps)" font "Helvetica, 12"
set title "TCP Yeah-1" font "Helvetica, 12"
set xrange[0:300]
set yrange[0:400]
set grid
set xtics 50
plot "1_yeah_goodput.txt" using 1:2 title "Goodput" with lines lc "green" lw 2
