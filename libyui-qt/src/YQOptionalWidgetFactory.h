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

  File:		YQOptionalWidgetFactory.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YQOptionalWidgetFactory_h
#define YQOptionalWidgetFactory_h


#include <yui/YOptionalWidgetFactory.h>

#include "YQBarGraph.h"
#include "YQDateField.h"
#include "YQDownloadProgress.h"
#include "YQDumbTab.h"
#include "YQMultiProgressMeter.h"
#include "YQPartitionSplitter.h"
#include "YQSlider.h"
#include "YQTimeField.h"

class YQWizard;

using std::string;
using std::vector;



/**
 * Widget factory for optional ("special") widgets.
 *
 * Remember to always check with the corresponding "has..()" method if the
 * current UI actually provides the requested widget. Otherwise the
 * "create...()" method will throw an exception.
 **/
class YQOptionalWidgetFactory: public YOptionalWidgetFactory
{
public:

    //
    // Optional Widgets
    //


    virtual bool			hasWizard();
    virtual YWizard *			createWizard		( YWidget *   		parent,
								  const std::string & 	backButtonLabel,
								  const std::string & 	abortButtonLabel,
								  const std::string & 	nextButtonLabel,
								  YWizardMode 	wizardMode = YWizardMode_Standard );

    virtual bool			hasDumbTab();
    virtual YQDumbTab *			createDumbTab		( YWidget * parent );

    virtual bool			hasSlider();
    virtual YQSlider *			createSlider		( YWidget *		parent,
								  const std::string	&	label,
								  int 			minVal,
								  int 			maxVal,
								  int 			initialVal );

    virtual bool			hasDateField();
    virtual YQDateField *		createDateField		( YWidget * parent, const std::string & label );

    virtual bool			hasTimeField();
    virtual YQTimeField *		createTimeField		( YWidget * parent, const std::string & label );

    virtual bool			hasBarGraph();
    virtual YQBarGraph *		createBarGraph		( YWidget * parent );

    virtual bool			hasPatternSelector();
    virtual YWidget *		        createPatternSelector	( YWidget * parent, long modeFlags = 0 );

    virtual bool			hasSimplePatchSelector();
    virtual YWidget *	                createSimplePatchSelector( YWidget * parent, long modeFlags = 0 );

    virtual bool			hasMultiProgressMeter();
    virtual YQMultiProgressMeter *	createMultiProgressMeter( YWidget * parent, YUIDimension dim, const vector<float> & maxValues );

    virtual bool			hasPartitionSplitter();
    virtual YQPartitionSplitter *	createPartitionSplitter	( YWidget * 		parent,
								  int 			usedSize,
								  int 			totalFreeSize,
								  int 			newPartSize,
								  int 			minNewPartSize,
								  int 			minFreeSize,
								  const std::string &	usedLabel,
								  const std::string &	freeLabel,
								  const std::string &	newPartLabel,
								  const std::string &	freeFieldLabel,
								  const std::string &	newPartFieldLabel );

    virtual bool			hasDownloadProgress();
    virtual YQDownloadProgress *	createDownloadProgress	( YWidget * 		parent,
								  const std::string & 	label,
								  const std::string & 	filename,
								  YFileSize_t		expectedFileSize );

    virtual bool                        hasTimezoneSelector();
    virtual YTimezoneSelector *         createTimezoneSelector( YWidget *               parent,
								const std::string &     timezoneMap,
								const map<string,string> & timezones );

    virtual bool			hasGraph();
    virtual YGraph *			createGraph( YWidget * parent,
                                                     const std::string & filename,
						     const std::string & layoutAlgorithm );

    virtual YGraph *			createGraph( YWidget * parent,
                                                     /* graph_t */ void * graph );

    virtual bool			hasContextMenu();


protected:

    friend class YQUI;

    /**
     * Constructor.
     *
     * Use YUI::optionalWidgetFactory() to get the singleton for this class.
     **/
    YQOptionalWidgetFactory();

    /**
     * Destructor.
     **/
    virtual ~YQOptionalWidgetFactory();

}; // class YQOptionalWidgetFactory



#endif // YQOptionalWidgetFactory_h
