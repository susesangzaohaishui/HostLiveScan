#ifndef __NET_I_QUEUE_SERVICE_INCLUDED__
#define __NET_I_QUEUE_SERVICE_INCLUDED__

namespace host
{

    // Virtual interface to be exposed by object that want to be notified
    // about events on file descriptors.

    struct i_queue_service
    {
        virtual ~i_queue_service() {}

        // Called by I/O thread when file descriptor is ready for reading.
        virtual void  add_queue_item(int identifier, void * const buf, int size) = 0;
    };

}
#endif
