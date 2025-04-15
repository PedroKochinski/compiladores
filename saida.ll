@x = global i32 0

@y = global i32 0

define i32 @plus(i32 %4, i32 %5) {
	%a = alloca i32
	store i32 %4, ptr %a
	%b = alloca i32
	store i32 %5, ptr %b

	%res = alloca i32, 0

	%7 = load i32, ptr %a
	%8 = add i32 %7, 10
	store i32 %8, ptr %a

	%9 = load i32, ptr %b
	%10 = add i32 %9, 1
	store i32 %10, ptr %b

	%11 = load i32, ptr %a
	%12 = load i32, ptr %b
	%13 = add i32 %11, %12
	store i32 %13, ptr %plus
	ret i32 %plus

}
define float @minus(i32 %15, i32 %16, float %17) {
	%c = alloca i32
	store i32 %15, ptr %c
	%d = alloca i32
	store i32 %16, ptr %d
	%e = alloca float
	store float %17, ptr %e

	%18 = call i32 @plus()
	store i32 %18, ptr %c

	%19 = load i32, ptr %y
	ret i32 %19

}
