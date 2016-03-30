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
		./${executable_name}
	done
	# until $executable; do
	#     sleep 1
	# done
fi

if [ $1 = "client" ]; then
	# declare -a ports=("5556" "5557" "5558" "5559" "5560" "5561" "5562" "5563" "5564")
	file_name="simple_client.cpp"
	executable="./simpleClient 5555"
	executable_name="simpleClient"
	rm ${executable_name}
	g++ ${file_name} -o ${executable_name}
	
	while true; do
		
		# for i in "${ports[@]}"
		for i in 5556 5557 5558 5559 5560 5561 5562 5563 5564
		do
			new_con="$executable_name $i"
			./${new_con}
		   # new_con="./$executable_name $i"
		   # until $new_con; do
		   #     echo "just finished time to wait"
		   # done
		   sleep 5
		done
	    sleep 3600
	done
fi

# for ((i=5555;i<=END;i++)); do
		#     ./${executable_name} i
		#     sleep 2
		# done
