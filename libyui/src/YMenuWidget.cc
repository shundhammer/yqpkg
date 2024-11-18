/*
  Copyright (c) [2020] SUSE LLC
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

  File:		YMenuWidget.cc

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/


#define YUILogComponent "ui"
#include "YUILog.h"

#include "YUISymbols.h"
#include "YShortcut.h"
#include "YMenuWidget.h"

#define VERBOSE_SHORTCUTS       0


using std::string;


struct YMenuWidgetPrivate
{
    YMenuWidgetPrivate()
	: nextSerialNo( 0 )
	{}

    int nextSerialNo;
};




YMenuWidget::YMenuWidget( YWidget * parent, const string & label )
    : YSelectionWidget( parent,
                        label,
			false )	// enforceSingleSelection
    , priv( new YMenuWidgetPrivate() )
{
    YUI_CHECK_NEW( priv );
}


YMenuWidget::~YMenuWidget()
{
    // NOP
}


void
YMenuWidget::addItems( const YItemCollection & itemCollection )
{
    YSelectionWidget::addItems( itemCollection );
    resolveShortcutConflicts();
    rebuildMenuTree();
}


void
YMenuWidget::addItem( YItem * item )
{
    YSelectionWidget::addItem( item );
    item->setIndex( ++(priv->nextSerialNo) );

    if ( item->hasChildren() )
	assignUniqueIndex( item->childrenBegin(), item->childrenEnd() );
}


void
YMenuWidget::assignUniqueIndex( YItemIterator begin, YItemIterator end )
{
    for ( YItemIterator it = begin; it != end; ++it )
    {
	YItem * item = *it;

	item->setIndex( ++(priv->nextSerialNo) );

	if ( item->hasChildren() )
	    assignUniqueIndex( item->childrenBegin(), item->childrenEnd() );
    }
}


void
YMenuWidget::deleteAllItems()
{
    YSelectionWidget::deleteAllItems();
    priv->nextSerialNo = 0;
}


void
YMenuWidget::setItemEnabled( YMenuItem * item, bool enabled )
{
    if ( item )
        item->setEnabled( enabled );
}


void
YMenuWidget::setItemVisible( YMenuItem * item, bool visible )
{
    if ( item )
        item->setVisible( visible );
}


YMenuItem *
YMenuWidget::findMenuItem( int index )
{
    return findMenuItem( index, itemsBegin(), itemsEnd() );
}


YMenuItem *
YMenuWidget::findMenuItem( int wantedIndex,
                        YItemConstIterator begin,
                        YItemConstIterator end )
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *> (*it);

	if ( item )
	{
	    if ( item->index() == wantedIndex )
		return item;

	    if ( item->hasChildren() )
	    {
		YMenuItem * result = findMenuItem( wantedIndex,
                                                   item->childrenBegin(),
                                                   item->childrenEnd() );
		if ( result )
		    return result;
	    }
	}
    }

    return 0;
}


// FIXME: This is ugly code; candidate for refactoring.
void
YMenuWidget::resolveShortcutConflicts( YItemConstIterator begin,
                                       YItemConstIterator end )
{
#define USED_SIZE ((int) sizeof( char ) << 8)

    bool used[ USED_SIZE ];

    for ( int i = 0; i < USED_SIZE; i++ )
	used[i] = false;

    std::vector<YMenuItem*> conflicts;

    for ( YItemConstIterator it = begin; it != end; ++it )
    {
	YMenuItem * item = dynamic_cast<YMenuItem *> (*it);

        if ( item->isSeparator() )
            continue;

	if ( item )
	{
	    if ( item->hasChildren() )
	    {
		resolveShortcutConflicts( item->childrenBegin(), item->childrenEnd() );
	    }

            char shortcut = YShortcut::normalized( YShortcut::findShortcut( item->label() ) ) ;

            if ( shortcut == 0 )
            {
                conflicts.push_back(item);

#if VERBOSE_SHORTCUTS
                yuiMilestone() << "No or invalid shortcut found: \""
                               << item->label() << "\""
                               << endl;
#endif
            }
            else if ( used[ (unsigned) shortcut ] )
            {
                conflicts.push_back(item);
#if VERBOSE_SHORTCUTS
                yuiWarning() << "Conflicting shortcut found: \""
                             << item->label() << "\""
                             << endl;
#endif
            }
            else
            {
                used[ (unsigned) shortcut ] = true;
            }
	}
        else
        {
            yuiWarning() << "Non-menu item used in call: \""
                         << (*it)->label() << "\""
                         << endl;
        }
    }

    // Cannot use YShortcut directly as an YItem is not a YWidget

    for ( YMenuItem *i: conflicts )
    {
        string clean = YShortcut::cleanShortcutString(i->label());
        char new_c = 0;

        size_t index = 0;
        for (; index < clean.size(); ++index)
        {
            char ch = YShortcut::normalized( clean[ index ] );

            // ch is set to 0 by normalized() if not valid
            if ( ch != 0 && ! used[ (unsigned) ch ] )
            {
                new_c = ch;
                used[ (unsigned) ch ] = true;
                break;
            }
        }

        if ( new_c != 0 )
        {
            clean.insert( index, 1, YShortcut::shortcutMarker() );

#if VERBOSE_SHORTCUTS
            yuiDebug() << "New label used: " << clean << endl;
#endif
        }

        i->setLabel( clean );
    }
}

// FIXME End


void
YMenuWidget::resolveShortcutConflicts()
{
    resolveShortcutConflicts( itemsBegin(), itemsEnd() );
}


YMenuItem *
YMenuWidget::findItem( std::vector<std::string> & path ) const
{
    return findItem( path.begin(), path.end(),
                     itemsBegin(), itemsEnd() );
}


YMenuItem *
YMenuWidget::findItem( std::vector<std::string>::iterator path_begin,
                       std::vector<std::string>::iterator path_end,
                       YItemConstIterator                 begin,
                       YItemConstIterator                 end ) const
{
    for ( YItemConstIterator it = begin; it != end; ++it )
    {
        YMenuItem * item = dynamic_cast<YMenuItem *>(*it);
        // Test that dynamic_cast didn't fail

        if ( !item )
            return 0;

        if ( item->label() == *path_begin )
        {
            if ( std::next( path_begin ) == path_end )
            {
                // Only return items which can trigger an action.
                // Intermediate items only open a submenu, so continue looking.
                if ( item->hasChildren() )
                    continue;

                return item;
            }

            // Look in child nodes
            YMenuItem * result = findItem( ++path_begin, path_end,
                                           item->childrenBegin(), item->childrenEnd() );
            if ( result )
                return result;
        }
    }
    return 0;
}

