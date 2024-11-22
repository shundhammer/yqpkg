/*  ------------------------------------------------------
              __   _____  ____  _
              \ \ / / _ \|  _ \| | ____ _
               \ V / | | | |_) | |/ / _` |
                | || |_| |  __/|   < (_| |
                |_| \__\_\_|   |_|\_\__, |
                                    |___/
    ------------------------------------------------------

    Project:  YQPkg Package Selector
    Copyright (c) 2024 SUSE LLC
    License:  GPL V2 - See file LICENSE for details.

    Textdomain "qt-pkg"
 */


#ifndef YQPkgChangesDialog_h
#define YQPkgChangesDialog_h

#include <QDialog>
#include <QComboBox>
#include <QRegExp>
#include <QFlags>

#include "YQZypp.h"


class YQPkgList;

/**
 * Changes dialog: Show a dialog with a list of packages that are changed.
 * By default, only packages with an "auto" status ( not set via selections )
 * are displayed.
 **/
class YQPkgChangesDialog : public QDialog
{
    Q_OBJECT

public:

  /**
   * filter combobox entries
   */
  enum FilterIndex
  {
    FilterIndexAll = 0,
    FilterIndexUser = 1,
    FilterIndexAutomatic = 2
  };

    /**
     * Filters
     */
    enum Filter
    {
      FilterNone = 0x0,
      FilterUser = 0x1,
      FilterAutomatic = 0x2,
      FilterAll = 0x1 | 0x2
    };
    Q_DECLARE_FLAGS(Filters, Filter);

    /**
     * Options
     */
    enum Option
    {
      OptionNone = 0x0,
      OptionAutoAcceptIfEmpty = 0x1
    };
    Q_DECLARE_FLAGS(Options, Option);

    /**
     * Set the current filter
     * This will change the combo box current selected
     * filter and update the list
     */
    void setFilter( Filters f );

    /**
     * Set the current filter
     * This will change the combo box current selected
     * filter and update the list
     */
    void setFilter( const QRegExp &regexp, Filters f );

    /**
     * Static convenience method: Post a changes dialog with text
     * 'message', a list of changed packages and one ( default ) or two buttons.
     *
     * Returns 'true' if the user accepted ( i.e. clicked the 'accept' button )
     * and 'false' if the user rejected ( i.e. clicked the 'reject' button or
     * the window manager close button ).
     *
     * If the list is empty ( i.e., there are no packages with an "auto"
     * status ), the dialog is not shown at all ( and returns 'true' ) - unless
     *'showIfListEmpty' is 'true'.
     **/
    static bool showChangesDialog( QWidget *		parent,
				   const QString & 	message,
				   const QString &	acceptButtonLabel,
				   const QString &	rejectButtonLabel = QString(),
                                   Filters f = FilterAutomatic,
                                   Options o = OptionAutoAcceptIfEmpty );

    /**
     * Static convenience method: Post a changes dialog with text 'message', a
     * list of changed packages whose names match the specified regular
     * expression 'regexp' and one ( default ) or two buttons.
     *
     * Returns 'true' if the user accepted ( i.e. clicked the 'accept' button )
     * and 'false' if the user rejected ( i.e. clicked the 'reject' button or
     * the window manager close button ).
     *
     * If the list is empty ( i.e., there are no packages with an "auto"
     * status ), the dialog is not shown at all ( and returns 'true' ) - unless
     *'showIfListEmpty' is 'true'.
     **/
    static bool showChangesDialog( QWidget *		parent,
				   const QString & 	message,
				   const QRegExp & 	regexp,
				   const QString &	acceptButtonLabel,
				   const QString &	rejectButtonLabel = QString(),
                                   Filters f = FilterAutomatic,
                                   Options o = OptionAutoAcceptIfEmpty );

    /**
     * Returns the preferred size.
     *
     * Reimplemented from QWidget to limit the dialog to the screen dimensions.
     **/
    virtual QSize sizeHint () const;

protected slots:
    /**
     * called when the filter is changed
     */
    void slotFilterChanged( int index );

protected:
    /**
     * Constructor: Creates a changes dialog with text 'message' on
     * top, a list packages with an "auto" status that is not set via selections
     * and one ( default ) or two buttons.
     *
     * Not meant for public use. Applications should use the static
     * 'showChangesDialog' method instead.
     *
     * This constructor does not call filter() yet - this is the caller's
     * responsibility.
     **/
    YQPkgChangesDialog( QWidget *		parent,
			const QString & 	message,
			const QString &		acceptButtonLabel,
			const QString &		rejectButtonLabel = QString() );

    /**
     * Apply the filter criteria: Fill the pkg list with pkgs that have a
     * "modify" status ( install, update, delete ) set by automatic ( i.e. via the
     * dependency solver ), by application ( i.e. via software selections ) or
     * manually by the user.
     **/
    void filter( Filters f = FilterAutomatic );

    /**
     * Apply the filter criteria: Fill the pkg list with pkgs that have a
     * "modify" status ( install, update, delete ) set by automatic ( i.e. via the
     * dependency solver ), by application ( i.e. via software selections ) or
     * manually by the user and whose name matches 'regexp'.
     **/
    void filter( const QRegExp & regexp, Filters f = FilterAutomatic );

    /**
     * extra filter for child classes
     */
    virtual bool extraFilter( ZyppSel sel, ZyppPkg pkg );

    /**
     * Returns 'true' if the pkg list is empty.
     * This is only meaningful after calling 'filter()' !
     **/
    bool isEmpty() const;


    // Data members
    QComboBox *_filter;
    YQPkgList *		_pkgList;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(YQPkgChangesDialog::Filters);
Q_DECLARE_METATYPE(YQPkgChangesDialog::Filters);


class YQPkgUnsupportedPackagesDialog : public YQPkgChangesDialog
{
public:
    /**
     * Constructor: Creates a changes dialog with text 'message' on
     * top, a list packages with an "auto" status that is not set via selections
     * and one ( default ) or two buttons.
     *
     * Not meant for public use. Applications should use the static
     * 'showChangesDialog' method instead.
     *
     * This constructor does not call filter() yet - this is the caller's
     * responsibility.
     **/
    YQPkgUnsupportedPackagesDialog( QWidget * parent,
                                    const QString &message,
                                    const QString &acceptButtonLabel,
                                    const QString &rejectButtonLabel = QString() );

  /**
   * Static convenience method: Post a changes dialog with text
   * 'message', a list of changed packages and one ( default ) or two buttons.
   *
   * Returns 'true' if the user accepted ( i.e. clicked the 'accept' button )
   * and 'false' if the user rejected ( i.e. clicked the 'reject' button or
   * the window manager close button ).
   *
   * If the list is empty ( i.e., there are no packages with an "auto"
   * status ), the dialog is not shown at all ( and returns 'true' ) - unless
   *'showIfListEmpty' is 'true'.
   **/
  static bool showUnsupportedPackagesDialog( QWidget *		parent,
                                             const QString & 	message,
                                             const QString &	acceptButtonLabel,
                                             const QString &	rejectButtonLabel = QString(),
                                             Filters f = FilterAutomatic,
                                             Options o = OptionAutoAcceptIfEmpty );
protected:
  /**
   * leave supported packages out.
   */
  virtual bool extraFilter( ZyppSel sel, ZyppPkg pkg );

};


#endif // ifndef YQPkgChangesDialog_h
