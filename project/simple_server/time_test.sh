END=10
file_name=""
executable=""
if [ $1 = "server" ]; then
	file_name="simple_server.cpp"
	executable="./simpleServer"
	executable_name="simpleServer"
	g++ ${file_name} -o ${executable_name}
	
	until $executable; do
	    sleep 3
	done
fi

if [ $1 = "client" ]; then
	file_name="simple_client.cpp"
	executable="./simpleClient"
	executable_name="simpleClient"
	g++ ${file_name} -o ${executable_name}
	
	until $executable; do
		for ((i=1;i<=END;i++)); do
		    ./${executable_name}
		    sleep 17
		done
	    sleep 3600
	done
fi

