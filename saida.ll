@x = global i32 0

@y = global i32 0

define i32 @plus(i32 %4, i32 %5) {
	%a = alloca i32
	store i32 %4, ptr %a
	%b = alloca i32
	store i32 %5, ptr %b

	%res = alloca i32, 0

	%7 = sub i32 10, 3
	%8 = load i32, ptr %a
	%9 = add i32 %8, %7
	store i32 %9, ptr %a

	%10 = load i32, ptr %b
	%11 = add i32 %10, 1
	store i32 %11, ptr %b

	%12 = load i32, ptr %a
	%13 = load i32, ptr %b
	%14 = add i32 %12, %13
	store i32 %14, ptr %plus
	ret i32 %plus

}
define float @minus(i32 %16, i32 %17, float %18) {
	%c = alloca i32
	store i32 %16, ptr %c
	%d = alloca i32
	store i32 %17, ptr %d
	%e = alloca float
	store float %18, ptr %e

	%19 = add i32 0, 4
	%20 = add i32 0, 10
	%21 = call i32 @plus(i32 %19, i32 %20)
	store i32 %21, ptr %c

	%22 = load i32, ptr %y
	ret i32 %22

}
