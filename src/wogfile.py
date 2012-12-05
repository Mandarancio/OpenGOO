from __future__ import with_statement
import sys, zlib, struct
import optparse
import os.path
import png

PLATFORM_WIN=0
PLATFORM_LINUX=1
PLATFORM_MAC=2

if sys.platform=='win32' or sys.platform=='cygwin':
    ON_PLATFORM = PLATFORM_WIN
elif sys.platform=='darwin':
    ON_PLATFORM = PLATFORM_MAC
else:
    ON_PLATFORM = PLATFORM_LINUX

#ON_PLATFORM=PLATFORM_MAC

#if ON_PLATFORM!=PLATFORM_MAC:
#    from Crypto.Cipher import AES

def pngbinltl2png(input_path,output_path):
    # repack .png.binltl files into png
    text = file( input_path, 'rb' ).read()
    width, height, size, fullsize = struct.unpack("<HHII", text[:12])
    data = zlib.decompress(text[12:12+size])
    side = 1
    while side < width or side < height:
      side *= 2
    def chunk(fh, type, data):
      check = type + data
      fh.write(struct.pack(">I", len(data)))
      fh.write(check)
      fh.write(struct.pack(">I", zlib.crc32(check) & 0xffffffff))

    # output png
    with file( output_path, 'wb' ) as fout:
        fout.write("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")
        chunk(fout,"IHDR", struct.pack(">II", width, height) + "\x08\x06\x00\x00\x00")
        chunk(fout, "IDAT", zlib.compress("\x00"+"\x00".join(*[(data[line*side*4:(line*side + width)*4] for line in range(height))]), 9))
        chunk(fout, "IEND", "")

    return width,height

#PNG
#Header \x89\x50\x4E\x47\x0D\x0A\x1A\x0A
#>I len(data) IHDR compressed
#IHDR{>II width,height\x08\x06\x00\x00\x00}
#>I checksum
#>I len(data) IDAT compressed
#IDAT{Image} \x00[Row]\x00[Row]
#>I checksum
#>I len(data) IEND 0
#IEND
#>I checksum

## copy header
# header = infile.read(8)
# if header != "\211PNG\r\n\032\n":
# raise IOError("not a valid PNG file")
# outfile.write(header)
#
# # copy chunks, until IEND
# while 1:
# chunk = infile.read(8)
# size, cid = struct.unpack("!l4s", chunk)
# outfile.write(chunk)
# outfile.write(infile.read(size))
# outfile.write(infile.read(4)) # checksum
# if cid == "IEND":
# break

def png2pngbinltl(input_path,output_path):
    r = png.Reader(filename=input_path)
    width,height,pixels,meta= r.asRGBA8()
    size = 1
    while size < width or size < height:
      size *= 2
    #print "width=",width,"height=",height,"size=",size
    rowend = ''.join([chr(0) for i in range((size-width)*4)])
    fullrow = ''.join([chr(0) for i in range(size*4)])
    imagelist=[]
    for row in pixels:
        imagelist.append(''.join([chr(v) for v in row])+rowend)
    for i in range(size-height):
        imagelist.append(fullrow)
    imagedata=''.join(imagelist)
    assert len(imagedata)==size*size*4
    cdata=zlib.compress(imagedata,9)
    #print len(imagedata),len(cdata)
    with file( output_path, 'wb' ) as fout:
        fout.write(struct.pack("<HHII",width,height,len(cdata),len(imagedata)))
        fout.write(cdata)

#.png.binltl
#>HHII  width, height, size, fullsize
#{Data} compressed

def make_aes_cipher():
    """Returns a new cipher object initialized with the correct key
       that can be used to encrypt/decrypt files."""
    # AES 192 bits keys used to encrypt/decrypt data
    key = '0D0607070C01080506090904060D030F03060E010E02070B'

    def key_as_binary( key ):
        """Converts the specified hexadecimal string into a byte string."""
        assert len(key) % 2 == 0
        binary_key = []
        for index in xrange(0,len(key),2):
            binary_key.append( chr(int(key[index:index+2],16)) )
        return ''.join( binary_key )
    
    binary_key = key_as_binary( key )
    cipher = AES.new(binary_key, AES.MODE_CBC)
    return cipher

