declare i32 @printf(ptr noundef, ...)
declare i32 @__isoc99_scanf(ptr noundef, ...)
@read_int = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@write_int = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@read_float = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@write_float = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

@x = global i32 0
@y = global i32 0

define i32 @modulo(i32 %4, i32 %5) {
entry:
	%a = alloca i32
	store i32 %4, ptr %a
	%b = alloca i32
	store i32 %5, ptr %b
	%modulo = alloca i32

	%i = alloca float

	%k = alloca float

; TERMO: a mod b
	; materializando variavel a (tipo_simb 8)
	%8 = load i32, ptr %a
	; materializando variavel b (tipo_simb 8)
	%9 = load i32, ptr %b
	%10 = srem i32 %8, %9
	; resultado da expressao %10 = srem i32 %8, %9 tipo_simbolo 6
; ENUNCIADO: modulo (tipo_simb 4) := %10 = srem i32 %8, %9 (tipo_simb 6)
	; materializando variavel %10 = srem i32 %8, %9 (tipo_simb 6)
	store i32 %10, ptr %modulo

	%11 = load i32, ptr %modulo
	ret i32 %11
}
define i32 @mdc(i32 %13, i32 %14) {
entry:
	%a = alloca i32
	store i32 %13, ptr %a
	%b = alloca i32
	store i32 %14, ptr %b
	%mdc = alloca i32

	%l = alloca float

	%m = alloca float

	%n = alloca i32

	; materializando variavel b (tipo_simb 8)
	%18 = load i32, ptr %b
	; materializando variavel 0 (tipo_simb 1)
	%19 = icmp eq i32 %18, 0

	br i1 %19, label %then_1, label %else_1
then_1:
; ENUNCIADO: mdc (tipo_simb 4) := a (tipo_simb 8)
	; materializando variavel a (tipo_simb 8)
	%20 = load i32, ptr %a
	store i32 %20, ptr %mdc

	br label %fim_if_1
else_1:
; ENUNCIADO: mdc (tipo_simb 4) := mdc (tipo_simb 2)
	; materializando variavel mdc (tipo_simb 2)
	; chamando funcao mdc (tipo_simb 2)
	; materializando argumento b (tipo_simb 8)
	; materializando variavel b (tipo_simb 8)
	%22 = load i32, ptr %b
	; materializando argumento modulo (tipo_simb 8)
	; materializando variavel modulo (tipo_simb 8)
	%24 = load i32, ptr @modulo
	%25 = call i32 @mdc(i32 %22, i32 %24)
	store i32 %25, ptr %mdc

	br label %fim_if_1
fim_if_1:
	%26 = load i32, ptr %mdc
	ret i32 %26
}
define i32 @main() {
entry:
	%27 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr @x)
	%28 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr @y)
	; materializando variavel mdc (tipo_simb 2)
	; chamando funcao mdc (tipo_simb 2)
	; materializando argumento x (tipo_simb 8)
	; materializando variavel x (tipo_simb 8)
	%31 = load i32, ptr @x
	; materializando argumento y (tipo_simb 8)
	; materializando variavel y (tipo_simb 8)
	%33 = load i32, ptr @y
	%34 = call i32 @mdc(i32 %31, i32 %33)
	%35 = call i32 (ptr, ...) @printf(ptr @write_int, i32 %34)

	ret i32 0
}
