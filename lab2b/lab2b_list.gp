#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# Plot the total number of operations per second by the number of threads
set title "Plot 1: Aggregate throughput for synchronized list operations"
set xlabel "Threads"
set logscale x 2
set xrange [1:24]
set ylabel "Operations per second"
set logscale y 10
set output 'lab2b_1.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/$7) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/$7) \
	title 'spin locks' with linespoints lc rgb 'green'


set title "Plot 2: Wait-for-lock time & average time per operation for mutex threads"
set xlabel "Threads"
set logscale x 2
set xrange [1:24]
set ylabel "Time (ns)"
set logscale y 10
set output 'lab2b_2.png'
# note that unsuccessful runs should have produced no output
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'wait-for-lock time' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'average time per operation' with linespoints lc rgb 'blue'

set title "Plot 3: Threads and Iterations that run without failure (4 sub-lists, yield=id)"
set logscale x 4
set xrange [1:24]
set xlabel "Threads"
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
set key left top
plot \
    "< grep 'list-id-none,[0-9]*' lab2b_list.csv" using ($2):($3) \
    title 'unprotected' with points lc rgb 'red', \
    "< grep 'list-id-m,[0-9]*' lab2b_list.csv" using ($2):($3) \
    title 'mutex' with points pointtype 5 lc rgb 'green', \
    "< grep 'list-id-s,[0-9]*' lab2b_list.csv" using ($2):($3) \
    title 'spin-lock' with points lc rgb 'blue'

set title "Plot 4: Aggregated throughput vs. Threads for Mutex"
set xlabel "Threads"
set xrange [1:12]
set logscale x 2
set logscale y 10
set ylabel "Total operations per second"
set output 'lab2b_4.png'
set key left bottom
plot \
    "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/$7) \
    title '1 sub-list' with linespoints lc rgb 'red', \
    "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/$7) \
    title '4 sub-list' with linespoints lc rgb 'blue', \
    "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/$7) \
    title '8 sub-list' with linespoints lc rgb 'green', \
    "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/$7) \
    title '16 sub-list' with linespoints lc rgb 'orange'


set title "Plot 5: Aggregated throughput vs. Threads for Spin-lock"
set xlabel "Threads"
set ylabel "Total operations per second"
set output 'lab2b_5.png'
set key left bottom
plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/$7) \
	title '1 sub-list' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/$7) \
	title '4 sub-list' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/$7) \
	title '8 sub-list' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/$7) \
	title '16 sub-list' with linespoints lc rgb 'orange'



