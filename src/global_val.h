#ifndef GLOBAL_VAL_H
#define GLOBAL_VAL_H
namespace host
{
    class global_val
    {
    private:
        global_val();

    public:
        static char PROXY_IP[256];
        static int  PROXY_PORT;
    };
}

#endif // GLOBAL_VAL_H
