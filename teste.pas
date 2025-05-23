program simples (input, output);
var x,y: integer;
var z: real;

procedure plus (a,b: integer);
var plus: integer;
begin
    plus := a + b
end;

begin
    read(x);
    y := 3;
    z := 4;
    x := x + 1;
    x := y * x;
    z := 2 * z;
    plus(x,y);
    write(x);
    write(y);
    write(z)
end.