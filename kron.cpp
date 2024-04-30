#include "cmdline.h"

double third(double A, double B) {
	return A*A/(1.0 - A*B);
}
double ninth(double A, double B, double C, double D) {
	return (A*A + A*B*third(A+C,B+D) + A*C*third(A+B,C+D))/(1 - A*D);
}
std::discrete_distribution<int> make_seed3(double A, double B, double C, double D) {
/* a  ab  b
 * ac x  bd
 * c  cd  d
 * */
	double a = ninth(A,B,C,D);
	double b = ninth(B,A,D,C);
	double c = ninth(C,A,D,B);
	double d = ninth(D,B,C,A);

	double ab = third(A+B,C+D) - a - b;
	double ac = third(A+C,B+D) - a - c;
	double bd = third(B+D,A+C) - b - d;
	double cd = third(C+D,A+B) - c - d;

	double x = 1.0-a-b-c-d-ab-ac-bd-cd;

//	printf("%f %f %f\n%f %f %f\n%f %f %f\n", a, ab, b, ac, x, bd, c, cd, d);
	return {a, ab, b,
	        ac, x, bd,
	        c, cd, d};
}


int main(int argc, char *argv[]) {
	std::ofstream outf;
	bool START_AT_INDEX_1 = false;
	size_t m = 0, s2 = 0, s3 = 0, s5 = 0, num_vertices = 0; //num_vertices is only used for optargs

	CmdLineBase cmd(argc, argv);
	if (!cmd.parse_args()) {
		cmd.print_help();
		std::exit(1);
	}
	m = cmd.get_num_edges();
	// std::cout <<"m= " << m << "\n";

	s2 = cmd.get_scale_factor();
	num_vertices = pow(2, s2);
	// std::cout << "s2= " << s2 << "; num_vertices="<<num_vertices<<"\n";

	s3 = cmd.get_scale3();
	s5 = cmd.get_scale5();
	//std::cout << "(s3,s5)=" << s3 << " "<<s5<< "\n";
	openFileToWrite(outf, cmd.get_outfile().c_str());
	START_AT_INDEX_1 = cmd.get_start_at_1();
	std::cout << "START_AT_INDEX_1= " << START_AT_INDEX_1 << "\n";
	
	//double A=9./16, B=3./16,C=B, D=1./16;	// GRAPH500 "pure" 75/25 bilinear map
	double A=.57, B=.19, C=B, D=.05;	// GRAPH500	l = ??, m = 16*2^l
	//double A=.42, B=.19, C=B, D=.20;	// CAHepPh	l = 14, m = 237010
	//double A=.48, B=.20, C=.21, D=.11;	// WEBNotreDame	l = 18, m = 1497134

	std::discrete_distribution<int> seed2 {A, B,
	                                       C, D};

	std::discrete_distribution<int> seed3 = make_seed3(A,B,C,D);

	//For now this is wrong, don't use it.
	assert(s5 == 0);
	double F_=0.1600458, G_=0.10313219, H_=0.06645781, I_=0.04282506, J_=0.02759618,
	       K_= 0.01778278, L_=0.01145914, M_=0.0073842, N_=0.00475833;
	std::discrete_distribution<int> seed5{F_, G_, H_, I_, J_,
	                                      G_, H_, I_, J_, K_,
                                              H_, I_, J_, K_, L_,
	                                      I_, J_, K_, L_, M_,
	                                      J_, K_, L_, M_, N_};

	std::random_device rd;
	std::mt19937 g(rd());

	enum class usecase { use2, use3, use5 };
	std::vector<usecase> sequence(s2 + s3 + s5);
	std::fill(sequence.begin(), sequence.begin() + s2, usecase::use2);
	std::fill(sequence.begin() + s2, sequence.begin() + s2 + s3, usecase::use3);
	std::fill(sequence.begin() + s2 + s3, sequence.end(), usecase::use5);

	std::unordered_set <std::string> edge_set;
	size_t num_collisions = 0;

	for (size_t m_i = 0; m_i != m; ) {
		size_t row=0, col=0;
		if(START_AT_INDEX_1 == true){
			row = 1;
			col = 1;
		}
		size_t base = 1;

		std::shuffle(sequence.begin(), sequence.end(), g);
		for (size_t s_i = 0; s_i != sequence.size(); ++s_i) {
			usecase const uc = sequence[s_i];
			if (uc == usecase::use2) {
				int cell = seed2(g);
				row += (cell / 2) * base;
				col += (cell % 2) * base;
				base *= 2;
			}
			else if (uc == usecase::use3) {
				int cell = seed3(g);
				row += (cell / 3) * base;
				col += (cell % 3) * base;
				base *= 3;
			} else if (uc == usecase::use5) {
				int cell = seed5(g);
				row += (cell / 5) * base;
				col += (cell % 5) * base;
				base *= 5;
			}
		}
		assert(row < base);
		assert(col < base);
		assert(base == size_t(pow(2,s2)*pow(3,s3)*pow(5,s5)));
		
		std::string key = std::to_string(row) + "->" + std::to_string(col);
		if (edge_set.find(key) == edge_set.end()) {	
			// Only increment the edge counter if we found a previously unseen edge
			outf << row << " " << col << "\n";
			outf.flush();
			// printf("%lu %lu\n", row, col);
			edge_set.insert(key);
			++m_i;
		} else {
			++num_collisions;
		}
	}
	std::cerr << "Num colisions= " << num_collisions << "\n";
	outf.close();
}


/**
 * 		size_t row, col;
		if (START_AT_INDEX_1 == true){
			row = 1;
			col = 1;
		}
		else{
			row = 0;
			col = 0;
		}
 * 
 */