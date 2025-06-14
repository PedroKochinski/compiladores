program IfAninhadoSomenteNumeros(input, output);
var idade: integer;
var classificacao: integer;
begin
  read(idade);

  if idade >= 0 then
    if idade < 12 then
      classificacao := 1 
    else
      if idade < 18 then
        classificacao := 2  
      else
        if idade < 60 then
          classificacao := 3 
        else
          classificacao := 4  
  else
    classificacao := 0;
  if classificacao = 0 then
    classificacao := 5
  else
    classificacao := classificacao;
  write(classificacao)
end.
