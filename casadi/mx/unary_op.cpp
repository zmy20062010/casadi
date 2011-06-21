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

#include "unary_op.hpp"
#include "mx_tools.hpp"
#include <vector>
#include <sstream>

using namespace std;

namespace CasADi{

UnaryOp::UnaryOp(Operation op_, MX x) : op(op_){
  // Put a densifying node in between if necessary
  if(!SX::f00_is_zero_[op]){
    makeDense(x);
  }
  
  setDependencies(x);
  setSparsity(x->sparsity());
}

UnaryOp* UnaryOp::clone() const{
  return new UnaryOp(*this);
}

void UnaryOp::print(std::ostream &stream, const std::vector<std::string>& args) const{
  casadi_math<double>::print[op](stream,args.at(0),"nan");
}

void UnaryOp::evaluate(const VDptr& input, Dptr& output, const VVDptr& fwdSeed, VDptr& fwdSens, const VDptr& adjSeed, VVDptr& adjSens, int nfwd, int nadj){
  double nan = numeric_limits<double>::quiet_NaN();
  if(nfwd==0 && nadj==0){
    // No sensitivities
    for(int i=0; i<size(); ++i)
      casadi_math<double>::fun[op](input[0][i],nan,output[i]);
    
  } else {
    // Sensitivities
    double tmp[2];  // temporary variable to hold value and partial derivatives of the function
    for(int i=0; i<size(); ++i){
      // Evaluate and get partial derivatives
      casadi_math<double>::fun[op](input[0][i],nan,output[i]);
      casadi_math<double>::der[op](input[0][i],nan,output[i],tmp);
      
      // Propagate forward seeds
      for(int d=0; d<nfwd; ++d){
        fwdSens[d][i] = tmp[0]*fwdSeed[0][d][i];
      }

      // Propagate adjoint seeds
      for(int d=0; d<nadj; ++d){
        adjSens[0][d][i] += adjSeed[d][i]*tmp[0];
      }
    }
  }
}

void UnaryOp::evaluateSX(const std::vector<SXMatrix*> &input, SXMatrix& output){
  // Do the operation on all non-zero elements
  const vector<SX> &xd = input[0]->data();
  vector<SX> &od = output.data();
  
  for(int el=0; el<size(); ++el){
    casadi_math<SX>::fun[op](xd[el],0,od[el]);
  }
}
} // namespace CasADi

