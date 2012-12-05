# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'texteditdialog.ui'
#
# Created: Sun Jun 06 21:07:15 2010
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_TextEditDialog(object):
    def setupUi(self, TextEditDialog):
        TextEditDialog.setObjectName("TextEditDialog")
        TextEditDialog.setWindowModality(QtCore.Qt.ApplicationModal)
        TextEditDialog.resize(416, 184)
        TextEditDialog.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.verticalLayout = QtGui.QVBoxLayout(TextEditDialog)
        self.verticalLayout.setObjectName("verticalLayout")
        self.textEdit = QtGui.QTextEdit(TextEditDialog)
        self.textEdit.setObjectName("textEdit")
        self.verticalLayout.addWidget(self.textEdit)
        self.buttonBox = QtGui.QDialogButtonBox(TextEditDialog)
        self.buttonBox.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedKingdom))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Cancel|QtGui.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(TextEditDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), TextEditDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), TextEditDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(TextEditDialog)

    def retranslateUi(self, TextEditDialog):
        TextEditDialog.setWindowTitle(QtGui.QApplication.translate("TextEditDialog", "Edit Text", None, QtGui.QApplication.UnicodeUTF8))

