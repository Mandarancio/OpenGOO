from distutils.core import setup
import py2exe

#setup(console=['wogfile.py', 'scanbinfile.py', 'scanxmlfile.py'],
#      windows=[{"script":"wogeditor.py"}],
#setup(console=['wogfile.py' , 'scanbinfile.py', 'scanxmlfile.py', "wogeditor.py" , 'icon_resources':[(1,'MyIcon.ico')]],
#      options={"py2exe":{"includes":["sip"]
#}})
setup(
        console=[
            {
              "script": "wogfile.py" ,
              "icon_resources": [(1,'images/wogedit.ico')]
            }
        ] ,
      options={
            "py2exe":{"includes":["sip"]}
      }
)