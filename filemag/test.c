int main(void)
{
    FileMan_InitDirectory();

    const char *filename = "test.txt";
    const uint8_t testdata[] = "Hello, FileMan!";
    const uint32_t datalen = sizeof(testdata);

    printf("Writing file...\n");
    if (!FileMan_WriteFile(filename, testdata, datalen))
        printf("Write failed!\n");
    else
        printf("Write success.\n");

    uint8_t read_buffer[64];
    uint32_t read_len;
    printf("Reading file...\n");

    if (FileMan_ReadFile(filename, read_buffer, &read_len))
    {
        printf("Read success. Data:\n");
        for (uint32_t i = 0; i < read_len; i++)
            putchar(read_buffer[i]);
        putchar('\n');
    }
    else
        printf("Read failed!\n");

    return 0;
}
