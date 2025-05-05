declare i32 @printf(ptr noundef, ...)
declare i32 @__isoc99_scanf(ptr noundef, ...)
@read_int = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@write_int = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@read_float = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@write_float = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

@x = global i32 0
@y = global i32 0

@z = global float 0.000000

define i32 @plus(i32 %5, i32 %6) {
entry:
	%a = alloca i32
	store i32 %5, ptr %a
	%b = alloca i32
	store i32 %6, ptr %b
	%plus = alloca i32

	; materializando variavel a (tipo_simb 7)
	%7 = load i32, ptr %a
	; materializando variavel b (tipo_simb 7)
	%8 = load i32, ptr %b
	%9 = add i32 %7, %8
; ENUNCIADO: plus (tipo 4) := %9 = add i32 %7, %8 (tipo 5)
	; materializando variavel %9 = add i32 %7, %8 (tipo_simb 5)
	store i32 %9, ptr %plus

	%10 = load i32, ptr %plus
	ret i32 %10
}

define i32 @main() {
	%11 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr @x)
; ENUNCIADO: y (tipo 0) := 3 (tipo 1)
	; materializando variavel 3 (tipo_simb 1)
	store i32 3, ptr @y

; ENUNCIADO: z (tipo 0) := 4 (tipo 1)
	; materializando variavel 4 (tipo_simb 1)
	store i32 4, ptr @z

	; materializando variavel x (tipo_simb 0)
	%12 = load i32, ptr @x
	; materializando variavel 1 (tipo_simb 1)
	%13 = add i32 %12, 1
; ENUNCIADO: x (tipo 0) := %13 = add i32 %12, 1 (tipo 5)
	; materializando variavel %13 = add i32 %12, 1 (tipo_simb 5)
	store i32 %13, ptr @x

	; materializando variavel y (tipo_simb 0)
	%14 = load i32, ptr @y
	; materializando variavel x (tipo_simb 0)
	%15 = load i32, ptr @x
	%16 = mul i32 %14, %15
; ENUNCIADO: x (tipo 0) := %16 = mul i32 %14, %15 (tipo 5)
	; materializando variavel %16 = mul i32 %14, %15 (tipo_simb 5)
	store i32 %16, ptr @x

	; materializando variavel 2 (tipo_simb 1)
	; materializando variavel z (tipo_simb 0)
	%17 = load i32, ptr @z
	%18 = mul i32 2, %17
; ENUNCIADO: z (tipo 0) := %18 = mul i32 2, %17 (tipo 5)
	; materializando variavel %18 = mul i32 2, %17 (tipo_simb 5)
	store i32 %18, ptr @z

; ENUNCIADO: x (tipo 0) := plus (tipo 2)
	; materializando variavel plus (tipo_simb 2)
	; chamando funcao plus
	; materializando variavel x (tipo_simb 0)
	%20 = load i32, ptr @x
	; materializando variavel y (tipo_simb 0)
	%22 = load i32, ptr @y
	%23 = call i32 @plus(i32 %20, i32 %22)
	store i32 %23, ptr @x

	; materializando variavel x (tipo_simb 0)
	%25 = load i32, ptr @x
	%26 = call i32 (ptr, ...) @printf(ptr @write_int, i32 %25)

	; materializando variavel y (tipo_simb 0)
	%28 = load i32, ptr @y
	%29 = call i32 (ptr, ...) @printf(ptr @write_int, i32 %28)

	; materializando variavel z (tipo_simb 0)
	%31 = load i32, ptr @z
	%32 = call i32 (ptr, ...) @printf(ptr @write_int, i32 %31)

	ret i32 0
}
