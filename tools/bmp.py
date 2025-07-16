#!/usr/bin/env python3

import argparse
import logging

class BitmapIn( object ):

    def __init__( self, in_bmp_f ):

        logger = logging.getLogger( 'bitmap.init' )

        # Seek past signature.
        in_bmp_f.seek( 2 )

        # Grab file size.
        self.bmp_sz = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap size: %d bytes', self.bmp_sz )

        # Seek past reserved.
        in_bmp_f.seek( 10 )

        # Grab pixel offset.
        self.px_offset = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap pixel offset: %d bytes', self.px_offset )

        # Grab header size.
        self.header_sz = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap header size: %d bytes', self.header_sz )

        # Grab image width.
        self.width = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap width: %d bytes', self.width )

        # Grab image height.
        self.height = int.from_bytes( in_bmp_f.read( 4 ), 'little' )
        logger.debug( 'bitmap height: %d bytes', self.height )

        self._f = in_bmp_f

    def pixels( self, draw_px_cb, pad_ln_cb ):

        for y in range( 0, self.height ):
            # Seek to the next line (the horizontal lines are backwards, so
            # start at the end of the file and processing horizontal lines
            # in reverse).
            self._f.seek(
                self.px_offset + (((self.height - 1) - y) * self.width) )

            # Process this row.
            for x in range( 0, self.width ):
                px = int.from_bytes( self._f.read( 1 ) )
                draw_px_cb( px )
            pad_ln_cb( y )

class ICNOut( object ):

    def __init__( self ):

        self.px_bytes = []
        self.mask_bytes = []
        self.px_byte_iter = 0
        self.mask_byte_iter = 0
        self.byte_sz = 0

    def pixel( self, px : int ):
        self.mask_byte_iter <<= 1
        self.px_byte_iter <<= 1
        if 1 < px:
            self.px_byte_iter |= 0x01
        if 0 < px:
            self.mask_byte_iter |= 0x01
        self.byte_sz += 1
        if 8 == self.byte_sz:
            self.px_bytes.append( self.px_byte_iter )
            self.mask_bytes.append( self.mask_byte_iter )
            self.px_byte_iter = 0
            self.mask_byte_iter = 0
            self.byte_sz = 0

    def write_bytes( self, bytes_arr, out_icn_f, in_hex ):
        idx = 0
        out_icn_f.write( '$"'.encode( 'utf-8' ) )
        for b in bytes_arr:
            # Iterate through the given byte array and write in hex or raw
            # as specified.
            if in_hex:
                if 0 == idx % 32 and 0 != idx:
                    out_icn_f.write( '"\n$"'.encode( 'utf-8' ) )
                out_icn_f.write( f"{b:02x}".encode( 'utf-8' ) )
            else:
                out_icn_f.write( b.to_bytes( 1 ) )
            idx += 1
        out_icn_f.write( '"\n'.encode( 'utf-8' ) )

    def line( self, y : int ):
        pass

def to_icn( in_bmp, out_icn, **args ):

    logger = logging.getLogger( 'to.icn' )

    # Open the bitmap and parse it.
    with open( in_bmp, 'rb' ) as in_bmp_f:
        bmp = BitmapIn( in_bmp_f )
        icn = ICNOut()
        bmp.pixels( icn.pixel, icn.line )
        logger.debug( f"output icn is {len( icn.px_bytes )} bytes" )
        with open( out_icn, 'wb' ) as out_icn_f:
            if args['hex']:
                out_icn_f.write(
                    'resource \'ICN#\' (128) {\n{\n'.encode( 'utf-8' ) )
            icn.write_bytes( icn.px_bytes, out_icn_f, args['hex'] )
            icn.write_bytes( icn.mask_bytes, out_icn_f, args['hex'] )
            if args['hex']:
                out_icn_f.write( '\n}\n};\n'.encode( 'utf-8' ) )

def main():

    parser = argparse.ArgumentParser()

    parser.add_argument( '-v', '--verbose', action='store_true' )

    subparsers = parser.add_subparsers( required=True )

    parser_to_icn = subparsers.add_parser( 'to_icn' )

    parser_to_icn.set_defaults( func=to_icn )

    parser_to_icn.add_argument( '-x', '--hex', action='store_true' )

    parser_to_icn.add_argument( 'in_bmp' )

    parser_to_icn.add_argument( 'out_icn' )

    args = parser.parse_args()

    # Setup logging.
    level = logging.WARNING
    if args.verbose:
        level = logging.DEBUG
    logging.basicConfig( level=level )
    logger = logging.getLogger( 'main' )

    args_arr = vars( args )
    args.func( **args_arr )

if '__main__' == __name__:
    main()

