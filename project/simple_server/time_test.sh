END=5565
file_name=""
executable=""
if [ $1 = "server" ]; then
	file_name="simple_server.cpp"
	executable="./simpleServer"
	executable_name="simpleServer"
	rm ${executable_name}
	g++ ${file_name} -o ${executable_name}
	
	until $executable; do
	    sleep 1
	done
fi

if [ $1 = "client" ]; then
	file_name="simple_client.cpp"
	executable="./simpleClient"
	executable_name="simpleClient"
	rm ${executable_name}
	g++ ${file_name} -o ${executable_name}
	
	until $executable; do
		for ((i=5555;i<=END;i++)); do
		    ./${executable_name} i
		    sleep 2
		done
	    sleep 3600
	done
fi

