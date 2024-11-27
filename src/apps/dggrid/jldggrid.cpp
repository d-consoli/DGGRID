
/*******************************************************************************
    Copyright (C) 2023 Kevin Sahr

    This file is part of DGGRID.

    DGGRID is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DGGRID is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
//
// SubOpTransform.cpp: SubOpTransform class implementation
//
////////////////////////////////////////////////////////////////////////////////


#include <jlcxx/jlcxx.hpp>
#include <vector>
#include <iostream>
using namespace std;
#include <dglib/DgConstants.h>
#include <dglib/DgBase.h>
#include "OpBasic.h"



void transfromPoints(std::string meta_file)
{
    std::cout << "Recieved file " << meta_file << std::endl;
    return;
}

std::string greet()
{
    std::cout << "Miaooooo" << std::endl;
    return "Bau Bau";
}

void print_vector(jlcxx::ArrayRef<double,1> vec) {
    std::cout << "Received vector: ";
    for (double val : vec) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
void pause (const string& where)
{
   dgcout << "*** execution paused: " << where << endl;
   dgcout << "*** press ENTER to continue: ";
   scanf("%*c");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void run_dggrid (std::string metaFileName, jlcxx::ArrayRef<double,1> lat, jlcxx::ArrayRef<double,1> lon)
{

   //// build and load the parameter list ////

   dgcout << "** executing DGGRID version " << DGGRID_VERSION;
#ifdef USE_GDAL
   dgcout << " with GDAL version " << string(GDALVersionInfo("VERSION_NUM"));
#else
   dgcout << " without GDAL";
#endif
   dgcout << " **\n";
   dgcout << "type sizes: big int: " << sizeof(long long int) * 8 << " bits / ";
   dgcout << "big double: " << sizeof(long double) * 8 << " bits\n";
   dgcout << "\n** using meta file " << metaFileName << "..." << endl;

   // create the operation object using parameters in the meta file
   OpBasic theOperation(metaFileName);
   theOperation.initialize();

   // echo the parameter list
   dgcout << "* parameter values:\n";
   dgcout << theOperation.pList << endl;

   if (theOperation.mainOp.pauseOnStart)
      pause("parameters loaded");

   // do the operation
   theOperation.execute();
   // theOperation.executeJl(lat, lon);

   // grab the value before the op is cleaned
   bool pauseBeforeExit = theOperation.mainOp.pauseBeforeExit;

   theOperation.cleanupAll();

   if (pauseBeforeExit)
      pause("before exit");

   return;

} // main


JLCXX_MODULE define_julia_module(jlcxx::Module& mod)
{
    mod.method("run_dggrid", &run_dggrid);
    mod.method("transfromPoints", &transfromPoints);
    mod.method("greet", &greet);
    mod.method("print_vector", &print_vector);
}

