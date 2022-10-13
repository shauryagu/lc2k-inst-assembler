//
//  A.cpp
//  370-1a
//
//  Created by Shaurya Gunderia on 2/14/22.
//

#include "A.hpp"
#include <fstream>
using namespace std;
int main(int argc, char *argv[]){
    ofstream output_stream("print.cpp");
    output_stream << "printf(";
    output_stream << argv[0];
    output_stream << """);";
}
