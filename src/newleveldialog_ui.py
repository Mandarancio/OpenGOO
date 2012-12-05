# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'newleveldialog.ui'
#
# Created: Sun Jun 06 21:07:15 2010
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_NewLevelDialog(object):
    def setupUi(self, NewLevelDialog):
        NewLevelDialog.setObjectName("NewLevelDialog")
        NewLevelDialog.resize(313, 112)
        self.verticalLayout = QtGui.QVBoxLayout(NewLevelDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtGui.QLabel(NewLevelDialog)
        self.label.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedKingdom))
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.levelName = QtGui.QLineEdit(NewLevelDialog)
        self.levelName.setMaxLength(100)
        self.levelName.setObjectName("levelName")
        self.verticalLayout.addWidget(self.levelName)
        self.label_2 = QtGui.QLabel(NewLevelDialog)
        self.label_2.setObjectName("label_2")
        self.verticalLayout.addWidget(self.label_2)
        self.buttonBox = QtGui.QDialogButtonBox(NewLevelDialog)
        self.buttonBox.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedKingdom))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)
        self.label.setBuddy(self.levelName)

        self.retranslateUi(NewLevelDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), NewLevelDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), NewLevelDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(NewLevelDialog)

    def retranslateUi(self, NewLevelDialog):
        NewLevelDialog.setWindowTitle(QtGui.QApplication.translate("NewLevelDialog", "Create New Level", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("NewLevelDialog", "&New level name (directory name):", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("NewLevelDialog", "(Letters and Numbers only - Must begin with a letter)", None, QtGui.QApplication.UnicodeUTF8))

