####
# End is the end of the range for port nums used
####
END=5565
file_name=""
executable=""

if [ $1 = "client" ]; then
		while true; do
		
		for i in `seq 1 5`;
		do
      echo $i
			new_con="iperf3 -c 54.152.45.231 -i1 --file 1MB_rand.txt"
			$new_con >> results.txt
		   sleep 5
		done
	    sleep 3600
	done
fi

