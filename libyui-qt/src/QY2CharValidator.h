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

  File:	      QY2CharValidator.h

  Author:     Stefan Hundhammer <shundhammer@suse.de>

  This is a pure Qt object - it can be used independently of YaST2.

/-*/


#ifndef QY2CharValidator_h
#define QY2CharValidator_h

#include <qvalidator.h>


class QY2CharValidator : public QValidator
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * Create a validator object that checks for input characters being any one
     * of 'validChars'. If this is empty, any input is accepted.
     **/
    QY2CharValidator( const QString &	initialValidChars,
		      QObject *		parent	= 0,
		      const char *	name	= 0 );

    /**
     * Destructor.
     **/
    ~QY2CharValidator();

    /**
     * Check user input.
     *
     * Reimplemented from @ref QValidator.
     **/
    virtual State validate( QString & input, int & pos ) const;

    /**
     * Set the valid input characters.
     * If 'newValidChars' is empty, any input is accepted.
     **/
    void setValidChars( const QString &	newValidChars )
	{ _validChars = newValidChars; }

    /**
     * Retrieve the valid input characters.
     **/
    QString validChars() const { return _validChars; }

protected:

    QString _validChars;
};

#endif // QY2CharValidator_h
