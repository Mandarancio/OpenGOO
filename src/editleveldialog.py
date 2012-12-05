# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'editleveldialog.ui'
#
# Created: Wed Sep 08 21:56:43 2010
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_EditLevelDialog(object):
    def setupUi(self, EditLevelDialog, all_levels,original_levels):

        EditLevelDialog.setObjectName("EditLevelDialog")
        EditLevelDialog.resize(400, 345)
        EditLevelDialog.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedStates))
        self.verticalLayout = QtGui.QVBoxLayout(EditLevelDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label = QtGui.QLabel(EditLevelDialog)
        self.label.setObjectName("label")
        self.horizontalLayout.addWidget(self.label)
        self.comboBox = QtGui.QComboBox(EditLevelDialog)
        self.comboBox.setObjectName("comboBox")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.comboBox.addItem("")
        self.horizontalLayout.addWidget(self.comboBox)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.levelList = QtGui.QListWidget(EditLevelDialog)
        self.levelList.setObjectName("levelList")
        self.verticalLayout.addWidget(self.levelList)
        self.buttonBox = QtGui.QDialogButtonBox(EditLevelDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)
        self.all_levels = all_levels
        self.original_levels = original_levels

        self.retranslateUi(EditLevelDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), EditLevelDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), EditLevelDialog.reject)
        QtCore.QObject.connect(self.levelList, QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem*)"), EditLevelDialog.accept)
        QtCore.QObject.connect(self.comboBox, QtCore.SIGNAL("currentIndexChanged(int)"), self.comboSelectionChanged)
        QtCore.QMetaObject.connectSlotsByName(EditLevelDialog)

        settings = QtCore.QSettings()
        settings.beginGroup( "MainWindow" )
        level_filter =  settings.value( "level_filter", 0 ).toInt()
        settings.endGroup()
        if level_filter[0]==0:
            self.comboSelectionChanged(0)
        else:
            self.comboBox.setCurrentIndex(level_filter[0])


    def retranslateUi(self, EditLevelDialog):
        EditLevelDialog.setWindowTitle(QtGui.QApplication.translate("EditLevelDialog", "Select level to edit...", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("EditLevelDialog", "Select level to edit:", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox.setItemText(0, QtGui.QApplication.translate("EditLevelDialog", "All Levels", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox.setItemText(1, QtGui.QApplication.translate("EditLevelDialog", "Custom Levels Only", None, QtGui.QApplication.UnicodeUTF8))
        self.comboBox.setItemText(2, QtGui.QApplication.translate("EditLevelDialog", "Original Levels Only", None, QtGui.QApplication.UnicodeUTF8))

    def comboSelectionChanged(self,index):
        self.levelList.clear()
        if index == 0:
            #All
          for level in sorted(self.all_levels,key=unicode.lower):
            self.levelList.addItem( level )
        elif index == 1:
            #Custom
            custom_levels = sorted(self.all_levels - self.original_levels,key=unicode.lower)
            for level in custom_levels:
              self.levelList.addItem( level )
        elif index == 2:
            #Original
          for level in sorted(self.original_levels,key=str.lower):
            self.levelList.addItem( level )

        else:
            print "unknown Combo Index",index
