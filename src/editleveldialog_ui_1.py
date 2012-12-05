# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'editleveldialog.ui'
#
# Created: Sun Jun 06 21:07:15 2010
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_EditLevelDialog(object):
    def setupUi(self, EditLevelDialog):
        EditLevelDialog.setObjectName("EditLevelDialog")
        EditLevelDialog.resize(400, 282)
        EditLevelDialog.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedStates))
        self.verticalLayout = QtGui.QVBoxLayout(EditLevelDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtGui.QLabel(EditLevelDialog)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.levelList = QtGui.QListWidget(EditLevelDialog)
        self.levelList.setObjectName("levelList")
        self.verticalLayout.addWidget(self.levelList)
        self.buttonBox = QtGui.QDialogButtonBox(EditLevelDialog)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(EditLevelDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), EditLevelDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), EditLevelDialog.reject)
        QtCore.QObject.connect(self.levelList, QtCore.SIGNAL("itemDoubleClicked(QListWidgetItem*)"), EditLevelDialog.accept)
        QtCore.QMetaObject.connectSlotsByName(EditLevelDialog)

    def retranslateUi(self, EditLevelDialog):
        EditLevelDialog.setWindowTitle(QtGui.QApplication.translate("EditLevelDialog", "Select level to edit...", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("EditLevelDialog", "Select level to edit:", None, QtGui.QApplication.UnicodeUTF8))

