let mystring = "kjasdjkads"
let mystring2 = mystring
let mystring3
let mystring4
var foowho

mystring99 = "99aa+():-,=var"
mystring3 = "askjfkasj:f"
mystring4 = mystring3

let foo:int = 6
let bar = 10
let foobar = 23+24
let mybar:double = 4.4

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
