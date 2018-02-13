
void sys_call(int n)
{
    __asm__ volatile (
        "int $0x80\n"\
        ::"eax"(n)
    );
}

void start_main() 
{
    sys_call('w');
    sys_call('a');
    sys_call('n');
    sys_call('g');
    sys_call('z');
    sys_call('q');
}
