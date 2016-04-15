for i in {1..100}
do
	strace -c -o "ping_output/output$i.txt" ping 104.154.88.118
done
