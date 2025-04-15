@x = global i32 0

@y = global i32 0

define i32 @plus(i32 %4, i32 %5) {
	%a = alloca i32, %4
	%b = alloca i32, %5

	%res = alloca i32, 0

	store i32 3, ptr %a

	%7 = load i32, ptr %b
	%8 = add i32 %7, 1
	store i32 %8, ptr %b

	%9 = load i32, ptr %a
	%10 = load i32, ptr %b
	%11 = add i32 %9, %10
	store i32 %11, %plus
	ret i32 %plus

}
define float @minus(i32 %13, i32 %14, float %15) {
	%c = alloca i32, %13
	%d = alloca i32, %14
	%e = alloca float, %15

	%16 = load i32, ptr %d
	%17 = sub i32 %16, 1
	store i32 %17, ptr %c

	%18 = load i32, ptr %y
	ret i32 %18

}
