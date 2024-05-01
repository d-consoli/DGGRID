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
// DgZ7StringRF.cpp: DgZ7StringRF class implementation
//
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <climits>
#include <cfloat>
#include <string.h>

#include <dglib/DgZ7StringRF.h>
#include <dglib/DgIDGGBase.h>
#include <dglib/DgIDGGSBase.h>
#include <dglib/DgIVec3D.h>

////////////////////////////////////////////////////////////////////////////////
const DgZ7StringCoord DgZ7StringCoord::undefDgZ7StringCoord("99");

////////////////////////////////////////////////////////////////////////////////
const char*
DgZ7StringRF::str2add (DgZ7StringCoord* add, const char* str,
                         char delimiter) const
{
   if (!add) add = new DgZ7StringCoord();

   char delimStr[2];
   delimStr[0] = delimiter;
   delimStr[1] = '\0';

   char* tmpStr = new char[strlen(str) + 1];
   strcpy(tmpStr, str);
   char* tok = strtok(tmpStr, delimStr);

   add->setValString(tok);

   unsigned long offset = strlen(tok) + 1;
   delete[] tmpStr;

   if (offset >= strlen(str)) return 0;
   else return &str[offset];

} // const char* DgZ7StringRF::str2add

////////////////////////////////////////////////////////////////////////////////
DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter
                (const DgRF<DgQ2DICoord, long long int>& from,
                 const DgRF<DgZ7StringCoord, long long int>& to)
        : DgConverter<DgQ2DICoord, long long int, DgZ7StringCoord, long long int> (from, to),
          pIDGG_ (NULL), effRes_ (0), effRadix_ (0)
{
   pIDGG_ = dynamic_cast<const DgIDGGBase*>(&fromFrame());
   if (!pIDGG_) {
      report("DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter(): "
         " fromFrame not of type DgIDGGBase", DgBase::Fatal);
   }

   const DgZ7StringRF* zRF = dynamic_cast<const DgZ7StringRF*>(&toFrame());
   if (!zRF) {
      report("DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter(): "
         " toFrame not of type DgZ7StringRF", DgBase::Fatal);
   }

   if (IDGG().dggs()->aperture() != zRF->aperture() || IDGG().res() != zRF->res()) {
       report("DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter(): "
         "fromFrame and toFrame apertures or resolutions do not match", DgBase::Fatal);
   }

   if (IDGG().gridTopo() != Hexagon || IDGG().dggs()->aperture() != 7) {
      report("DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter(): "
         "only implemented for aperture 7 hexagon grids", DgBase::Fatal);
   }

   effRadix_ = 7;

   // effRes_ is the number of Class I resolutions
   effRes_ = (IDGG().res() + 1) / 2;

} // DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter

////////////////////////////////////////////////////////////////////////////////
DgZ7StringCoord
DgQ2DItoZ7StringConverter::convertTypedAddress (const DgQ2DICoord& addIn) const
{
   //printf("DgQ2DItoZ7StringConverter::convertTypedAddress\n");
/*
   // check for res 0/base cell
    if (res == 0) {
        H3_SET_BASE_CELL(h, _faceIjkToBaseCell(fijk));
        return h;
    }
*/
    // we need to find the correct base cell for this H3 index;
    // start with the passed in quad and resolution res ijk coordinates
    // in that quad's coordinate system
    DgIVec3D ijk = addIn.coord;
    int baseCell = addIn.quadNum;
    int res = IDGG.res();
    bool isClassIII = res % 2; // odd resolutions are Class III
    // for Class III effective res of q2di is the Class I substrate 
    int effectiveRes = (isClassIII) ? res + 1 : res;

    // build the Z7 index from finest res up
    // adjust r for the fact that the res 0 base cell offsets the indexing
    // digits
    DgIVec3D::Direction digits[res + 1]; // +1 so index == resolution
    for (int r = 0; r < res+1; r++) digits[r] = 0;
    bool first = true;
    for (int r = effectiveRes - 1; r >= 0; r--) {
        DgIVec3D lastIJK = ijk;
        DgIVec3D lastCenter;
        if ((r + 1) % 2) { // finer res is Class III
            // rotate ccw
            ijk.upAp7();
            lastCenter = ijk;
            lastCenter.downAp7();
        } else {
            // rotate cw
            ijk.upAp7r();
            lastCenter = ijk;
            lastCenter.downAp7r();
        }

        // no digit generated for substrate
        if (first && isClassIII) { 
           first = false;
           continue;
        }
           
        DgIVec3D diff = lastIJK.diffVec(lastCenter);
        // don't need to normalize; done in unitIjkPlusToDigit
        //diff.ijkPlusNormalize();

        digits[r] = diff.unitIjkPlusToDigit()
        //H3_SET_INDEX_DIGIT(h, r + 1, _unitIjkToDigit(&diff));
    }

    string bcstr = dgg::util::to_string(baseCell, 2);
    string addstr = bcstr;
    for (int r = 1; r < res+1; r++) {
         addstr = addstr + string((int) digits[r]);
    }

/*
    // fijkBC should now hold the IJK of the base cell in the
    // coordinate system of the current face

    if (fijkBC.coord.i > MAX_FACE_COORD || fijkBC.coord.j > MAX_FACE_COORD ||
        fijkBC.coord.k > MAX_FACE_COORD) {
        // out of range input
        return H3_NULL;
    }

    // lookup the correct base cell
    int baseCell = _faceIjkToBaseCell(&fijkBC);
    H3_SET_BASE_CELL(h, baseCell);

    // rotate if necessary to get canonical base cell orientation
    // for this base cell
    int numRots = _faceIjkToBaseCellCCWrot60(&fijkBC);
    if (_isBaseCellPentagon(baseCell)) {
        // force rotation out of missing k-axes sub-sequence
        if (_h3LeadingNonZeroDigit(h) == K_AXES_DIGIT) {
            // check for a cw/ccw offset face; default is ccw
            if (_baseCellIsCwOffset(baseCell, fijkBC.face)) {
                h = _h3Rotate60cw(h);
            } else {
                h = _h3Rotate60ccw(h);
            }
        }

        for (int i = 0; i < numRots; i++) h = _h3RotatePent60ccw(h);
    } else {
        for (int i = 0; i < numRots; i++) {
            h = _h3Rotate60ccw(h);
        }
    }
 */

    DgZ7StringCoord z7str;
    z7str.setValString(addstr);
    dgcout << "addIn: " << addIn << " z7str: " << z7str << endl;

    return z7str;

} // DgZ7StringCoord DgQ2DItoZ7StringConverter::convertTypedAddress

