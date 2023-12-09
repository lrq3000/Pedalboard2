//	c0xHeaders.h - Some c++0x stuff.
//  ----------------------------------------------------------------------------
//	This file is part of 'NiallsAVLib', base code for any kind of audiovisual
//	apps.
//	Copyright (C) 2012  Niall Moody
//	
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.
//	----------------------------------------------------------------------------

#ifndef C0XHEADERS_H_
#define C0XHEADERS_H_

///	Here we have code which brings in the correct versions of shared_ptr etc.
/*!
	This is necessary because XCode (whichever version it is I'm using, anyway)
	doesn't have c++11 support, meaning we need to use boost.
 */

#include <memory>

#ifdef __APPLE__
#include <boost/pointer_cast.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#else
#include <array>
#endif

//------------------------------------------------------------------------------
#ifdef __APPLE__
using boost::array;
using boost::shared_ptr;
using boost::weak_ptr;
using boost::dynamic_pointer_cast;
#else
using std::array;
using std::shared_ptr;
using std::weak_ptr;
using std::dynamic_pointer_cast;
#endif

#endif
