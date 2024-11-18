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

  File:		YWidget.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YWidget_h
#define YWidget_h

#include <string>
#include <iosfwd>

#include "YTypes.h"
#include "YProperty.h"
#include "YUISymbols.h"
#include "YUIException.h"
#include "YChildrenManager.h"
#include "ImplPtr.h"


class YDialog;
class YWidgetID;
class YMacroRecorder;


typedef YChildrenManager<YWidget>	YWidgetChildrenManager;
typedef YSingleChildManager<YWidget>	YSingleWidgetChildManager;
typedef YChildrenRejector<YWidget>	YWidgetChildrenRejector;

class YWidgetPrivate;


/**
 * Abstract base class of all UI widgets
 **/
class YWidget
{
protected:
    /**
     * Constructor.
     **/
    YWidget( YWidget * parent );

public:
    /**
     * Destructor.
     **/
    virtual ~YWidget();

    /**
     * Returns a descriptive name of this widget class for logging,
     * debugging etc.
     **/
    virtual const char * widgetClass() const { return "YWidget"; }

    /**
     * Returns a descriptive label of this widget instance.
     *
     * This default implementation returns this widget's "shortcut property"
     * (possibly trunctated to avoid over-long texts) - the property that
     * contains the keyboard shortcut used to activate this widget or to move
     * the keyboard focus to it. In most cases this is this widget's label.
     *
     * Note: This is usually translated to the user's target language.
     * This makes this useful for debugging only.
     **/
    virtual std::string debugLabel() const;

    /**
     * Return the help text for this widget.
     **/
    std::string helpText() const;

    /**
     * Set a help text for this widget.
     *
     * Currently, the UI does not do anything with this text but store it.
     * Displaying the text at a convenient time is currently the application's
     * responsibility. This may change in future versions.
     **/
    void setHelpText( const std::string & helpText );


    //
    // Property Management
    //

    /**
     * Return this class's property set.
     * This also initializes the property upon the first call.
     *
     * Derived classes should reimplement this.
     *
     * Remember to add the base class's property set to your own
     * in reimplemented versions, e.g.:
     *
     *	   const YPropertySet &
     *	   MyWidgetClass::propertySet()
     *	   {
     *	       static YPropertySet propSet;
     *
     *	       if ( propSet.isEmpty() )
     *	       {
     *             // Add properties for the derived class
     *		   propSet.add( YProperty( YUIProperty_Value, YStringProperty ) );
     *		   propSet.add( YProperty( YUIProperty_Label, YStringProperty ) );
     *
     *             // Add base class properties
     *		   propSet.add( YWidget::propertySet() );
     *	       }
     *
     *	       return propSet;
     *	   }
     *
     * Otherwise the base class's properties will not be available in the
     * derived class. It is also important that the base class's properties
     * are added after those of the derived class so the derived class's
     * properties have priority over those of the base class.
     **/
    virtual const YPropertySet & propertySet();

    /**
     * Set a property. Derived classes need to implement this.
     *
     * This method may throw exceptions, for example
     *	 - if there is no property with that name
     *	 - if the expected type and the type mismatch
     *	 - if the value is out of range
     *
     * This function returns 'true' if the value was successfully set and
     * 'false' if that value requires special handling (not in error cases:
     * those are covered by exceptions).
     **/
    virtual bool setProperty( const std::string    & propertyName,
			      const YPropertyValue & val );

    /**
     * Get a property. Derived classes need to implement this.
     *
     * This method may throw exceptions, for example
     *	 - if there is no property with that name
     **/
    virtual YPropertyValue getProperty( const std::string & propertyName );


    //
    // Children Management
    //
    // Even though many widget classes are leaf classes and thus cannot have
    // children by design, it makes sense to have the children management in
    // this base class: Then descending down a widget tree is transparent to
    // the outside without the need to check for container widget classes,
    // casting to those container widget classes and only calling child
    // management methods in that case.
    //
    // By default, YWidget and derived classes have a YWidgetChildrenRejector
    // as their children manager, i.e. any attempt to add a child will result
    // in a YUITooManyChildrenException.
    //
    // Derived classes that can (semantically) handle children should set the
    // children manager to one of
    //
    // - YWidgetChildrenManager: handles any number of child widgets;
    //   useful for VBox / HBox
    //
    // - YSingleWidgetChildManager: handles exactly one child
    //   useful for widgets like Alignment, Frame, Dialog
    //


    /**
     * Returns 'true' if this widget has any children.
     **/
    bool hasChildren() const
	{ return childrenManager()->hasChildren(); }

    /**
     * Returns the first child or 0 if there is none.
     * Useful mostly for children managers that handle only one child.
     **/
    YWidget * firstChild() const
	{ return childrenManager()->firstChild(); }

