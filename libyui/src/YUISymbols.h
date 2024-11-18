/*
  Copyright (C) 2000-2012 Novell, Inc
  Copyright (C) 2019-2020 SUSE LLC
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

  File:		YUISymbols.h

  Author:	Stefan Hundhammer <shundhammer@suse.de>

/-*/

#ifndef YUISymbols_h
#define YUISymbols_h


// UI builtin functions

#define YUIBuiltin_AskForExistingDirectory	"AskForExistingDirectory"
#define YUIBuiltin_AskForExistingFile		"AskForExistingFile"
#define YUIBuiltin_AskForSaveFileName		"AskForSaveFileName"
#define YUIBuiltin_AskForWidgetStyle		"AskForWidgetStyle"
#define YUIBuiltin_Beep				"Beep"
#define YUIBuiltin_BusyCursor			"BusyCursor"
#define YUIBuiltin_OpenContextMenu		"OpenContextMenu"
#define YUIBuiltin_ChangeWidget			"ChangeWidget"
#define YUIBuiltin_CloseDialog			"CloseDialog"
#define YUIBuiltin_CloseUI			"CloseUI"
#define YUIBuiltin_DumpWidgetTree		"DumpWidgetTree"
#define YUIBuiltin_GetDisplayInfo		"GetDisplayInfo"
#define YUIBuiltin_GetLanguage			"GetLanguage"
#define YUIBuiltin_GetProductName		"GetProductName"
#define YUIBuiltin_Glyph			"Glyph"
#define YUIBuiltin_HasSpecialWidget		"HasSpecialWidget"
#define YUIBuiltin_MakeScreenShot		"MakeScreenShot"
#define YUIBuiltin_NormalCursor			"NormalCursor"
#define YUIBuiltin_OpenDialog			"OpenDialog"
#define YUIBuiltin_OpenUI			"OpenUI"
#define YUIBuiltin_PollInput			"PollInput"
#define YUIBuiltin_QueryWidget			"QueryWidget"
#define YUIBuiltin_RecalcLayout			"RecalcLayout"
#define YUIBuiltin_Recode			"Recode"
#define YUIBuiltin_RedrawScreen			"RedrawScreen"
#define YUIBuiltin_ReplaceWidget		"ReplaceWidget"
#define YUIBuiltin_RunPkgSelection		"RunPkgSelection"
#define YUIBuiltin_SetConsoleFont		"SetConsoleFont"
#define YUIBuiltin_SetFocus			"SetFocus"
#define YUIBuiltin_SetFunctionKeys		"SetFunctionKeys"
#define YUIBuiltin_SetKeyboard			"SetKeyboard"
#define YUIBuiltin_RunInTerminal		"RunInTerminal"
#define YUIBuiltin_SetLanguage			"SetLanguage"
#define YUIBuiltin_SetProductName		"SetProductName"
#define YUIBuiltin_TimeoutUserInput		"TimeoutUserInput"
#define YUIBuiltin_UserInput			"UserInput"
#define YUIBuiltin_WaitForEvent			"WaitForEvent"
#define YUIBuiltin_WidgetExists			"WidgetExists"
#define YUIBuiltin_WizardCommand		"WizardCommand"

#define YUIBuiltin_PostponeShortcutCheck	"PostponeShortcutCheck"
#define YUIBuiltin_CheckShortcuts		"CheckShortcuts"

#define YUIBuiltin_RecordMacro			"RecordMacro"
#define YUIBuiltin_StopRecordMacro		"StopRecordMacro"
#define YUIBuiltin_PlayMacro			"PlayMacro"
#define YUIBuiltin_FakeUserInput		"FakeUserInput"
#define YUIBuiltin_WFM				"WFM"
#define YUIBuiltin_SCR				"SCR"



// Mandatory widgets

