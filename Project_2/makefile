default: worker diseaseAggregator
diseaseAggregator: main.o read_write.o handler.o bucket_list.o hash.o avl.o max_heap.o record.o statistics.o
	gcc main.o read_write.o handler.o bucket_list.o hash.o avl.o max_heap.o record.o statistics.o -o diseaseAggregator
worker: worker.o avl.o record.o bucket_list.o max_heap.o list.o hash.o statistics.o read_write.o handler_worker.o
	gcc worker.o avl.o record.o bucket_list.o max_heap.o list.o hash.o statistics.o read_write.o handler_worker.o -o worker 
main.o:
	gcc -c -o main.o main.c
worker.o:
	gcc -c -o worker.o worker.c
handler_worker.o:
	gcc -c -o handler_worker.o handler_worker.c
handler.o:
	gcc -c -o handler.o handler.c
read_write.o:
	gcc -c -o read_write.o read_write.c
statistics.o:
	gcc -c -o statistics.o statistics.c
hash.o:
	gcc -c -o hash.o hash.c
list.o:
	gcc -c -o list.o list.c
record.o: 
	gcc -c -o record.o record.c
bucket_list.o: 
	gcc -c -o bucket_list.o bucket_list.c
avl.o: 
	gcc -c -o avl.o avl.c
max_heap.c: 
	gcc -c -o max_heap.o max_heap.c
clean:
	rm worker.o avl.o record.o main.o bucket_list.o max_heap.o list.o hash.o statistics.o read_write.o handler_worker.o handler.o worker diseaseAggregator Pipe_* log_file.*