    /**
     * Returns the last child or 0 if there is none.
     **/
    YWidget * lastChild() const
	{ return childrenManager()->lastChild(); }

    /**
     * Return an iterator that points to the first child or to childrenEnd() if
     * there are no children.
     **/
    YWidgetListIterator childrenBegin() const
	{ return childrenManager()->begin(); }

    /**
     * Return an interator that points after the last child.
     **/
    YWidgetListIterator childrenEnd() const
	{ return childrenManager()->end(); }

    /**
     * Return a const iterator that points to the first child or to childrenEnd() if
     * there are no children.
     **/
    YWidgetListConstIterator childrenConstBegin() const
	{ return childrenManager()->begin(); }

    /**
     * Return a const interator that points after the last child.
     **/
    YWidgetListConstIterator childrenConstEnd() const
	{ return childrenManager()->end(); }

    /**
     * A helper for the range-based "for" loop
     * @return Iterator pointing to the beginning of the children list
     */
    YWidgetListIterator begin()
    { return childrenBegin(); }

    /**
     * A helper for the range-based "for" loop
     * @return Iterator pointing to the end of the children list
     */
    YWidgetListIterator end()
    { return childrenEnd(); }

    /**
     * Returns the current number of children.
     **/
    int childrenCount() const { return childrenManager()->count(); }

    /**
     * Checks if 'child' is a (direct!) child of this widget.
     **/
    bool contains( YWidget * child ) const
	{ return childrenManager()->contains( child ); }

    /**
     * Add a new child.
     *
     * This may throw exceptions if more children are added than this widget
     * can handle.
     **/
    virtual void addChild( YWidget * child );

    /**
     * Remove a child. This only removes the child from the children manager's
     * list; it does not delete it.
     **/
    virtual void removeChild( YWidget * child );

    /**
     * Delete all children and remove them from the children manager's list.
     **/
    void deleteChildren();

    /**
     * Return this widget's parent or 0 if it doesn't have a parent.
     **/
    YWidget * parent() const;

    /**
     * Return 'true' if this widget has a parent, 'false' if not.
     **/
    bool hasParent() const;

    /**
     * Set this widget's parent.
     **/
    void setParent( YWidget * newParent );

    /**
     * Traverse up the widget hierarchy and find the dialog this widget belongs
     * to. Returns 0 if there is none.
     **/
    YDialog * findDialog();

    /**
     * Recursively find a widget by its ID.
     * If there is no widget with that ID, this function throws a
     * YUIWidgetNotFoundException if 'doThrow' is 'true'. It returns 0 if
     * 'doThrow' is 'false'.
     **/
    YWidget * findWidget( YWidgetID * id, bool doThrow = true ) const;


    //
    // Geometry Management
    //

    /**
     * Preferred width of the widget.
     *
     * Derived classes are required to implement this.
     **/
    virtual int preferredWidth() = 0;

    /**
     * Preferred height of the widget.
     *
     * Derived classes are required to implement this.
     **/
    virtual int preferredHeight() = 0;

    /**
     * Preferred size of the widget in the specified dimension.
     * This default implementation calls preferredWidth() or preferredHeight()
     * which makes sense for most cases.
     *
     * Derived classes can reimplement this, but this is discouraged.
     *
     * Note: Even in that case, preferredWidth() and preferredHeight() need to
     * be implemented, but they might then call preferredSize().
     **/
    virtual int preferredSize( YUIDimension dim );

    /**
     * Set the new size of the widget.
     *
     * Layout manager widgets (like YLayoutBox) call this during geometry
     * management after all widgets are queried about their preferred widths
     * and heights. Depending on layout constraints, widgets might be resized
     * beyond or below their preferred size.
     *
     * The sizes passed here are not meant to affect any future
     * preferredWidth() or preferredHeight() calls; they are just the outcome
     * of all kinds of compromises (too little screen space or too much) for
     * the current geometry management calculation.
     *
     * Derived classes are required to implement this function.
     **/
    virtual void setSize( int newWidth, int newHeight ) = 0;


    //
    // Misc
    //


    /**
     * Checks whether or not this object is valid. This is to enable
     * dangling pointer error checking (i.e. this object is already
     * deallocated, but a pointer to it is still in use).
     *
     * See also the YUI_CHECK_WIDGET() macro in YUIException.h
     **/
    bool isValid() const;

    /**
     * Check if this widget is in the process of being destroyed.
     **/
    bool beingDestroyed() const;

    /**
     * Return a pointer to the underlying toolkit's (Qt, ...) widget
     * representing this abstract UI widget.
     **/
    void * widgetRep() const;

