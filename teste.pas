program simples (input, output);
var x,y: integer; var lixo: real;

function plus (a,b: integer): integer;
var res, i: integer; var sum: real;
begin
    read(res, sum, x);
    a := a + (10 - 3);
    b := b + 1;
    if (a > b) then
        a := a + 1
    else
        a := b;
    i := 0;
    while (i <= 10) do
    begin
        a := a + 1;
        i := i + 1
    end;
    write(a + b);
    plus := a + b
end;