#define YUIWidget_Bottom			"Bottom"
#define YUIWidget_BusyIndicator			"BusyIndicator"
#define YUIWidget_ButtonBox			"ButtonBox"
#define YUIWidget_CheckBox			"CheckBox"
#define YUIWidget_CheckBoxFrame			"CheckBoxFrame"
#define YUIWidget_ComboBox			"ComboBox"
#define YUIWidget_CustomStatusItemSelector      "CustomStatusItemSelector"
#define YUIWidget_Empty				"Empty"
#define YUIWidget_Frame				"Frame"
#define YUIWidget_HBox				"HBox"
#define YUIWidget_HCenter			"HCenter"
#define YUIWidget_HSpacing			"HSpacing"
#define YUIWidget_HSquash			"HSquash"
#define YUIWidget_HStretch			"HStretch"
#define YUIWidget_HVCenter			"HVCenter"
#define YUIWidget_HVSquash			"HVSquash"
#define YUIWidget_HWeight			"HWeight"
#define YUIWidget_Heading			"Heading"
#define YUIWidget_IconButton			"IconButton"
#define YUIWidget_Image				"Image"
#define YUIWidget_InputField			"InputField"
#define YUIWidget_IntField			"IntField"
#define YUIWidget_Label				"Label"
#define YUIWidget_Left				"Left"
#define YUIWidget_LogView			"LogView"
#define YUIWidget_MarginBox			"MarginBox"
#define YUIWidget_MenuBar                       "MenuBar"
#define YUIWidget_MenuButton			"MenuButton"
#define YUIWidget_MinHeight			"MinHeight"
#define YUIWidget_MinSize			"MinSize"
#define YUIWidget_MinWidth			"MinWidth"
#define YUIWidget_MultiItemSelector             "MultiItemSelector"
#define YUIWidget_MultiLineEdit			"MultiLineEdit"
#define YUIWidget_MultiSelectionBox		"MultiSelectionBox"
#define YUIWidget_PackageSelector		"PackageSelector"
#define YUIWidget_Password			"Password"
#define YUIWidget_PkgSpecial			"PkgSpecial"
#define YUIWidget_ProgressBar			"ProgressBar"
#define YUIWidget_PushButton			"PushButton"
#define YUIWidget_RadioButton			"RadioButton"
#define YUIWidget_RadioButtonGroup		"RadioButtonGroup"
#define YUIWidget_ReplacePoint			"ReplacePoint"
#define YUIWidget_RichText			"RichText"
#define YUIWidget_Right				"Right"
#define YUIWidget_SelectionBox			"SelectionBox"
#define YUIWidget_SingleItemSelector            "SingleItemSelector"
#define YUIWidget_Table				"Table"
#define YUIWidget_TextEntry			"TextEntry"	// for backwards compatibility
#define YUIWidget_Top				"Top"
#define YUIWidget_Tree				"Tree"
#define YUIWidget_VBox				"VBox"
#define YUIWidget_VCenter			"VCenter"
#define YUIWidget_VSpacing			"VSpacing"
#define YUIWidget_VSquash			"VSquash"
#define YUIWidget_VStretch			"VStretch"
#define YUIWidget_VWeight			"VWeight"


// Special (optional) widgets

#define YUISpecialWidget_BarGraph		"BarGraph"
#define YUISpecialWidget_Date			"Date"
#define YUISpecialWidget_DateField		"DateField"
#define YUISpecialWidget_DownloadProgress	"DownloadProgress"
#define YUISpecialWidget_DumbTab		"DumbTab"
#define YUISpecialWidget_DummySpecialWidget	"DummySpecialWidget"
#define YUISpecialWidget_HMultiProgressMeter	"HMultiProgressMeter"
#define YUISpecialWidget_VMultiProgressMeter	"VMultiProgressMeter"
#define YUISpecialWidget_PartitionSplitter	"PartitionSplitter"
#define YUISpecialWidget_PatternSelector	"PatternSelector"
#define YUISpecialWidget_SimplePatchSelector	"SimplePatchSelector"
#define YUISpecialWidget_Slider			"Slider"
#define YUISpecialWidget_Time			"Time"
#define YUISpecialWidget_TimeField		"TimeField"
#define YUISpecialWidget_Wizard			"Wizard"
#define YUISpecialWidget_TimezoneSelector	"TimezoneSelector"
#define YUISpecialWidget_Graph			"Graph"
#define YUISpecialWidget_ContextMenu		"ContextMenu"


// Widget properties

