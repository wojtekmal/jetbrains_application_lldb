// DO NOT CHANGE OR TESTS WILL BREAK
int global_int_to_watch = 0;

int main()
{
    for (int i = 0; i < 5; i++)
    {
        global_int_to_watch--;
    }
}