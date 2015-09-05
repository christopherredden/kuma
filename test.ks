let foo:int = 6
let bar = 10
let foobar = 23+24

def add(a:int, b:int)(int, double)
    let result:int = 0
    result = a + b

    return result, 5.5
end

def two(a:int)(int, int)
    return 91, 9
end

let myresult:int = add(foo, bar)

println(myresult)
println(add(9, bar))
println(add(40, 2))
println(add(two(0)))
