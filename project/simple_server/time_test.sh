file_name=""
executable=""
if [ $1 = "server" ]; then
	file_name="simple_server.cpp"
	executable="simpleServer"
fi
if [ $1 = "client" ]; then
	file_name="simple_client.cpp"
	executable="simpleClient"
fi
while true
do 
    echo "g++ ${file_name} -o ${executable}"
    echo "./${executable}"
    sleep 5
done