#define YUIProperty_Alive			"Alive"
#define YUIProperty_Cell			"Cell"
#define YUIProperty_ContextMenu			"ContextMenu"
#define YUIProperty_CurrentBranch		"CurrentBranch"
#define YUIProperty_CurrentButton		"CurrentButton"
#define YUIProperty_CurrentItem			"CurrentItem"
#define YUIProperty_CurrentSize			"CurrentSize"
#define YUIProperty_DebugLabel			"DebugLabel"
#define YUIProperty_EasterEgg			"EasterEgg"
#define YUIProperty_Enabled			"Enabled"
#define YUIProperty_EnabledItems		"EnabledItems"
#define YUIProperty_ExpectedSize		"ExpectedSize"
#define YUIProperty_Filename			"Filename"
#define YUIProperty_Layout			"Layout"
#define YUIProperty_HelpText			"HelpText"
#define YUIProperty_IconPath			"IconPath"
#define YUIProperty_InputMaxLength		"InputMaxLength"
#define YUIProperty_HWeight			"HWeight"
#define YUIProperty_HStretch			"HStretch"
#define YUIProperty_ID   			"ID"
#define YUIProperty_Item			"Item"
#define YUIProperty_Items			"Items"
#define YUIProperty_ItemStatus			"ItemStatus"
#define YUIProperty_Label			"Label"
#define YUIProperty_Labels			"Labels"
#define YUIProperty_LastLine			"LastLine"
#define YUIProperty_MaxLines			"MaxLines"
#define YUIProperty_MaxValue			"MaxValue"
#define YUIProperty_MinValue			"MinValue"
#define YUIProperty_MultiSelection		"MultiSelection"
#define YUIProperty_Notify			"Notify"
#define YUIProperty_OpenItems			"OpenItems"
#define YUIProperty_SelectedItems		"SelectedItems"
#define YUIProperty_Text			"Text"
#define YUIProperty_Timeout			"Timeout"
#define YUIProperty_ValidChars			"ValidChars"
#define YUIProperty_Value			"Value"
#define YUIProperty_Values			"Values"
#define YUIProperty_VisibleLines		"VisibleLines"
#define YUIProperty_VisibleItems                "VisibleItems"
#define YUIProperty_VWeight			"VWeight"
#define YUIProperty_VStretch			"VStretch"
#define YUIProperty_WidgetClass			"WidgetClass"
#define YUIProperty_VScrollValue		"VScrollValue"
#define YUIProperty_HScrollValue		"HScrollValue"


// Widget and dialog options

#define YUIOpt_animated				"animated"
#define YUIOpt_autoWrap                         "autoWrap"
#define YUIOpt_applyButton			"applyButton"
#define YUIOpt_autoScrollDown			"autoScrollDown"
#define YUIOpt_autoShortcut			"autoShortcut"
#define YUIOpt_boldFont				"boldFont"
#define YUIOpt_cancelButton			"cancelButton"
#define YUIOpt_centered				"centered"
#define YUIOpt_confirmUnsupported		"confirmUnsupported"
#define YUIOpt_customButton			"customButton"
#define YUIOpt_debugLayout			"debugLayout"
#define YUIOpt_decorated			"decorated"
#define YUIOpt_default				"default"
#define YUIOpt_defaultsize			"defaultsize"
#define YUIOpt_disabled				"disabled"
#define YUIOpt_easterEgg			"easterEgg"
#define YUIOpt_editable				"editable"
#define YUIOpt_helpButton			"helpButton"
#define YUIOpt_relNotesButton			"relNotesButton"
#define YUIOpt_hstretch				"hstretch"
#define YUIOpt_hvstretch			"hvstretch"
#define YUIOpt_immediate			"immediate"
#define YUIOpt_infocolor			"infocolor"
#define YUIOpt_invertAutoEnable			"invertAutoEnable"
#define YUIOpt_keepSorting			"keepSorting"
#define YUIOpt_keyEvents			"keyEvents"
#define YUIOpt_mainDialog			"mainDialog"
#define YUIOpt_multiSelection			"multiSelection"
#define YUIOpt_noAutoEnable			"noAutoEnable"
#define YUIOpt_notify				"notify"
#define YUIOpt_notifyContextMenu		"notifyContextMenu"
#define YUIOpt_onlineSearch			"onlineSearch"
#define YUIOpt_okButton				"okButton"
#define YUIOpt_outputField			"outputField"
#define YUIOpt_plainText			"plainText"
#define YUIOpt_recursiveSelection		"recursiveSelection"
#define YUIOpt_relaxSanityCheck			"relaxSanityCheck"
#define YUIOpt_repoMgr				"repoMgr"
#define YUIOpt_repoMode				"repoMode"
#define YUIOpt_scaleToFit			"scaleToFit"
#define YUIOpt_searchMode			"searchMode"
#define YUIOpt_shrinkable			"shrinkable"
#define YUIOpt_stepsEnabled			"stepsEnabled"
#define YUIOpt_summaryMode			"summaryMode"
#define YUIOpt_testMode				"testMode"
#define YUIOpt_tiled				"tiled"
#define YUIOpt_titleOnLeft			"titleOnLeft"
#define YUIOpt_treeEnabled			"treeEnabled"
#define YUIOpt_updateMode			"updateMode"
#define YUIOpt_vstretch				"vstretch"
#define YUIOpt_warncolor			"warncolor"
#define YUIOpt_wizardDialog			"wizardDialog"
#define YUIOpt_youMode				"youMode"
#define YUIOpt_zeroHeight			"zeroHeight"
#define YUIOpt_zeroWidth			"zeroWidth"

