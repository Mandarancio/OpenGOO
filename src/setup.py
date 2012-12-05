"""
 py2app/py2exe build script for WooGLE.
 Usage (Windows):     python setup.py py2exe
 Usage (Mac OS X):    python setup.py py2app
 Usage (Other) :      Not Implemented (Yet)
"""

import sys
if sys.platform=='win32':
    from distutils.core import setup
    import py2exe
    setup( console=[{ "script": "wogeditor.py" ,
                      "icon_resources": [(1,'images/wogedit.ico')]}] ,
           options={ "py2exe":{"includes":["sip"]} } )
elif sys.platform=='darwin':
    from setuptools import setup
    APP = ['wogeditor.py']
    DATA_FILES = []
    OPTIONS = {'argv_emulation': True,
           'includes': ['sip', 'PyQt4', 'PyQt4.QtCore', 'PyQt4.QtGui'],
           'excludes': ['PyQt4.QtDesigner', 'PyQt4.QtNetwork', 'PyQt4.QtOpenGL', 'PyQt4.QtScript', 'PyQt4.QtSql', 'PyQt4.QtTest', 'PyQt4.QtWebKit', 'PyQt4.QtXml', 'PyQt4.phonon'],
           'iconfile': 'images/wogedit.icns',
           'resources': 'files.xml.xml',
           'plist': {'CFBundleDisplayName': 'WooGLE'}
           }
    setup(  app=APP,
            data_files=DATA_FILES,
            options={'py2app': OPTIONS},
            setup_requires=['py2app'])
else:
    print "setup.py not configured for this platform ",sys.platform