program simples (input, output);
var x,y: integer;
function plus (a,b: integer): integer;
begin
    a := a + 1 * 3 - 2;
    b := b + 1;
    plus := a + b
end;
function minus(c,d: integer; e: real): real;
begin
    c := plus(c, 1);
    d := plus(d, 1);
    minus := c - d
end;
