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
// DgDVec2D.cpp: DgDVec2D class implementation
//
////////////////////////////////////////////////////////////////////////////////

#include <cfloat>
#include <string.h>

#include <dglib/DgBase.h>
#include <dglib/DgDVec2D.h>
#include <dglib/DgDVec3D.h>

////////////////////////////////////////////////////////////////////////////////

const DgDVec2D& DgDVec2D::undefDgDVec2D = DgDVec2D(DBL_MAX, DBL_MAX);

////////////////////////////////////////////////////////////////////////////////
DgDVec2D::DgDVec2D (const DgDVec3D& pt) : x_ (pt.x()), y_ (pt.y()) {}

////////////////////////////////////////////////////////////////////////////////
DgDVec2D&
DgDVec2D::operator= (const DgDVec3D& pt)
{
   x_ = pt.x();
   y_ = pt.y();

   return *this;

} // DgDVec2D& DgDVec2D::operator=

////////////////////////////////////////////////////////////////////////////////
const char*
DgDVec2D::fromString (const char* str, char delimiter)
{
      std::cout << "Miaoooooo4" << std::endl;
    
   char delimStr[2];
   delimStr[0] = delimiter;
   delimStr[1] = '\0';

   char* tmpStr = new char[strlen(str) + 1];
   strcpy(tmpStr, str);

   char* tok;

   // get the x

   tok = strtok(tmpStr, delimStr);
   long double xIn;
    
   if (sscanf(tok, "%LF", &xIn) != 1)
   {
      ::report("DgDVec2D::fromString() invalid value in string " + string(tok),
               DgBase::Fatal);
   }

   // get the y

   tok = strtok(NULL, delimStr);
   long double yIn;
   if (sscanf(tok, "%LF", &yIn) != 1)
   {
      ::report("DgDVec2D::fromString() invalid value in string " + string(tok),
               DgBase::Fatal);
   }

   setX(xIn);
      std::cout << "yIn " << double(yIn) <<std::endl;
      std::cout << "xIn " << double(xIn) <<std::endl;
    
    
   setY(yIn);

   unsigned long offset = (tok - tmpStr) + strlen(tok) + 1;
   delete [] tmpStr;
   if (offset >= strlen(str))
    return 0;
   else
    return &str[offset];
} // const char* DgDVec2D::fromString

const char*
DgDVec2D::fromFloat (long double xIn, long double yIn)
{

   setX(xIn);
   setY(yIn);
    
      std::cout << "double! yIn " << double(yIn) <<std::endl;
      std::cout << "double! xIn " << double(xIn) <<std::endl;
    
    

    return 0;
} // const char* DgDVec2D::fromFloat

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