    /**
     * Set the pointer to the underlying toolkit's (Qt, ...) widget
     * representing this abstract UI widget.
     *
     * This pointer might be useful for derived UIs to store a counterpart of
     * the toolkit widget in each YWidget. The abstract UI does not need that,
     * though; this is purely for the convenience of derived UIs. All the
     * abstract UI ever does with that pointer is store it.
     **/
    void setWidgetRep( void * toolkitWidgetRep );

    /**
     * Returns 'true' if this widget has an ID.
     **/
    bool hasId() const;

    /**
     * Returns this widget's ID.
     **/
    YWidgetID * id() const;

    /**
     * Set this widget's ID.
     *
     * The widget assumes ownership of this ID and will delete it when needed.
     * (In the widget's destructor or when a new ID is set)
     *
     * Widget IDs are purely for application use. C++ applications don't need
     * to use them; they are much better off using widget pointers. For other
     * languages, though, that can't use C++ pointers (e.g. Ruby) it makes
     * sense to have widget IDs to identify widgets.
     **/
    void setId( YWidgetID * newId_disown );

    /**
     * Enable or disable this widget, i.e. make it accept or reject user input.
     *
     * Derived classes should call the base class method to update the internal
     *"enabled" flag.
     **/
    virtual void setEnabled( bool enabled = true );

    /**
     * Disable this widget (overloaded for better readability).
     **/
    void setDisabled() { setEnabled( false); }

    /**
     * Returns 'true' if this widget is enabled.
     **/
    virtual bool isEnabled() const;

    /**
     * This is a boolean value that determines whether the widget is resizable
     * beyond its preferred size in the specified dimension. A selection box is
     * stretchable in both dimensions, a push button is not stretchable by
     * default, a frame is stretchable if its contents are stretchable. Most
     * widgets accept a :hstretch or :vstretch option to become stretchable
     * even when by default they are not.
     **/
    virtual bool stretchable( YUIDimension dim ) const;

    /**
     * Set the stretchable state to "newStretch" regardless of any :hstretch or
     * :vstretch options.
     **/
    void setStretchable( YUIDimension dim, bool newStretch );

    /**
     * Set the stretchable state to "newStretch".
     * :hstretch or :vstretch options may override this.
     **/
    void setDefaultStretchable( YUIDimension dim, bool newStretch );

    /**
     * The weight is used in situations where all widgets can get their
     * preferred size and yet space is available. The remaining space will be
     * devided between all stretchable widgets according to their weights. A
     * widget with greater weight will get more space. The default weight for
     * all widgets is 0.
     *
     * Derived classes can overwrite this function, but they should call this
     * base class function in the new function.
     **/
    virtual int weight( YUIDimension dim );

    /**
     * Return whether or not the widget has a weight in the specified
     * dimension.
     **/
    bool hasWeight( YUIDimension dim );

    /**
     * Set a weight in the specified dimension.
     **/
    void setWeight( YUIDimension dim, int weight );

    /**
     * Sets the Notify property
     **/
    void setNotify( bool notify = true );

    /**
     * Returns whether the widget will notify, i.e. will case UserInput to
     * return.
     **/
    bool notify() const;

    /**
     * Sets the notifyContextMenu property
     **/
    void setNotifyContextMenu( bool notifyContextMenu = true );

    /**
     * Returns whether the widget will send an event when the user
     * clicks selects the context menu e.g. via right click.
     **/
    bool notifyContextMenu() const;


    /**
     * Returns 'true' if this widget should send key events, i.e. if it has
     * Opt(:keyEvent) set.
     **/
    bool sendKeyEvents() const;

    /**
     * Specify whether or not this widget should send key events.
     **/
    void setSendKeyEvents( bool doSend );

    /**
     * Returns 'true' if a keyboard shortcut should automatically be assigned
     * to this widget - without complaints in the log file.
     **/
    bool autoShortcut() const;

    /**
     * Sets the 'autoShortcut' flag.
     **/
    void setAutoShortcut( bool _newAutoShortcut );

    /**
     * Return a function key number that is assigned to this widget.
     * (1 for F1, 2 for F2, etc.; 0 for none)
     **/
    int functionKey() const;

    /**
     * Check if a function key is assigned to this widget.
     **/
    bool hasFunctionKey() const;

    /**
     * Assign a function key to this widget
     * (1 for F1, 2 for F2, etc.; 0 for none)
     *
     * Derived classes may want to overwrite this function, but they should
     * call this base class function in the new function.
     **/
    virtual void setFunctionKey( int fkey_no );

    /**
     * Set the keyboard focus to this widget.
     * The default implementation just emits a warning message.
     * Overwrite this function for all widgets that can accept the
     * keyboard focus.
     *
     * This function returns true if the widget did accept the
     * keyboard focus, and false if not.
     **/
    virtual bool setKeyboardFocus();

