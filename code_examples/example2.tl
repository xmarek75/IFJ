-- Program 2: Vypocet faktorialu (rekurzivne)
require "ifj21"

global factorial : function (integer) : integer
=======

function factorial(n : integer) : integer
  local n1 : integer = n - 1 + 2
  --local n1 : integer = n - 1 + 2
  n1 = 5
  if n < 2 then

    return 1
  else

    local tmp : integer = factorial(n1)
    factorial(n1)
    --factorial(n1)

    return n * tmp
  end
end
function main()
  write("Zadejte cislo pro vypocet faktorialu: ")
  local a : integer = readi()
  local b : integer
  local c : integer = c + 1 --melo by to vyhodit error nejspis
  if a ~= b then
        if a < b then
            write("Faktorial nejde spocitat!", "\n")
        else
            local vysl : integer = factorial(a)
            write("Vysledek je ", vysl, "\n")
    end
  else
     write("Chyba pri nacitani celeho cisla!\n")
  end
end

main()
