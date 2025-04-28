declare i32 @printf(ptr noundef, ...)
declare i32 @scanf(ptr noundef, ...)
@read_int = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@write_int = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@read_float = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@write_float = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1

@x = global i32, 0
@y = global i32, 0

@lixo = global float, 0

define i32 @plus(i32 %5, i32 %6) {
entry:
	%a = alloca i32
	store i32 %5, ptr %a
	%b = alloca i32
	store i32 %6, ptr %b
	%plus = alloca i32

	%res = alloca i32

	%i = alloca i32

	%sum = alloca float

	%10 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr %res)
	%11 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr %sum)
	%12 = call i32 (ptr, ...) @__isoc99_scanf(ptr @read_int, ptr @x)
	%13 = sub i32 10, 3
	%14 = load i32 ptr %a
	%15 = add i32 %14, %13
	store i32 %15, ptr %a

	%16 = load i32 ptr %b
	%17 = add i32 %16, 1
	store i32 %17, ptr %b

	%18 = load i32 ptr %a
	%19 = load i32 ptr %b
	%20 = icmp sgt i32 %18, %19

	br i1 %20 label %then_0, label %else_0
then_0:
	%21 = load i32 ptr %a
	%22 = add i32 %21, 1
	store i32 %22, ptr %a

	br label %fim_if_0
else_0:
	%23 = load i32 ptr %b
	store i32 %23, ptr %a

	br label %fim_if_0
fim_if_0:
	store i32 0, ptr %i

	br label %teste_while_0
teste_while_0:
	%24 = load i32 ptr %i
	%25 = icmp sle i32 %24, 10

	br i1 %25 label %while_0, label %fim_while_0
while_0:
	%26 = load i32 ptr %a
	%27 = add i32 %26, 1
	store i32 %27, ptr %a

	%28 = load i32 ptr %i
	%29 = add i32 %28, 1
	store i32 %29, ptr %i

	br label %teste_while_0
fim_while_0:
	%30 = load i32 ptr %a
	%31 = load i32 ptr %b
	%32 = add i32 %30, %31
	%34 = call i32 (ptr, ...) @printf(ptr @write_int, i32 %32)

	%35 = load i32 ptr %a
	%36 = load i32 ptr %b
	%37 = add i32 %35, %36
	store i32 %37, ptr %plus
	ret i32 %plus
}

