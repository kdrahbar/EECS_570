file_name=""
executable=""
declare -a ports=("5556" "5557" "5558" "5559" "5560" "5561" "5562" "5563" "5564")
file_name="simple_client.cpp"
executable_name="simpleClient"
rm ${executable_name}
g++ ${file_name} -o ${executable_name}
	
	while true; do
		for i in "${ports[@]}"
		do
			new_con="$executable_name $i"
			./${new_con}
		   sleep 5
		done
	    sleep 3600
	done

# for ((i=5555;i<=END;i++)); do
		#     ./${executable_name} i
		#     sleep 2
		# done
