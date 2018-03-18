
void start_main()
{
    ata_read(0, 1, (void *)0x2000);
    ata_write((void*)0x7c00, 1, 1);
}