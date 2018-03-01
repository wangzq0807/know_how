
void swap_f(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void start_main() 
{
    int a = 3;
    int b = 4;
    swap_f(&a, &b);
}