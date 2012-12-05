#!/bin/sh
rm -fr dist build
python setup.py py2app || exit 1
cd "dist/World of Goo Level Editor.app" || exit 1
mv Contents/Resources/files.xml.xml Contents/MacOS || exit 1
echo "--- Contents/Resources/__boot__.py
+++ Contents/Resources/__boot__.py
@@ -122,6 +122,7 @@
 def _run(*scripts):
     global __file__
     import os, sys, site
+    sys.path = [os.path.join(os.environ['RESOURCEPATH'], 'lib', 'python2.5', 'lib-dynload')] + sys.path
     sys.frozen = 'macosx_app'
     base = os.environ['RESOURCEPATH']
     site.addsitedir(base)
" | patch -p0 || exit 1
find . -name *_debug* | xargs rm
cd ..
ls -l