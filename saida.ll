declare i32 @printf(ptr noundef, ...)
declare i32 @__isoc99_scanf(ptr noundef, ...)
@read_int = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@write_int = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@read_float = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@write_float = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

@x = global i32 0
@y = global i32 0

define void @imprime() {

	; materializando variavel x (tipo_simb 0)
	%5 = load i32, ptr @x
	%6 = call i32 (ptr, ...) @printf(ptr @write_int, i32 %5)

	ret void
}

define void @menos(i32 %8, i32 %9) {
	%a = alloca i32
	store i32 %8, ptr %a
	%b = alloca i32
	store i32 %9, ptr %b

	%i = alloca i32

; ENUNCIADO: i (tipo 0) := 0 (tipo 1)
	; materializando variavel 0 (tipo_simb 1)
	store i32 0, ptr %i

	; materializando variavel a (tipo_simb 8)
	%11 = load i32, ptr %a
	; materializando variavel b (tipo_simb 8)
	%12 = load i32, ptr %b
	%13 = sub i32 %11, %12
; ENUNCIADO: a (tipo 8) := %13 = sub i32 %11, %12 (tipo 6)
	; materializando variavel %13 = sub i32 %11, %12 (tipo_simb 6)
	store i32 %13, ptr %a

	ret void
}

define i32 @main() {
	%14 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr @x)
	%15 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr @y)
	; chamando funcao menos (tipo_simb 3)
	; materializando variavel x (tipo_simb 8)
	%17 = load i32, ptr @x
	; materializando variavel y (tipo_simb 8)
	%19 = load i32, ptr @y
	call void @menos(i32 %17, i32 %19)
	; chamando funcao imprime (tipo_simb 3)
	call void @imprime()
	ret i32 0
}
