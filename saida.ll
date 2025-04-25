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

	%14 = load i32, ptr %a
	%15 = load i32, ptr %b
	%16 = add i32 %14, %15
	store i32 %16, ptr %plus
	ret i32 %plus

}
define float @minus(i32 %18, i32 %19, float %20) {
	%c = alloca i32
	store i32 %18, ptr %c
	%d = alloca i32
	store i32 %19, ptr %d
	%e = alloca float
	store float %20, ptr %e
	%minus = alloca float

	%21 = add i32 0, 4
	%22 = add i32 0, 10
	%23 = call i32 @plus(i32 %21, i32 %22)
	store i32 %23, ptr %c

	%24 = load i32, ptr %y
	ret i32 %24

}
