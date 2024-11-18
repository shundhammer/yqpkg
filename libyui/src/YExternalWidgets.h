/*
  Copyright (C) 2013 Angelo Naselli <anaselli at linux dot it>

  This file is part of libyui project

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

#ifndef YExternalWidgets_h
#define YExternalWidgets_h
#include <map>
#include <string>

class YExternalWidgetFactory;

/**
 * Abstract base class of a libYUI Widget Extension interface.
 **/
class YExternalWidgets
{
    friend class YExternalWidgetsTerminator;

protected:
    /**
     * Constructor.
     * 'name' is the plugin name
     *
     * throws a YUIException if the plugin 'name' has been alread created
     **/
    YExternalWidgets( const std::string& name );

public:

    /**
     * Destructor.
     **/
    virtual ~YExternalWidgets();


    /**
     * Access the global YUI external widgets.
     * 'name' is the plugin name
     *
     * if plugin 'name' has not been explicitally loaded by YUILoader::loadExternalWidgets
     * externalWidgets try loading it (exactly as YUI::ui does) with default function symbol
     * to be executed (see YUILoader::loadExternalWidgets for explanation)
     **/
    static YExternalWidgets * externalWidgets( const std::string & name );

    /**
     * Return the external widget factory that provides all the createXY() methods for
     * user defined widgets.
     *
     * This will create the factory upon the first call and return a pointer to
     * the one and only (singleton) factory upon each subsequent call.
     * This may throw exceptions if the factory cannot be created.
     *
     * It is up to user extend YExternalWidgetFactory to add createXY() methods in
     * his/her implementation. So once YExternalWidgetFactory is extended with
     * all the createXY() methods, three sub-plugins must be defined one for each
     * supported graphical environment, e.g. Gtk, ncurses and QT, following the
     * libyui implementation rules.
     *
     * For instance an external widgets plugin called yui-foo that needs Gtk, ncurses
     * and QT specialization will require also yui-foo-gtk, yui-foo-ncurses and
     * yui-foo-qt plugin implementation.
     *
     **/
    YExternalWidgetFactory        * externalWidgetFactory();
    static YExternalWidgetFactory * externalWidgetFactory( const std::string & name );

protected:

    /**
     * Create the external widgets factory that provides all the createXY() methods for
     *
     * Derived classes are required to implement this. Usually createXY() is virtual,
     * real implementation is demanded to derived classes that implement Gtk, ncurses and QT
     * specialization.
     **/
    virtual YExternalWidgetFactory * createExternalWidgetFactory() = 0;

private:

    /** External widgets plugin name */
    std::string _name;

    /** External widget factory */
    YExternalWidgetFactory* _factory;

    /** plugin instances */
    static  std::map<std::string, YExternalWidgets *> _externalWidgets;
};

#endif // YExternalWidgets_h