#define YUIOpt_key_F1				"key_F1"
#define YUIOpt_key_F2				"key_F2"
#define YUIOpt_key_F3				"key_F3"
#define YUIOpt_key_F4				"key_F4"
#define YUIOpt_key_F5				"key_F5"
#define YUIOpt_key_F6				"key_F6"
#define YUIOpt_key_F7				"key_F7"
#define YUIOpt_key_F8				"key_F8"
#define YUIOpt_key_F9				"key_F9"
#define YUIOpt_key_F10				"key_F10"
#define YUIOpt_key_F11				"key_F11"
#define YUIOpt_key_F12				"key_F12"
#define YUIOpt_key_F13				"key_F13"
#define YUIOpt_key_F14				"key_F14"
#define YUIOpt_key_F15				"key_F15"
#define YUIOpt_key_F16				"key_F16"
#define YUIOpt_key_F17				"key_F17"
#define YUIOpt_key_F18				"key_F18"
#define YUIOpt_key_F19				"key_F19"
#define YUIOpt_key_F20				"key_F20"
#define YUIOpt_key_F21				"key_F21"
#define YUIOpt_key_F22				"key_F22"
#define YUIOpt_key_F23				"key_F23"
#define YUIOpt_key_F24				"key_F24"
#define YUIOpt_key_none				"key_none"


// Predefined glyphs for builtin Glyph()
//
// - remember there must be a substitute that can be displayed in plain ASCII,
// so don't include just everything here that is included in Unicode / UTF-8!

#define YUIGlyph_ArrowLeft			"ArrowLeft"
#define YUIGlyph_ArrowRight			"ArrowRight"
#define YUIGlyph_ArrowUp			"ArrowUp"
#define YUIGlyph_ArrowDown			"ArrowDown"

#define YUIGlyph_CheckMark			"CheckMark"
#define YUIGlyph_BulletArrowRight		"BulletArrowRight"
#define YUIGlyph_BulletCircle			"BulletCircle"
#define YUIGlyph_BulletSquare			"BulletSquare"



// Display capabilities for GetDisplayInfo()

#define YUICap_Width				"Width"
#define YUICap_Height				"Height"
#define YUICap_Depth				"Depth"
#define YUICap_Colors				"Colors"
#define YUICap_DefaultWidth			"DefaultWidth"
#define YUICap_DefaultHeight			"DefaultHeight"
#define YUICap_TextMode				"TextMode"
#define YUICap_HasImageSupport			"HasImageSupport"
#define YUICap_HasAnimationSupport		"HasAnimationSupport"
#define YUICap_HasIconSupport			"HasIconSupport"
#define YUICap_HasFullUtf8Support		"HasFullUtf8Support"
#define YUICap_HasWidgetStyleSupport		"HasWidgetStyleSupport"
#define YUICap_HasWizardDialogSupport		"HasWizardDialogSupport"
#define YUICap_RichTextSupportsTable		"RichTextSupportsTable"
#define YUICap_LeftHandedMouse			"LeftHandedMouse"
#define YUICap_y2debug				"y2debug"



// Misc

#define YUISymbol_id				"id"
#define YUISymbol_opt				"opt"
#define YUISymbol_icon				"icon"
#define YUISymbol_sortKey			"sortKey"
#define YUISymbol_item				"item"
#define YUISymbol_cell				"cell"
#define YUISymbol_menu				"menu"
#define YUISymbol_header			"header"
#define YUISymbol_rgb				"rgb"
#define YUISymbol_leftMargin			"leftMargin"
#define YUISymbol_rightMargin			"rightMargin"
#define YUISymbol_topMargin			"topMargin"
#define YUISymbol_bottomMargin			"bottomMargin"
#define YUISymbol_BackgroundPixmap		"BackgroundPixmap"
#define YUISymbol_open                          "open"
#define YUISymbol_closed                        "closed"

#define YUISymbol_Left				"Left"
#define YUISymbol_Right				"Right"
#define YUISymbol_Center			"Center"


#endif // YUISymbols_h
