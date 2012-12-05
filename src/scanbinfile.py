import wogfile
import os
import os.path
import glob
import optparse

def make_path_dirs( path ):
    """Creates the parent directory of the specified file path if required.
    """
    parent_dir = os.path.split(path)[0]
    if not os.path.exists( parent_dir ):
        os.makedirs( parent_dir )
        print 'Created directory: "%s"' % parent_dir
    return True        

def decrypt_dir_files( input_dir, output_dir ):
    """Decrypts all .bin files found in the directory and
    recurse in subdirectories."""
    for entry in os.listdir( input_dir ):
        entry_path = os.path.join( input_dir, entry )
        if os.path.isdir( entry_path ):
            if not decrypt_dir_files( entry_path,
                                      os.path.join( output_dir, entry ) ):
                return False
        elif os.path.splitext( entry )[1].lower() == '.bin':
            entry_xml = os.path.splitext( entry )[0]
            if not entry_xml.lower().endswith('.xml'):
                entry_xml += '.xml'
            output_path = os.path.join( output_dir, entry_xml )
            if not make_path_dirs( output_path ):
                return False
            if not wogfile.decrypt_file( entry_path, output_path ):
                return False
    return True

def main():
    parser = optparse.OptionParser( """%prog res-dir-path output-dir-path

This applications decrypt all *.bin files found in res-dir-path sub-directories,
and output them in output-dir-path after replacing the extension with .xml.
""" )
    (options, args) = parser.parse_args()
    if len(args) != 2:
        parser.error( 'You must specify the input and ouput path' )

    levels_dir, output_dir = args[0], args[1]
    if not os.path.isdir( levels_dir ):
        parser.error( '"%s" is not a directory' % levels_dir )
    if not os.path.isdir( output_dir ):
        parser.error( '"%s" is not a directory' % output_dir )

    if decrypt_dir_files( levels_dir, output_dir ):
        print 'Done.'
        return True
    else:
        return False

if __name__ == '__main__':
    succeed = main()
    if not succeed:
        print 'Failed'
        sys.exit( 2 )