def encrypt_file_data( output_path, xml_data ):
    """Encrypt the string xml_data into a .bin file output_path."""
    if ON_PLATFORM==PLATFORM_MAC:
        #print "XOR encrypting",output_path
        encrypted_data = XORencrypt(xml_data)
    else:
        cipher = make_aes_cipher()
        # adds filler so that input data length is a multiple of 16
        filler = '\xfd\xfd\xfd\xfd' + '\0' * 12
        filler_size = 16 - len(xml_data) % 16
        xml_data += filler[0:filler_size]
        # encrypt the data
        encrypted_data = cipher.encrypt( xml_data )

    file( output_path, 'wb' ).write( encrypted_data )
    return True

def encrypt_file( input_path, output_path ):
    """Encrypt XML file input_path into .bin file output_path using AES algorithm."""
    xml_data= file( input_path, 'rb' ).read()
    if encrypt_file_data( output_path, xml_data ):
       print 'Encrypted "%s" into "%s"' % (input_path, output_path)
    return True

## MAC XOR decrypt and encrypt functions taken from goocrypt.py by SoulTaker
def XORdecrypt(input):
    output = ''
    size = len(input)
    a = (((size & 1) << 6) | ((size & 2) << 3) | (size & 4)) ^ 0xab
    for c in input:
      output += chr(a ^ ord(c))
      a = ((a & 0x7f) << 1 | (a & 0x80) >> 7) ^ ord(c)
    return output

def XORencrypt(input):
    output = ''
    size = len(input)
    a = (((size & 1) << 6) | ((size & 2) << 3) | (size & 4)) ^ 0xab
    for c in input:
      output += chr(a ^ ord(c))
      a = ((a & 0x7f) << 1 | (a & 0x80) >> 7) ^ ord(output[-1])
    return output

def decrypt_file_data( input_path ):
    """Decrypt a .bin file input_path and return the corresponding XML. May raise IOError exception."""
    crypted_data = file( input_path, 'rb' ).read()

    if ON_PLATFORM==PLATFORM_MAC:
        #print "XOR decrypting",input_path
        xml_data =  XORdecrypt(crypted_data)
    else:
        cipher = make_aes_cipher()
        xml_data = cipher.decrypt( crypted_data )
        # clean-up the data. Usually has '\xfd\xfd\xfd\xfd\0' at the
        # end, but this may be truncated to first '\xfd' if
        # size is nearly a multiple of 16, though there will
        # always be at least one '\xfd'.
        zero_index = xml_data.find( '\0' )
        if zero_index != -1:
            xml_data = xml_data[:zero_index]
        fd_index = xml_data.find( '\xfd' )
        if fd_index != -1:
            xml_data = xml_data[:fd_index]
    return xml_data

def decrypt_file( input_path, output_path ):
    """Decrypt a .bin file input_path into .xml file output_path using AES algorithm."""
    xml_data = decrypt_file_data( input_path )
    file( output_path, 'wb' ).write( xml_data )
    print 'Decrypted "%s" into "%s"' % (input_path, output_path)
    return True

def main():
    parser = optparse.OptionParser( """
%prog --decrypt source-file [output-path][output-filename]
%prog --encrypt source-file [output-path][output-filename]

You must always supply the full path to the "source" file.
For the "output" you may supply...
Full path and filename
Path Only  (filename from source-file, extension converted to .xml or .bin)
{omitted}  (path and filename from source-file, extension changed to .xml or .bin)

""" )
    parser.add_option( '-d', '--decrypt', dest='decrypt', action="store_true", default = False,
                       help = 'Decrypt the input path into the output path' )
    parser.add_option( '-e', '--encrypt', dest='encrypt', action="store_true", default = False,
                       help = 'Encrypt the input path into the output path' )
    (options, args) = parser.parse_args()
    if len(args) == 2:
        if args[0] == args[1]:
            parser.error( 'Input path must be different from output path' )
    elif len(args)!=1:
         parser.error( 'You must specify the input path' )

    if options.decrypt:
        if len(args)==1:
            args.append(os.path.splitext(os.path.normpath(args[0]))[0]+".xml")
        if args[1][len(args[1])-4:]!='.xml':
            args[1]=os.path.join(args[1],os.path.splitext(os.path.split(args[0])[1])[0]+".xml")
        return decrypt_file( args[0], args[1] )
    elif options.encrypt:
        if len(args)==1:
            args.append(os.path.splitext(os.path.normpath(args[0]))[0]+".bin")
        if args[1][len(args[1])-4:]!='.bin':
            args[1]=os.path.join(args[1],os.path.splitext(os.path.split(args[0])[1])[0]+".bin")
        return encrypt_file( args[0], args[1] )
    else:
        parser.error( 'You must specify either --decrypt or --encrypt' )

if __name__ == '__main__':
    succeed = main()
    if not succeed:
        print 'Failed'
        sys.exit( 2 )
