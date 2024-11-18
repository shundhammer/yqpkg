/*
  Copyright (C) 2000-2012 Novell, Inc
  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; either version 2.1 of the
  License, or (at your option) version 3.0 of the License. This library
  is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
  License for more details. You should have received a copy of the GNU
  Lesser General Public License along with this library; if not, write
  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
  Floor, Boston, MA 02110-1301 USA
*/


/*-/

  File:	      YRpmGroupsTree.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YRpmGroupsTree_h
#define YRpmGroupsTree_h


#include "YStringTree.h"


// This has been declared obsolete quite some time ago (2/2020), but it is
// still used in libyui-gtk-pkg.
//
// Once it is no longer used there, remove this class and also YStringTree and
// YTransText.
//
// -- shundhammer 2020-02-03


/**
 * This was an efficient storage for RPM group tags.
 *
 * THIS CLASS IS OBSOLETE. DO NOT USE THIS ANYMORE.
 **/
#ifndef SUPPRESS_YRPMGROUPSTREE_OBSOLETE_WARNING
#  warning The YRpmGroupsTree class is obsolete. DO NOT USE ANYMORE.
#endif
class YRpmGroupsTree: public YStringTree
{
public:

    YRpmGroupsTree();
    virtual ~YRpmGroupsTree();
    YStringTreeItem * addRpmGroup( const std::string & rpmGroup )
	{ return 0; }

    std::string rpmGroup( const YStringTreeItem * node )
	{ return origPath( node, '/', false ); }

    std::string translatedRpmGroup( const YStringTreeItem * node )
	{ return translatedPath( node, '/', false ); }

    void addFallbackRpmGroups();
};



#endif // YRpmGroupsTree_h
