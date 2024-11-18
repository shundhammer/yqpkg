/*
  Copyright (C) 2019 SUSE LLC
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

  File:	      YQCustomStatusItemSelector.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

/-*/


#ifndef YQCustomStatusItemSelector_h
#define YQCustomStatusItemSelector_h

#include "YQItemSelector.h"


class YQCustomStatusSelectorItemWidget;


/**
 * ItemSelector widget with support for custom status values, not just 0 or 1.
 *
 * This does not use a standard QCheckBox or QRadioButton, but a QToolButton
 * with an icon that is exchanged whenever the status changes.
 **/
class YQCustomStatusItemSelector: public YQItemSelector
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQCustomStatusItemSelector( YWidget *                           parent,
                                const YItemCustomStatusVector &     customStates );

    /**
     * Destructor.
     **/
    virtual ~YQCustomStatusItemSelector();

    /**
     * Add an item.
     *
     * Reimplemented from YQItemSelector / YSelectionWidget.
     **/
    virtual void addItem( YItem * item );

    /**
     * Select or deselect an item.
     *
     * Reimplemented from YSelectionWidget.
     **/
    virtual void selectItem( YItem * item, bool selected = true );

    /**
     * Return the status icon for the specified status.
     **/
    const QIcon & statusIcon( int status ) const;

    /**
     * Activate selected item. Can be used in tests to simulate user input.
     *
     * Derived classes are required to implement this.
     **/
    virtual void activateItem( YItem * item );

protected slots:

    /**
     * Cycle the status of an item and report the click to the calling
     * application.
     **/
    void itemClicked( YQCustomStatusSelectorItemWidget * itemWidget );


protected:

    /**
     * Preload and cache the status icons.
     **/
    void preloadStatusIcons();

    /**
     * Update the status indicator (the status icon) for an item.
     *
     * Reimplemented from YItemSelector.
     **/
    virtual void updateCustomStatusIndicator( YItem * item );


    // Data members

    QList<QIcon>        _statusIcons;

};	// class YQCustomStatusItemSelector



/**
 * Class for the widgets of one YQCustomStatusItemSelector item.
 **/
class YQCustomStatusSelectorItemWidget: public YQSelectorItemWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     **/
    YQCustomStatusSelectorItemWidget( YQItemSelector    * parent,
                                      YItem		* item );

    /**
     * Destructor.
     **/
    virtual ~YQCustomStatusSelectorItemWidget();

    /**
     * Set the correct status icon according to the status of this item.
     **/
    void setStatusIcon();

signals:

    /**
     * Emitted when the user clicks on the heading toggle to change the status
     * or uses a keyboard operation to do the same.
     **/
    void clicked( YQCustomStatusSelectorItemWidget * itemWidget );

protected slots:

    /**
     * Forwarder slot from the heading toggle to this class.
     **/
    void slotClicked();

protected:

    /**
     * Create the appropriate toggle button for this item and connect it to
     * appropriate slots.
     *
     * Reimplemented from YQCustomStatusSelectorItemWidget.
     **/
    virtual QAbstractButton * createHeadingToggle( const std::string &  label,
                                                   QWidget *            parent );

    /**
     * Set the correct status icon according to the status of this item.
     **/
    void setStatusIcon( QAbstractButton * toggle );

    /**
     * Return the amount of indentation in pixels for the description text.
     *
     * Reimplemented from YQCustomStatusSelectorItemWidget.
     **/
    virtual int itemDescriptionIndent() const;

};	// class YQCustomStatusSelectorItemWidget


#endif	// YQCustomStatusItemSelector_h
