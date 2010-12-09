/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010 by Joel Andersson, Moritz Diehl, K.U.Leuven. All rights reserved.
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <iostream>
#include <ctime>
#include <casadi/stl_vector_tools.hpp>
#include <ipopt_interface/ipopt_solver.hpp>
#include <casadi/expression_tools.hpp>

using namespace CasADi;
using namespace std;

int main(){
  try{
  
  cout << "program started" << endl;
  

  SXMatrix aa("aa");
  SXMatrix xx("xx",4);

  SXMatrix ff = xx[3]-2; ff *= ff;
  SXMatrix gg;
  gg << aa*aa       - xx[0];
  gg << xx[0]*xx[0] - xx[1];
  gg << xx[1]*xx[1] - xx[2];
  gg << xx[2]*xx[2] - xx[3];
    
  SXMatrix xxaa;
  xxaa << aa << xx;

  SXFunction fff(xxaa,ff);
  SXFunction ggg(xxaa,gg);
    
      
  IpoptSolver solver(fff,ggg);
  solver.setOption("exact_hessian",false);
  solver.setOption("sparse_jacobian",true);
  solver.init();
  
  // Bounds on u and initial condition
  vector<double> uumin(5,-10), uumax(5,10), uusol(5,1);
  solver.input(NLP_LBX).set(uumin);
  solver.input(NLP_UBX).set(uumax);
  solver.input(NLP_X_INIT).set(uusol);
  
  // Upper and lower bounds
  vector<double> ggmin(4,0), ggmax(4,0);
  solver.input(NLP_LBG).set(ggmin);
  solver.input(NLP_UBG).set(ggmax);
  
  // Solve
  solver.solve();
  
  // Get the solution
  solver.output(NLP_X_OPT).get(uusol);
  cout << "optimal solution: " << uusol << endl;

  return 0;
  }
  catch (exception& e){
    cout << "root_test failed: " << e.what() << endl;
    return 1;
  }
  catch (const char * str){
    cerr << "root_test failed (OLD EXCEPTION TYPE!): " << str << endl;
    return 1;
  }
}