    /**
     * Get the string of this widget that holds the keyboard shortcut, if any.
     * Most widgets will return label().
     *
     * Overwrite this for widgets that can have keyboard shortcuts.
     **/
    virtual std::string shortcutString() const { return std::string( "" ); }

    /**
     * Set the string of this widget that holds the keyboard shortcut, if any.
     * Most widgets will call setLabel().
     *
     * Overwrite this for widgets that can have keyboard shortcuts.
     **/
    virtual void setShortcutString( const std::string & str );

    /**
     * The name of the widget property that will return user input, if there is
     * any.  Widgets that do have user input (such as InputField, ComboBox,
     * SelBox) should overwrite this methods. Widgets that are purely passive
     * (such as Label, RichText) should not.
     **/
    virtual const char * userInputProperty() { return (const char *) 0; }

    /**
     * Debugging function:
     * Dump the widget tree from here on to the log file.
     **/
    void dumpWidgetTree( int indentationLevel = 0 );

    /**
     * Debugging function:
     * Dump the widget tree from this widget's dialog parent.
     * If there is no such dialog parent, dump the widget tree from
     * here on.
     **/
    void dumpDialogWidgetTree();

    /**
     * Enable or disable all widgets in this widget tree.
     **/
    void setChildrenEnabled( bool enabled );

    //
    // Macro Recorder Support
    //

    /**
     * Recursively save the user input of all child widgets to a macro
     * recorder:
     *
     * All child widgets that could contain data entered by the user
     * are requested to send their contents to the macro recorder, e.g. input
     * fields, check boxes etc.
     *
     * This default implementation records this widget's user input property
     * (the property returned by userInputProperty) and then recursively calls
     * saveUserInput() for all child widgets. This is suitable for most cases,
     * for container widgets as well as for leaf widgets that have no or
     * exactly one property that needs to be recorded.
     *
     * Widgets that need another number of properties recorded should
     * reimplement this method (and NOT call this default method in the new
     * implementation).
     **/
    virtual void saveUserInput( YMacroRecorder *macroRecorder );

    /**
     * Overloaded operator new to ensure widgets are always created on the
     * heap, never on the stack.
     *
     * Simpler implementations of this have a tendency to be fooled by poorly
     * implemented derived classes.
     **/
    void * operator new( size_t size );

    /**
     * Operator delete counterpart to our custom operator new.
     **/
    void operator delete( void * ptr ) noexcept;


    // NCurses optimizations


    /**
     * In some UIs updating the screen content is an expensive operation. Use
     * startMultipleChanges() to tell the ui that you're going to perform
     * multiple chages to the widget.  The UI may delay any screen updates
     * until doneMultipleChanges() is called.
     **/
    virtual void startMultipleChanges() {}
    virtual void doneMultipleChanges() {}


protected:

    /**
     * Returns this widget's children manager.
     **/
    YWidgetChildrenManager * childrenManager() const;

    /**
     * Sets a new children manager for this widget. The widget assumes
     * ownership of this children manager and will delete it when appropriate.
     *
     * The default children manager (a YWidgetChildrenRejector) rejects all
     * children. This is useful for leaf widgets such as PushButton, ComboBox
     * etc.
     *
     * Derived classes that can handle children might want to set the children
     * manager to a YWidgetChildrenManager (the base class that does not reject
     * children) or to a YSingleWidgetChildManager (the class that handles
     * exactly one child widget).
     **/
    void setChildrenManager( YWidgetChildrenManager * manager );

    /**
     * Set the "being destroyed" flag, i.e. indicate that this widget is in the
     * process of being destroyed. The base class method already sets this, but
     * sometimes it might be useful to call this in a derived class's
     * destructor so certain optimizations work better.
     *
     * This status intentionally cannot be reverted to "not being destroyed".
     **/
     void setBeingDestroyed();

    /**
     * Helper function for dumpWidgetTree():
     * Dump one widget to the log file.
     **/
    void dumpWidget( YWidget *w, int indentationLevel );


private:

    /**
     * Make this widget invalid. This operation cannot be reversed.
     **/
    void invalidate();

    /**
     * Disable copy constructor.
     **/
    YWidget( const YWidget & other );

    /**
     * Disable assignment operator.
     **/
    const YWidget & operator=( const YWidget & other );

private:

    //
    // Data Members
    //

    int 			_magic; // should always be the first member
    ImplPtr<YWidgetPrivate>	priv;
    static YPropertySet		_propertySet;
    static bool			_usedOperatorNew;


#include "YWidget_OptimizeChanges.h"

};


std::ostream & operator<<( std::ostream & stream, const YWidget * widget );


#endif // YWidget_h
