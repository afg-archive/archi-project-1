// http://stackoverflow.com/a/7818394/4661520

#include <iostream>
#include <iomanip>

class null_out_buf : public std::streambuf {
    public:
        virtual std::streamsize xsputn (const char * s, std::streamsize n) {
            return n;
        }
        virtual int overflow (int c) {
            return 1;
        }
};

class null_out_stream : public std::ostream {
    public:
        null_out_stream() : std::ostream (&buf) {}
    private:
        null_out_buf buf;
};

null_out_stream nullstream;
