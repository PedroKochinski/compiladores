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

	%sum = alloca float

	%9 = sub i32 10, 3
	%10 = load i32, ptr %a
	%11 = add i32 %10, %9
	store i32 %11, ptr %a

	%12 = load i32, ptr %b
	%13 = add i32 %12, 1
	store i32 %13, ptr %b

	%14 = load i32 ptr %a
	%15 = load i32 ptr %b
	%16 = icmp sgt i32 %14, %15

	br i1 %16 label %then_0, label %else_0
then_0:
	%17 = load i32, ptr %a
	%18 = add i32 %17, 1
	store i32 %18, ptr %a

	br label %fim_if_0
else_0:
	%19 = load i32, ptr %b
	store i32 %19, ptr %a

	br label %fim_if_0
fim_if_0:
	%20 = load i32, ptr %a
	%21 = load i32, ptr %b
	%22 = add i32 %20, %21
	store i32 %22, ptr %plus
	ret i32 %plus

}
define float @minus(i32 %24, i32 %25, float %26) {
	%c = alloca i32
	store i32 %24, ptr %c
	%d = alloca i32
	store i32 %25, ptr %d
	%e = alloca float
	store float %26, ptr %e
	%minus = alloca float

	%27 = add i32 0, 4
	%28 = add i32 0, 10
	%29 = call i32 @plus(i32 %27, i32 %28)
	store i32 %29, ptr %c

	%30 = load i32, ptr %y
	ret i32 %30

}