////////////////////////////////////////////////////////////////////////////////
DgZ7StringToQ2DIConverter::DgZ7StringToQ2DIConverter
                (const DgRF<DgZ7StringCoord, long long int>& from,
                 const DgRF<DgQ2DICoord, long long int>& to)
        : DgConverter<DgZ7StringCoord, long long int, DgQ2DICoord, long long int> (from, to),
          pIDGG_ (NULL), effRes_ (0), effRadix_ (0)
{
   pIDGG_ = dynamic_cast<const DgIDGGBase*>(&toFrame());
   if (!pIDGG_) {
      report("DgZ7StringToQ2DIConverter::DgZ7StringToQ2DIConverter(): "
         " toFrame not of type DgIDGGBase", DgBase::Fatal);
   }

   const DgZ7StringRF* zRF = dynamic_cast<const DgZ7StringRF*>(&fromFrame());
   if (!zRF) {
      report("DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter(): "
         " fromFrame not of type DgZ7StringRF", DgBase::Fatal);
   }

   if (IDGG().dggs()->aperture() != zRF->aperture() || IDGG().res() != zRF->res()) {
      report("DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter(): "
         " fromFrame and toFrame apertures or resolutions do not match", DgBase::Fatal);
   }

   if (IDGG().gridTopo() != Hexagon || IDGG().dggs()->aperture() != 7) {
      report("DgZ7StringToQ2DIConverter::DgZ7StringToQ2DIConverter(): "
         "only implemented for aperture 3 hexagon grids", DgBase::Fatal);
   }

   effRes_ = IDGG().res();       // effective resolution
   effRadix_ = 7;
   // effRes_ is the number of Class I resolutions
   effRes_ = (IDGG().res() + 1) / 2;

} // DgQ2DItoZ7StringConverter::DgQ2DItoZ7StringConverter

////////////////////////////////////////////////////////////////////////////////
DgQ2DICoord
DgZ7StringToQ2DIConverter::convertTypedAddress (const DgZ7StringCoord& addIn) const
{
   printf("DgZ7StringToQ2DIConverter::convertTypedAddress\n");

   string addstr = addIn.valString();

   // first get the quad number
   string qstr = addstr.substr(0, 2);
   if (qstr[0] == '0') // leading 0
      qstr = qstr.substr(1, 1);
   int quadNum = std::stoi(qstr);

   // res 0 is just the quad number
   if (effRes_ == 0)
      return DgQ2DICoord(quadNum, DgIVec2D(0, 0));

   int index = 2; // skip the two quad digits

   // the rest is the Z7 digit string
   string z7str = addstr.substr(index);

//dgcout << "z7str in: " << z7str;

   // adjust if Class II (odd res)
   if (z7str.length() % 2)
      z7str += "0";
//dgcout << " adjusted: " << z7str << endl;

/*
   // build the digit string for i and j from the two-digit
   // z7 codes
   string radStr1 = "";
   string radStr2 = "";
   for (int i = 0; i < z7str.length(); i += 2) {
      string z7code = z7str.substr(i, 2);
      if (z7code == "00") {
         radStr1 += "0";
         radStr2 += "0";
      } else if (z7code == "22") {
         radStr1 += "0";
         radStr2 += "1";
      } else if (z7code == "21") {
         radStr1 += "0";
         radStr2 += "2";
      } else if (z7code == "01") {
         radStr1 += "1";
         radStr2 += "0";
      } else if (z7code == "02") {
         radStr1 += "1";
         radStr2 += "1";
      } else if (z7code == "20") {
         radStr1 += "1";
         radStr2 += "2";
      } else if (z7code == "12") {
         radStr1 += "2";
         radStr2 += "0";
      } else if (z7code == "10") {
         radStr1 += "2";
         radStr2 += "1";
      } else if (z7code == "11") {
         radStr1 += "2";
         radStr2 += "2";
      }
//      dgcout << "z7code: " << z7code << " radStr1: " << radStr1
//             << " radStr2: " << radStr2 << endl;
   }

   DgRadixString rad1(effRadix_, radStr1);
   DgRadixString rad2(effRadix_, radStr2);

//   dgcout << "qstr: " << qstr << " rad1: " << rad1 << " rad2: " << rad2 << endl;
*/

   //DgQ2DICoord q2di(quadNum, DgIVec2D(rad1.value(), rad2.value()));
   //dgcout << "q2di: " << q2di << endl;

   DgQ2DICoord q2di(0, DgIVec2D(0, 0));
   return q2di;

} // DgQ2DICoord DgZ7StringToQ2DIConverter::convertTypedAddress

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
