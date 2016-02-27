//
// a single-threaded, multi client(using select), debug webserver - streaming out mjpg.
//  on win, _WIN32 has to be defined, must link against ws2_32.lib (socks on linux are for free)
//

//
// socket related abstractions:
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include <evhttp.h>
#include <event2/event.h>

#include <opencv2/core/core.hpp>
#include <iostream>
using std::cerr;
using std::endl;

using namespace cv;

class MJPGWriter
{
    int sock;
    int maxfd;
    fd_set master;
    int timeout; // master sock timeout, shutdown after timeout millis.
    int quality; // jpeg compression [1..100]

    int _write( int sock, char *s, int len )
    {
        if ( len < 1 ) { len = strlen(s); }
        return ::send( sock, s, len, 0 );
    }

public:

    MJPGWriter(int port = 0)
        : sock(-1)
        , timeout(200000)
        , quality(30)
        , maxfd(-1)
    {
        FD_ZERO( &master );
            if (port)
                open(port);
    }

    ~MJPGWriter()
    {
        release();
    }

    bool release()
    {
        if ( sock != -1 )
            ::shutdown( sock, 2 );
        sock = (-1);
        return false;
    }

    bool open( int port )
    {
        sock = ::socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
        int yes=1;        // for setsockopt() SO_REUSEADDR, below

        struct sockaddr_in address;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_family      = AF_INET;
        address.sin_port        = ::htons(port);
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if ( ::bind( sock, (struct sockaddr *) &address, sizeof(struct sockaddr_in *) ) == -1 )
        {
            cerr << "error : couldn't bind sock "<<sock<<" to port "<<port<<"!" << endl;
            return release();
        }
        if ( ::listen( sock, 10 ) == -1 )
        {
            cerr << "error : couldn't listen on sock "<<sock<<" on port "<<port<<" !" << endl;
            return release();
        }
        FD_SET( sock, &master );
        maxfd = sock+1;
        ::freeaddrinfo(address);
        return true;
    }

    bool isOpened()
    {
      return sock != -1;
    }

    bool write(const Mat &frame)
    {
        fd_set rread = master;
        struct timeval to = {0,timeout};

        if ( ::select( maxfd, &rread, NULL, NULL, &to ) <= 0 )
            return true; // nothing broken, there's just noone listening

        std::vector<uchar>outbuf;
        std::vector<int> params;
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(quality);
        cv::imencode(".jpg", frame, outbuf, params);
        int outlen = outbuf.size();

        for ( int s=0; s<maxfd; s++ )
        {
            if ( !FD_ISSET(s,&rread) )      // ... on linux it's a bitmask ;)
                continue;
            if ( s == sock ) // request on master socket, accept and send main header.
            {
                socklen_t addrlen = sizeof(struct sockaddr *);
                struct sockaddr_in address = {0};
                int      client  = ::accept( sock,  (struct sockaddr *)&address, &addrlen );
                if ( client == -1 )
                {
                    cerr << "error : couldn't accept connection on sock " << sock<< " !" << endl;
                    return false;
                }
                maxfd=( client > maxfd ? client : maxfd);
                FD_SET( client, &master );
                _write( client,"HTTP/1.0 200 OK\r\n",0);
                _write( client,
                    "Server: Mozarella/2.2\r\n"
                    "Accept-Range: bytes\r\n"
                    "Connection: close\r\n"
                    "Max-Age: 0\r\n"
                    "Expires: 0\r\n"
                    "Cache-Control: no-cache, private\r\n"
                    "Pragma: no-cache\r\n"
                    "Content-Type: multipart/x-mixed-replace; boundary=mjpegstream\r\n"
                    "\r\n",0);
                cerr << "new client " << client << endl;
            }
            else // existing client, just stream pix
            {
                char head[400];
                sprintf(head,"--mjpegstream\r\nContent-Type: image/jpeg\r\nContent-Length: %lu\r\n\r\n",outlen);
                _write(s,head,0);
                int n = _write(s,(char*)(&outbuf[0]),outlen);
                cerr << "known client " << s << " " << n << endl;
                if ( n < outlen )
                {
                    cerr << "kill client " << s << endl;
                    ::shutdown(s,2);
                    FD_CLR(s,&master);
                }
            }
        }
        return true;
    }
}
