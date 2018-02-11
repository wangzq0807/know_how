
void start_main() 
{
    __asm__ volatile ( \
        "xor %%eax, %%eax \n" \
        "xor %%ebx, %%ebx \n" \
        : : :"eax", "ebx"
    );
}