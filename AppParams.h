#ifndef __APP_PARAMS_H
#define __APP_PARAMS_H

#include <sstream>

struct Audio;

struct AppParams {
    std::string deviceName;
    std::string server;
    int port;
    int audioDevice;
    bool autoConnect;
    unsigned char id[2];
    bool recording;
    bool playback;
    std::string file;
    float gain;
    bool probe;
    int test;

    bool valid;

    Audio* audio;

    void usage(std::string exe) {
        std::cout   << "Usage: " << exe << " -d <MindWave device> -s <OSC server>:<port>"
#ifndef AUDIO_NULL
					<< " -a <audio device #>"
#endif
					<< std::endl
					<< std::endl
					<< "Defaults:" << std::endl
					<< "    Mindwave device: "<< deviceName << std::endl
					<< "    Server: localhost" << std::endl
					<< "    Port: 9000" << std::endl
					<< std::endl
                    << "Other:" << std::endl
#ifndef AUDIO_NULL
					<< "   -g <float> : Increase gain of audio wave\n"
                    << "   --probe : List out audio devices and quit\n"
#endif
                    << "   --connect <id> : Connect only to specific MindWave serial number\n"
                    << "   --record <file> : record incoming mindwave stream to <file>\n"
                    << "   --play <file> : Do not connect to MindWave, instead play from <file>\n"
                    //<< "   --test <num> : Play test output\n"
                    //<< std::endl
                    //<< "Tests:\n"
                    //<< " *   0: Send out sawtooth wave on audio device " 
                    << std::endl;
    }

    AppParams( ) :
#if defined(__APPLE__)
        deviceName("/dev/cu.MindWave"),
#else
        deviceName("/dev/ttyUSB0"),
#endif
        server("localhost"),
        port(9000),
        audioDevice(2),
        autoConnect(true),
        recording(false),
        playback(false),
        file(""),
        gain(1.0f),
        probe(false),
        test(-1),
        valid(true)
    {}

    bool init( int argc, char **argv ) {
        int nexti = 0;

    #define FAIL() { usage(argv[0]); valid=false; return false; } 
    #define CNI() { if(i+1 >= argc) FAIL(); } 
    #define PARSE_INTO(str, out) { std::stringstream ss(str); if(!(ss>>out)) FAIL(); }
        for( int i = 1; i < argc; ++i) {
            std::string str(argv[i]);
            if( str == "-d" ) {
                CNI();
                deviceName = argv[++i];
            } else if( str == "-s") {
                CNI();
                server = argv[++i];
                size_t colon = server.find(':');
                if(colon != std::string::npos)
                {
                    std::string portTemp = server.substr(colon+1);
                    server = server.substr(0,colon);
                    PARSE_INTO(portTemp, port);
                }
            } else if( str == "-a") {
                CNI();
                PARSE_INTO(argv[++i], audioDevice);
            } else if( str == "--probe") {
				probe = true;
            } else if( str == "--connect") {
                CNI();
                std::string ID_in = argv[++i];
                if(ID_in.size() != 4)
                {
                    FAIL();
                }
                for(int k = 0; k < 4; ++k)
                {
                    if(!isxdigit(ID_in[k]))
                    {
                        FAIL();
                    }
                }
                autoConnect = false;
                id[0] = hexToByte2(ID_in[0], ID_in[1]);
                id[1] = hexToByte2(ID_in[2], ID_in[3]);
            } else if( str == "--record") {
                CNI();
                file = argv[++i];
                recording = true;
                playback = false;
            } else if( str == "--play") {
                CNI();
                file = argv[++i];
                recording = false;
                playback = true;
            } else if( str == "--test" ) {
                CNI();
                PARSE_INTO(argv[++i], test);
            } else if( str == "-g" ) {
                CNI();
                PARSE_INTO(argv[++i], gain);
            } else {
                FAIL();
            }
        }
        return valid;
    }

    unsigned char hexToByte(char a)
    {
        if('0' << a && a <= '9')
            return (unsigned char)(a-'0');
        if('A' << a && a <= 'F')
            return (unsigned char)(a-'A'+10);
        if('a' << a && a <= 'f')
            return (unsigned char)(a-'a'+10);
        return 0;
    }

    unsigned char hexToByte2(char a, char b)
    {
        return 16*hexToByte(a) + hexToByte(b);
    }
};


#endif