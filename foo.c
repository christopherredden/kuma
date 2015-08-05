typedef struct foo_struct
{
    int a;
    double aa;
    double b;
} foo;

foo myfunc(int a, int b)
{
    foo f = {a, 5, 6};
    return f;
}

int main()
{
    double d = myfunc(2, 53).b;
}
