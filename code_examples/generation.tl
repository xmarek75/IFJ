require "ifj21"

function foo()



    local x: string = "asd"
    if x == "sad" then
    write("Faktorial nelze spocitat\n")
    else
    end

    local a: string = substr("abcdef", 0, 3)
    write("a:", a, "\n")

    local b: string = substr("abcdef", 1, 7)
    write("b:", b, "\n")

    local c: string = substr("abcdef", 2, 1)
    write("c:", c, "\n")

    local d: string = substr("abcdef", 2, 2)
    write("d:", d, "\n")

    local e: string = substr("abcdef", 1, 6)
    write("e:", e, "\n")

    local f: string = substr("abcdef", 1, 1)
    write("f:", f, "\n")

    local g: string = substr("abcdef", 6, 6)
    write("g:", g, "\n")
end


foo()

