all: clean compile exec

exec:
	@./teste

compile:
	@gcc main.c ../librarys/vc.c -o teste
	@echo "Executavel gerado"
	
results:
	@open ../results/
	
clean:
	@rm -rf ../results/*.*
	@echo "//\nTodos os ficheiros da pasta results foram eliminados\n//"