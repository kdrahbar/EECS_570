####
# End is the end of the range for port nums used
####
END=5565
file_name=""
executable=""
if [ $1 = "server" ]; then
	file_name="simple_server.cpp"
	executable="./simpleServer"
	executable_name="simpleServer"
	rm ${executable_name}
	g++ ${file_name} -o ${executable_name}
	
	while true; do
		for i in 1 2 3 4 5 6 7 8 9
		do
			./${executable_name}
		done
		sleep 120
	done
fi

if [ $1 = "client" ]; then
	file_name="simple_client.cpp"
	executable="./simpleClient 5555"
	executable_name="simpleClient"
	rm ${executable_name}
	g++ ${file_name} -o ${executable_name}
	
	while true; do
		
		for i in 5556 5557 5558 5559 5560 5561 5562 5563 5564
		do
			new_con="$executable_name $i"
			./${new_con}
		   sleep 5
		done
	    sleep 3600
	done
fi
