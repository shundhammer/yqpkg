/*
  Copyright (C) 2014 SUSE LLC
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
/**
@page notes1 Notes about Initialization

@section Important entry points

- YUI::widgetFactory()
- YUI::ui()

@section What happens on initialization

YUI::ui()
calls YUI::ensureUICreated()
calls YUILoader::loadUI
calls YUILoader::loadPlugin.
That instantiates YUIPlugin(string plugin_name) and
calls its YUIPlugin::locateSymbol "_Z8createUIb"
  (mangled createUI(bool) )
to produce a YUI * (* createUIFunction_t)(bool withThreads)
which is called. Its result is discarded, but YUI::_ui gets initialized
by the YUI::YUI ctor called by the derived ctor.

YUIPlugin calls dlopen, but dlclose is not called by default,
and actually proably never.

@section Utility classes

- YCommandLine

 */
