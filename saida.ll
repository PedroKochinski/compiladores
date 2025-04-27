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

	%10 = sub i32 10, 3
	%11 = load i32, ptr %a
	%12 = add i32 %11, %10
	store i32 %12, ptr %a

	%13 = load i32, ptr %b
	%14 = add i32 %13, 1
	store i32 %14, ptr %b

	%15 = load i32 ptr %a
	%16 = load i32 ptr %b
	%17 = icmp sgt i32 %15, %16

	br i1 %17 label %then_0, label %else_0
then_0:
	%18 = load i32, ptr %a
	%19 = add i32 %18, 1
	store i32 %19, ptr %a

	br label %fim_if_0
else_0:
	%20 = load i32, ptr %b
	store i32 %20, ptr %a

	br label %fim_if_0
fim_if_0:
	store i32 0, ptr %i

	br label %teste_while_0
teste_while_0:
	%21 = load i32 ptr %i
	%22 = icmp sle i32 %21, 10

	br i1 %22 label %while_0, label %fim_while_0
while0:
	%23 = load i32, ptr %a
	%24 = add i32 %23, 1
	store i32 %24, ptr %a

	%25 = add i32 æ¦k, 1
	store i32 %25, ptr %i

}

	br label %teste_while_0
fim_while_0:
	%26 = load i32, ptr %a
	%27 = load i32, ptr %b
	%28 = add i32 %26, %27
	store i32 %28, ptr %plus
	ret i32 %plus

}

define float @minus(i32 %30, i32 %31, float %32) {
	%c = alloca i32
	store i32 %30, ptr %c
	%d = alloca i32
	store i32 %31, ptr %d
	%e = alloca float
	store float %32, ptr %e
	%minus = alloca float

	%33 = add i32 0, 4
	%34 = add i32 0, 10
	%35 = call i32 @plus(i32 %33, i32 %34)
	store i32 %35, ptr %c

	%36 = load i32, ptr %y
	ret i32 %36

}

