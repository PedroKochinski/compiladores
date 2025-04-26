program simples (input, output);
var x,y: integer; var lixo: real;

function plus (a,b: integer): integer;
var res: integer; var sum: real;
begin

    a := a + (10 - 3);
    b := b + 1;
    if (a > b) then
        a := a + 1
    else
        a := b;
    plus := a + b
end;

function minus (c,d: integer; e: real): real;
begin
    c := plus(4, 10);
    minus := y
end;