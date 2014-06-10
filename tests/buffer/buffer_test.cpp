#include <Buffer.h>

#if 0
class muffer : public std::vector<char>
{
public:
    muffer(int size) : std::vector<char>(size) { std::cout << "created \n"; }
    muffer(const muffer& m) { std::cout << "copied \n"; }
    muffer& operator = (const muffer& m) { std::cout << "copied 1\n"; return *this; }
    ~muffer() { std::cout << "destroyed \n"; }
};
#endif

int main()
{
#if 0    
    char d1[128];
    std::vector<char> d2(128);
    boost::array<char, 128> d3;
    
    
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    
    std::cout << d2.size() << std::endl;

    boost::asio::mutable_buffers_1 buf = boost::asio::buffer(d2);
    std::cout << boost::asio::buffer_size(buf) << std::endl;
    std::cout << boost::asio::buffer_cast<char*>(buf) << std::endl;
    
    
//    socket.receive(buf);
    //std::cout << boost::asio::buffer_size(d1) << std::endl;
    //std::cout << boost::asio::buffer_size(d2) << std::endl;
    //std::cout << boost::asio::buffer_size(d3) << std::endl;
    
    
    std::vector<boost::asio::const_buffer> bufs2;
    bufs2.push_back(boost::asio::buffer(d1));
    bufs2.push_back(boost::asio::buffer(d2));
    bufs2.push_back(boost::asio::buffer(d3));
    
    /*
    std::vector<char> vdata(256);
    vdata.at(0) = 'k';
    vdata.at(1) = 'o';
    vdata.at(2) = 'k';
    vdata.at(3) = 'o';
    boost::asio::mutable_buffers_1 buf1 = boost::asio::buffer(vdata);
    std::cout << boost::asio::buffer_size(buf1) << std::endl;
    std::cout << boost::asio::buffer_cast<const char*>(buf1) << std::endl;
    vdata.clear();
    std::cout << vdata.size() << std::endl;
    std::cout << boost::asio::buffer_size(buf1) << std::endl;
    //std::cout << vdata.at(0) << std::endl;
    //vdata.at(0) = 'b';
    //vdata.at(1) = 'b';
    //vdata.at(2) = 'b';
    //vdata.at(3) = 'b';
    std::cout << boost::asio::buffer_cast<const char*>(buf1) << std::endl;
    */
    
    muffer m(256);
    /*
    m.at(0) = 'k';
    m.at(1) = 'o';
    m.at(2) = 'k';
    m.at(3) = 'o';
    */
    
    boost::asio::mutable_buffers_1 buf1 = boost::asio::buffer(m);
    std::cout << boost::asio::buffer_size(buf1) << std::endl;
    std::cout << boost::asio::buffer_cast<const char*>(buf1) << std::endl;
    //m.clear();
    
    std::cout << m.data() << std::endl;
#endif
    
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    Buffer b1(100);

    unsigned char* ptr1 = b1.Get<unsigned char*>();
    if (ptr1 != NULL)
    {
        std::cout << "data is not empty \n";
    }
    //BufferSequence b;
    //b.push_back(b1);
    std::cout << b1.Size() << std::endl;
    
    Buffer b12 = (b1 + 70);
    std::cout << b12.Size() << std::endl;
    Buffer b3;
    std::cout << b3.Size() << std::endl;
    unsigned char* ptr = b3.Get<unsigned char*>();
    
    
    if (ptr == NULL)
    {
        std::cout << "data is empty \n";
    }
    
    //socket.receive(b);
    
    Buffer test(10);
    unsigned char* pbuf = test.Get<unsigned char*>();
    pbuf[0] = 'k';
    pbuf[1] = 'o';
    pbuf[2] = 'k';
    pbuf[3] = 'o';
    pbuf[4] = '\0';
    std::cout << pbuf << std::endl;
    Buffer bom = (2 + test);
    std::cout << bom.Get<unsigned char*>() << std::endl;
    pbuf[2] = 'n';
    pbuf[3] = 'b';
    std::cout << bom.Get<unsigned char*>() << std::endl;
    
    
    
    BufferSequence b;
    b.push_back(bom);
    
    socket.receive(b);
    
    return 0;
    
    
}

