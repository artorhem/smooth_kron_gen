#ifndef CMDLINE_H
#define CMDLINE_H
#include <algorithm>
#include <iostream>
#include <random>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <unordered_set>
#include <string>
#include <fstream>
#include <cstring>
#include <cmath>
#include <getopt.h>

void openFileToWrite( std::ofstream& input_file, const char* file_name ) {
	input_file.open(file_name);
	if( !input_file )
		throw std::runtime_error( "Failed to open specified file." );
}


class CmdLineBase
{
   protected:
    int argc_;
    char **argv_;

		std::string argstr =  "e:s:o:tfih";

    std::string help_string;
    
		std::ofstream outf;
		bool start_at_1 = false;
		size_t scale3 = 0, scale5 = 0;
		size_t nEdges = 0, scale_factor = 0;
    std::string outfile = "out.txt";


   public:
    CmdLineBase(int argc, char **argv) : argc_(argc), argv_(argv)
    {
        help_string=  "Required command line arguments:\n\
        -e : number of edges (ex. -e 1021)\n\
        -s : scale factor. 2^s vertices will be used.  (ex. -n 5)\n\
        -o : output file (ex. -o out.txt)\n";
    
      help_string += "Additional arguments:\n\
      -i (default: 0). E.g., -start_at_index_1\n\
      -t Scale 3 parameter; default= 0 (ex. -t 2)\n\
      -f parameter; default= 0 (ex. -f 8)\n";
      if (argc_ == 1)
      {
            std::cout << "No arguments provided. Use -h for help." << std::endl;
            std::exit(1);
      }
    }
		size_t get_scale_factor() { return scale_factor; }
		size_t get_num_edges() { return nEdges; }
		size_t get_scale3() { return scale3; }
		size_t get_scale5() { return scale5; }
		bool get_start_at_1() { return start_at_1; }
    std::string get_outfile() { return outfile; }

    bool parse_args()
    {
        signed char opt_c;
        while ((opt_c = getopt(argc_, argv_, argstr.c_str())) != -1)
            handle_args(opt_c, optarg);

        if(nEdges == 0) {
						std::cout << "Number of edges not specified. Use -h for help." << std::endl;
						return false;
				}
				if(scale_factor== 0) {
						std::cout << "Scale factor not specified. Use -h for help." << std::endl;
						return false;
				}
				if(scale_factor >=nEdges){
						std::cout << "Number of vertices should be less than the number of edges. Use -h for help." << std::endl;
						return false;
				}
        if(outfile == "") {
            std::cout << "Output file not specified. Using out.txt" << std::endl;
        }

        return true;
		}
    

    void handle_args(signed char opt, char *opt_arg)
    {
        switch (opt)
        {
            case 'h':
              print_help();
							break;
						case 'e':
							nEdges = std::stoull(std::string(opt_arg));
							break;
						case 's':
							scale_factor = std::stoull(std::string(opt_arg));
							break;
            case 'i':
                start_at_1 = true;
                break;
						case 't':
							scale3 = std::stoull(std::string(opt_arg));
							break;
						case 'f':
							scale5 = std::stoull(std::string(opt_arg));
							break;
						case 'o':
							outfile = std::string(opt_arg);
							break;
        }
    }

    void print_help()
    {
        std::cout << help_string << std::endl;
        std::exit(0);
    }

};

#endif