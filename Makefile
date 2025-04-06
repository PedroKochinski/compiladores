compilador: compilador.y compilador.l tabela_simbolos.c 
	bison -d compilador.y
	flex compilador.l
	clang -o compilador tabela_simbolos.c compilador.tab.c lex.yy.c /opt/homebrew/opt/flex/lib/libfl.a

clean:
	rm -f lex.yy.c compilador compilador.tab.c compilador.tab.h